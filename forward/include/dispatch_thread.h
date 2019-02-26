//
// Created by lvcheng1 on 19-2-24.
//

#ifndef SNAPPY_SERVER_DISPATCH_THREAD_H
#define SNAPPY_SERVER_DISPATCH_THREAD_H

#include "work_thread.h"
#include "forward_socket.h"

namespace forward {

class DispatchThread {
 public:
  DispatchThread(const int &port, const int &work_num);
  ~DispatchThread();

  void Quit();

 private:
  void ThreadMain();

  int port_;
  int listen_fd_;
  int work_num_;
  int time_out_ms_;
  int distribution_pointer_;
  std::atomic<bool> quit_;
  std::unique_ptr<Socket> socket_ptr_;
  std::unique_ptr<std::thread> thread_ptr_;
  std::unique_ptr<Epoll> epoll_ptr_;
  std::unique_ptr<WorkThread[]> work_threads_ptr_;

  /*
   * Not copy and copy assign
   */
  DispatchThread(const DispatchThread &);
  void operator=(const DispatchThread &);
};

};

#endif //SNAPPY_SERVER_DISPATCH_THREAD_H
