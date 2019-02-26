//
// Created by lvcheng1 on 19-2-24.
//

#include <sys/epoll.h>
#include <unistd.h>

#include "work_thread.h"

namespace forward {

WorkThread::WorkThread()
    : quit_(false),
      time_out_ms_(EPOLL_DEFINE_TIME_OUT),
      thread_ptr_(new std::thread(std::bind(&WorkThread::ThreadMain, this))),
      ep_ptr_(new Epoll) {
  thread_id_ = thread_ptr_->get_id();
}

WorkThread::~WorkThread() {

}

void WorkThread::ThreadMain() {
  while (!quit_) {
    ssize_t status;
    char buf[1000];
    std::vector<std::pair<int, uint32_t >> read_list;

    int wait_num = ep_ptr_->Wait(&read_list, time_out_ms_);
    if (status != 0) {
      log_warn("epoll wait error");
      abort();
    }
    for (int i = 0; i < wait_num; ++i) {
      int fd = read_list[i].first;
      uint32_t events = read_list[i].second;
      if (events & EPOLLIN) {
        status = read(fd, buf, sizeof(buf));
        if (status) log_err("read error");

        status = write(fd, buf, sizeof(buf));
        if (status) log_err("write error");
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
