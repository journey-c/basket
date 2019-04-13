#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "forward/include/dispatch_thread.h"
#include "forward/include/forward_tools.h"

namespace forward {

DispatchThread::DispatchThread(const std::string &ip, const int &port, const int &work_num, forward::ConnFactory *conn_factory)
    : ip_(ip),
      port_(port),
      work_num_(work_num),
      time_out_ms_(EPOLL_DEFINE_TIME_OUT),
      distribution_pointer_(0),
      quit_(true),
      socket_ptr_(new Socket(port_, false)),
      epoll_ptr_(new Epoll(FORWARD_MAX_CLIENTS_NUM)),
      thread_ptr_(nullptr) {
  work_threads_ = new WorkThread *[work_num_];
  for (int i = 0; i < work_num; ++i) {
    work_threads_[i] = new WorkThread(conn_factory);
  }
}

DispatchThread::~DispatchThread() {
  Quit();
  thread_ptr_->join();
  for (int i = 0; i < work_num_; ++i) {
    delete (work_threads_[i]);
  }
  delete (work_threads_);
  delete (socket_ptr_);
  delete (epoll_ptr_);
  delete (thread_ptr_);
}

int DispatchThread::HandlingNewConnection(int conn_fd, std::string ip, uint16_t port) {
  std::queue<ConnNotify> *q = &(work_threads_[distribution_pointer_]->conn_queue_);
  ConnNotify tmp_notify = ConnNotify{conn_fd, ip, port};
  {
    std::lock_guard<std::mutex> guard(work_threads_[distribution_pointer_]->conn_queue_mutex);
    q->push(tmp_notify);
  }
  auto ret = write(work_threads_[distribution_pointer_]->getNotify_send_fd_(), "", 1);
  distribution_pointer_++;
  distribution_pointer_ %= work_num_;
  if (ret == -1) return -1;
  return 0;
}

void DispatchThread::ThreadMain() {
  int ret;
  for (int i = 0; i < work_num_; ++i) {
    work_threads_[i]->Start();
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
          int accept_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
          SetNonBlocking(accept_fd);
          std::string ip(inet_ntoa(client_addr.sin_addr));
          uint16_t port = ntohs(client_addr.sin_port);

          HandlingNewConnection(accept_fd, ip, port);
        }
      }
    }
  }
}

void DispatchThread::Start() {
  quit_.store(false);
  thread_ptr_ = new std::thread(std::bind(&DispatchThread::ThreadMain, this));
}

void DispatchThread::Quit() {
  quit_.store(true);
  thread_ptr_->join();
}
};
