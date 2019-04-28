#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <unistd.h>

#include "forward/include/forward_tools.h"
#include "forward/include/work_thread.h"

namespace forward {

WorkThread::WorkThread(ConnFactory *conn_factory)
    : thread_id_(0),
      clean_interval_(DEF_CLEAN_INTERVAL),
      time_wheel_size_(DEF_TIME_WHEEL_SIZE),
      time_wheel_scale_(0),
      time_wheel_(static_cast<unsigned long>(time_wheel_size_)),
      quit_(true),
      ep_ptr_(new Epoll(FORWARD_MAX_CLIENTS_NUM)),
      thread_ptr_(nullptr),
      conn_factory_(conn_factory) {
  int fds[2];
  /*
   * close-on-exec and sync
   */
  if (pipe(fds)) {
    log_warn("create pipe failed");
    exit(-1);
  }
  notify_receive_fd_ = fds[0];
  notify_send_fd_ = fds[1];
  if (ep_ptr_->AddEvent(notify_receive_fd_, EPOLLIN | EPOLLERR | EPOLLHUP) == -1) {
    log_warn("add notify fd failed");
    exit(-1);
  }
}

WorkThread::~WorkThread() {
  Quit();
  thread_ptr_->join();
  close(notify_send_fd_);
  close(notify_receive_fd_);
  delete (ep_ptr_);
  delete (thread_ptr_);
}

void WorkThread::ThreadMain() {
  int ret;
  struct timeval when = {0};
  gettimeofday(&when, nullptr);
  struct timeval now = when;
  ConnNotify cn;

  when.tv_sec += (clean_interval_ / 1000);
  when.tv_usec += ((clean_interval_ % 1000) * 1000);
  int time_out_ms = clean_interval_;
  if (time_out_ms < 0) {
    log_warn("heartbeat time is illegal");
  }

  while (!quit_) {
    gettimeofday(&now, nullptr);
    if (when.tv_sec > now.tv_sec || (when.tv_sec == now.tv_sec && when.tv_usec > now.tv_usec)) {
      time_out_ms = static_cast<int>((when.tv_sec - now.tv_sec) * 1000 + (when.tv_usec - now.tv_usec) / 1000);
    } else {
      CleanUpExpiredConnection();
      time_wheel_scale_ = (time_wheel_scale_ + 1) % time_wheel_size_;
      when.tv_sec = now.tv_sec + (clean_interval_ / 1000);
      when.tv_usec = now.tv_usec + ((clean_interval_ % 1000) * 1000);
      time_out_ms = clean_interval_;
    }

    std::vector<std::pair<int, uint32_t>> read_list;
    ret = ep_ptr_->Wait(&read_list, time_out_ms);
    if (ret != 0) {
      log_warn("epoll wait error");
    }
    for (auto &item : read_list) {
      int fd = item.first;
      uint32_t events = item.second;
      if (fd == notify_receive_fd_) {  // new connection
        if (events & EPOLLIN) {
          char tmp[2048];
          int32_t num_of_new_conn = read(notify_receive_fd_, tmp, 2048);
          if (num_of_new_conn <= 0) {
            continue;
          }
          for (int32_t idx = 0; idx != num_of_new_conn; ++idx) {
            {
              std::lock_guard<std::mutex> guard(conn_queue_mutex);
              cn = conn_queue_.front();
              conn_queue_.pop();
            }
            auto tmp = std::shared_ptr<forward::ForwardConn>(conn_factory_->NewConn(cn.conn_fd_, cn.ip_, cn.port_, this));

            fd_connector_map_[cn.conn_fd_] = tmp;
            time_wheel_[time_wheel_scale_].insert(std::make_pair(cn.conn_fd_, tmp));
            fd_connector_map_[cn.conn_fd_].lock()->setLast_active_time_(GetNowMillis());
            fd_connector_map_[cn.conn_fd_].lock()->setLast_time_wheel_scale_(time_wheel_scale_);

            if (ep_ptr_->AddEvent(cn.conn_fd_, EPOLLIN) == -1) {
              log_warn("AddEvent error");
              continue;
            }
          }
        }
      } else {
        if (fd_connector_map_[fd].lock()) {
          if (events & EPOLLIN) {
            ret = fd_connector_map_[fd].lock()->GetRequest();
            if (ret != kReadAll && ret != kReadHalf) {
              // kReadErr kReadClose kParseErr
              int ret = fd_connector_map_[fd].lock()->ClearUp("The client is disconnected abnormally");
              if (ret) {
                log_warn("ClearUp error");
              }  
              time_wheel_[fd_connector_map_[fd].lock()->getLast_time_wheel_scale_()].erase(fd);
              log_warn("GetRequest error");
              continue;
            }
          }

          if (fd_connector_map_[fd].lock()->isIs_reply_()) {
            ret = fd_connector_map_[fd].lock()->SendReply();
            if (ret != kWriteAll && ret != kWriteHalf) {
              // kWriteErr
              int ret = fd_connector_map_[fd].lock()->ClearUp("The client is disconnected abnormally");
              if (ret) {
                log_warn("ClearUp error");
              }
              time_wheel_[fd_connector_map_[fd].lock()->getLast_time_wheel_scale_()].erase(fd);
              log_warn("SendReply error");
              continue;
            }
          }

          if (events & EPOLLERR || events & EPOLLHUP) {
            int ret = fd_connector_map_[fd].lock()->ClearUp("The client is disconnected abnormally");
            if (ret) {
              log_warn("ClearUp error");
            }
            time_wheel_[fd_connector_map_[fd].lock()->getLast_time_wheel_scale_()].erase(fd);
            continue;
          }

          int32_t hb = fd_connector_map_[fd].lock()->getHeart_beat_();
          int32_t last_time_wheel_scale = fd_connector_map_[fd].lock()->getLast_time_wheel_scale_();
          if (last_time_wheel_scale != time_wheel_scale_ + hb) {
            /*
             * The heartbeat strategy:
             * if the client uploads the heartbeat time, it uses the client's,
             * otherwise it uses the default.
             */
            time_wheel_[(time_wheel_scale_ + hb) % time_wheel_size_].insert(
                std::make_pair(fd_connector_map_[fd].lock()->getFd_(), fd_connector_map_[fd].lock()));
            /*
             * Remove the previous pointer inserted in the time wheel to save
             * memory
             */
            time_wheel_[last_time_wheel_scale].erase(fd);
          }
          fd_connector_map_[fd].lock()->setLast_active_time_(GetNowMillis());
          fd_connector_map_[fd].lock()->setLast_time_wheel_scale_((time_wheel_scale_ + hb) % time_wheel_size_);
        } else {
          log_warn("expire");
        }
      }
    }
  }
}

int WorkThread::DelConn(const int conn_fd) {
  fd_connector_map_.erase(conn_fd);
  return ep_ptr_->DelEvent(conn_fd);
}

void WorkThread::CleanUpExpiredConnection() {
  time_wheel_[(time_wheel_scale_ + 1) % time_wheel_size_].clear();
}

void WorkThread::Start() {
  quit_.store(false);
  thread_ptr_ = new std::thread(std::bind(&WorkThread::ThreadMain, this));
  thread_id_ = thread_ptr_->get_id();
}

void WorkThread::Quit() {
  quit_.store(true);
}
};
