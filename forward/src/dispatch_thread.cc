//
// Created by lvcheng1 on 19-2-24.
//

#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "dispatch_thread.h"

namespace forward {

DispatchThread::DispatchThread(const int &port, const int &work_num)
    : port_(port),
      work_num_(work_num),
      quit_(false),
      time_out_ms_(EPOLL_DEFINE_TIME_OUT),
      socket_ptr_(new Socket(port_, false)),
      distribution_pointer_(0),
      thread_ptr_(new std::thread(std::bind(&DispatchThread::ThreadMain, this))),
      epoll_ptr_(new Epoll),
      work_threads_ptr_(new WorkThread[work_num_]) {
}

DispatchThread::~DispatchThread() {

}

void DispatchThread::ThreadMain() {
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(struct sockaddr);

  socket_ptr_->Listen("127.0.0.1");
  listen_fd_ = socket_ptr_->getSock_fd_();
  epoll_ptr_->AddEvent(socket_ptr_->getSock_fd_(), EPOLLIN);
  quit_ = false;
  while (quit_) {
    std::vector<std::pair<int, uint32_t> > read_list;
    int wait_num = epoll_ptr_->Wait(& read_list, time_out_ms_);
    for (int i = 0; i < wait_num; ++i) {
      int fd = read_list[i].first;
      uint32_t events = read_list[i].second;
      if (fd == listen_fd_) {
        if (events & EPOLLHUP)
          continue;
        if (events & EPOLLERR)
          continue;
        if (events & EPOLLIN) {
          int accept_fd = accept(fd, (struct sockaddr *) &client_addr, &client_len);
          work_threads_ptr_[i].Acceptwork(accept_fd, EPOLLIN);
        }

      }
    }
  }
}

void DispatchThread::Quit() {
  quit_ = true;
}

};