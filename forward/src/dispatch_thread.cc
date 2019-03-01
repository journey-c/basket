//
// Created by lvcheng1 on 19-2-24.
//
#include <iostream>

#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#include "dispatch_thread.h"

namespace forward {

static int SetNonBlock(int sockfd) {
  int flags;
  if ((flags = fcntl(sockfd, F_GETFL, 0)) < 0) {
    close(sockfd);
    return -1;
  }

  flags |= O_NONBLOCK;
  if (fcntl(sockfd, F_SETFL, flags) < 0) {
    close(sockfd);
    return -1;
  }
  return flags;
}

DispatchThread::DispatchThread(const std::string &ip, const int &port, const int &work_num, bool is_block)
    : ip_(ip),
      port_(port),
      work_num_(work_num),
      time_out_ms_(EPOLL_DEFINE_TIME_OUT),
      distribution_pointer_(0),
      quit_(true),
      socket_ptr_(new Socket(port_, is_block)),
      epoll_ptr_(new Epoll(FORWARD_MAX_CLIENTS_NUM)),
      work_threads_ptr_(new WorkThread[work_num_]),
      thread_ptr_(nullptr) {
}

DispatchThread::~DispatchThread() {
  Quit();
  thread_ptr_->join();
}

void DispatchThread::ThreadMain() {
  int ret;
  for (int i = 0; i < work_num_; ++i) {
    work_threads_ptr_[i].Start();
  }
  struct sockaddr_in client_addr = {0};
  socklen_t client_len = sizeof(client_addr);

  socket_ptr_->Listen(ip_);
  listen_fd_ = socket_ptr_->getSock_fd_();
  if (listen_fd_ < 0) {
    log_err("listen error");
  }

  ret = epoll_ptr_->AddEvent(socket_ptr_->getSock_fd_(), EPOLLIN);
  if (ret != 0) {
    log_err("add event error");
  }

  while (!quit_) {
    std::vector<std::pair<int, uint32_t> > read_list;
    ret = epoll_ptr_->Wait(&read_list, time_out_ms_);
    if (ret != 0) {
      log_err("wait error")
    }
    for (auto &item : read_list) {
      int fd = item.first;
      uint32_t events = item.second;
      if (events & EPOLLERR || events & EPOLLHUP) {
        log_err("Epoll has error\n");
      } else if (fd == listen_fd_) {
        if (events & EPOLLIN) {
          int accept_fd = accept(fd, (struct sockaddr *) &client_addr, &client_len);
          SetNonBlock(accept_fd);
          work_threads_ptr_[distribution_pointer_++].AcceptWork(accept_fd, EPOLLIN);
          distribution_pointer_ %= work_num_;
        }
      }
    }
  }
}

void DispatchThread::Start() {
  quit_.store(false);
  thread_ptr_.reset(new std::thread(std::bind(&DispatchThread::ThreadMain, this)));
  for (;;) {

  }
}

void DispatchThread::Quit() {
  quit_.store(true);
}

};