#include <utility>

//
// Created by lvcheng1 on 19-2-24.
//

#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <sys/time.h>

#include "work_thread.h"

namespace forward {

WorkThread::Connector::Connector(const std::function<int(const int)> &disconnect_callback, const int &fd) : disconnect_callback_(disconnect_callback), is_connecting_(true), fd_(fd) {}

WorkThread::Connector::~Connector() {
  WorkThread::Connector::Shutdown();
  auto ret = WorkThread::Connector::disconnect_callback_(fd_);
  if (ret != 0) {
    log_err("disconnect callback error");
  }
  close(fd_);
}

WorkThread::WorkThread()
    : thread_id_(0),
      clean_interval_(DEF_CLEAN_INTERVAL),
      heart_beat_s_(DEF_HEART_BEAT),
      time_wheel_scale_(0),
      time_wheel_(static_cast<unsigned long>(heart_beat_s_)),
      quit_(true),
      ep_ptr_(new Epoll(FORWARD_MAX_CLIENTS_NUM)),
      thread_ptr_(nullptr) {
}

WorkThread::~WorkThread() {
  Quit();
  thread_ptr_->join();
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
    if (when.tv_sec > now.tv_sec || (when.tv_sec == now.tv_sec && when.tv_usec > now.tv_usec)) {
      time_out_ms = static_cast<int>((when.tv_sec - now.tv_sec) * 1000 + (when.tv_usec - now.tv_usec) / 1000);
    } else {
      CleanUpExpiredConnection();
      time_wheel_scale_ = (time_wheel_scale_ + 1) % heart_beat_s_;
      when.tv_sec = now.tv_sec + (clean_interval_ / 1000);
      when.tv_usec =
          now.tv_usec + ((clean_interval_ % 1000) * 1000);
      time_out_ms = clean_interval_;
    }

    char buf[DEF_MAX_MSG_LEN];
    std::vector<std::pair<int, uint32_t >> read_list;
    ret = ep_ptr_->Wait(&read_list, time_out_ms);
    if (ret != 0) {
      log_err("epoll wait error");
    }
    for (auto &item : read_list) {
      int fd = item.first;
      uint32_t events = item.second;
      if (events & EPOLLERR || events & EPOLLHUP) {
        ep_ptr_->DelEvent(fd);
        DelConn(fd);
        close(fd);
      } else if (events & EPOLLIN) {
        /*
         * TODO: to be implemented
         * Function: Work
         */
        ret = static_cast<int>(read(fd, buf, DEF_MAX_MSG_LEN));
        if (ret < 0) {
          log_err("read error");
        }
        std::string msg = std::string(buf) + "thread_id: " + std::to_string(pthread_self());
        ret = static_cast<int>(write(fd, msg.c_str(), msg.size()));

        if (ret < 0) {
          log_err("write error");
        }
        /*
         * End TODO
         */

        NewConn(fd);
      }
    }
  }
}

void WorkThread::NewConn(const int &conn_fd) {
  /*
   * There may be dirty data in the map (the connection entity is not properly destructed, but the client is disconnected)
   */
  if (fd_connector_map_.find(conn_fd) == fd_connector_map_.end() || !fd_connector_map_[conn_fd].lock()->isIs_connecting_()) {
    std::shared_ptr<Connector> tmp = std::make_shared<WorkThread::Connector>(std::bind(&WorkThread::DelConn, this, conn_fd), conn_fd);
    fd_connector_map_.insert(std::make_pair(conn_fd, tmp));
    time_wheel_[time_wheel_scale_].push_back(tmp);
  } else {
    time_wheel_[time_wheel_scale_].push_back(fd_connector_map_[conn_fd].lock());
  }
}

int WorkThread::DelConn(const int &conn_fd) {
  fd_connector_map_.erase(conn_fd);
  return ep_ptr_->DelEvent(conn_fd);
}

void WorkThread::CleanUpExpiredConnection() {
  time_wheel_[(time_wheel_scale_ + 1) % heart_beat_s_].clear();
}

void WorkThread::Start() {
  quit_.store(false);
  thread_ptr_.reset(new std::thread(std::bind(&WorkThread::ThreadMain, this)));
  thread_id_ = thread_ptr_->get_id();
}

void WorkThread::Quit() {
  quit_.store(true);
}

int WorkThread::AcceptWork(const int &work_fd_, const int &events) {
  return ep_ptr_->AddEvent(work_fd_, events);
}

};
