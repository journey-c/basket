//
// Created by lvcheng1 on 19-2-24.
//

#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>

#include "work_thread.h"

namespace forward {

WorkThread::WorkThread()
    : thread_id_(0),
      time_out_ms_(EPOLL_DEFINE_TIME_OUT),
      quit_(true),
      ep_ptr_(new Epoll(FORWARD_MAX_CLIENTS_NUM)),
      thread_ptr_(nullptr) {
}

WorkThread::~WorkThread() {
  Quit();
}

void WorkThread::Start() {
  quit_ = false;
  thread_ptr_.reset(new std::thread(std::bind(&WorkThread::ThreadMain, this)));
  thread_id_ = thread_ptr_->get_id();
}

void WorkThread::ThreadMain() {
  int ret;
  while (!quit_) {
    char buf[1000];
    std::vector<std::pair<int, uint32_t >> read_list;
    ret = ep_ptr_->Wait(&read_list, time_out_ms_);
    if (ret != 0) {
      log_err("epoll wait error");
    }
    for (auto &item : read_list) {
      int fd = item.first;
      uint32_t events = item.second;
      if (events & EPOLLERR || events & EPOLLHUP) {
        ep_ptr_->DelEvent(fd);
      } else if (events & EPOLLIN) {
        ret = static_cast<int>(read(fd, buf, sizeof(buf) / sizeof(buf[0])));
        if (ret < 0) {
          std::cout << ret << std::endl;
          log_err("read error");
        }
        std::string msg = std::string(buf) + "thread_id: " + std::to_string(pthread_self());
        ret = static_cast<int>(write(fd, msg.c_str(), msg.size()));

        if (ret < 0) {
          log_err("write error");
        }
      }
    }
  }
}

void WorkThread::Quit() {
  quit_ = true;
}

int WorkThread::Acceptwork(const int &work_fd_, const int &events) {
  return ep_ptr_->AddEvent(work_fd_, events);
}

};
