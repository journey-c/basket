#include <sys/epoll.h>
#include <sys/time.h>
#include <unistd.h>

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
}

WorkThread::~WorkThread() {
  Quit();
  thread_ptr_->join();
  delete (ep_ptr_);
  delete (thread_ptr_);
}

void WorkThread::ThreadMain() {
  int ret;
  struct timeval when = {0};
  gettimeofday(&when, nullptr);
  struct timeval now = when;

  when.tv_sec += (clean_interval_ / 1000);
  when.tv_usec += ((clean_interval_ % 1000) * 1000);
  int time_out_ms = clean_interval_;
  if (time_out_ms < 0) {
    log_err("heartbeat time is illegal");
  }

  while (!quit_) {
    gettimeofday(&now, nullptr);
    if (when.tv_sec > now.tv_sec ||
        (when.tv_sec == now.tv_sec && when.tv_usec > now.tv_usec)) {
      time_out_ms = static_cast<int>((when.tv_sec - now.tv_sec) * 1000 +
                                     (when.tv_usec - now.tv_usec) / 1000);
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
      log_err("epoll wait error");
    }
    for (auto &item : read_list) {
      int fd = item.first;
      uint32_t events = item.second;
      if (events & EPOLLERR || events & EPOLLHUP) {
        DelConn(fd);
        close(fd);
      } else if (events & EPOLLIN) {
        if (fd_connector_map_[fd].lock()) {
          ret = fd_connector_map_[fd].lock()->GetRequest();
          if (ret) {
            DelConn(fd);
            close(fd);
            log_warn("GetRequest error");
          }
          int32_t hb = fd_connector_map_[fd].lock()->getHeart_beat_();
          /*
           * The heartbeat strategy:
           * if the client uploads the heartbeat time, it uses the client's,
           * otherwise it uses the default.
           */
          time_wheel_[(time_wheel_scale_ + hb) % time_wheel_size_].push_back(
              fd_connector_map_[fd].lock());
          if (fd_connector_map_[fd].lock()->isIs_reply_()) {
            if (events & EPOLLOUT) {
              ret = fd_connector_map_[fd].lock()->SendReply();
              if (ret) {
                DelConn(fd);
                close(fd);
                log_warn("SendReply error");
              }
            }
          }
        } else {
          log_warn("expire");
        }
      }
    }
  }
}

int WorkThread::DelConn(const int &conn_fd) {
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

int WorkThread::AcceptWork(const int work_fd_, const int events,
                           const std::string &ip, const int16_t port) {
  if (WorkThread::conn_factory_ != nullptr) {
    auto tmp = std::shared_ptr<forward::ForwardConn>(
        conn_factory_->NewConn(work_fd_, ip, port, this));
    fd_connector_map_[work_fd_] = tmp;
    time_wheel_[time_wheel_scale_].push_back(tmp);
    return ep_ptr_->AddEvent(work_fd_, events);
  } else {
    log_warn("conn factory is null");
    return -1;
  }
}
};
