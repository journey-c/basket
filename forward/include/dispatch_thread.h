#ifndef FORWARD_INCLUDE_DISPATCH_THREAD_H_
#define FORWARD_INCLUDE_DISPATCH_THREAD_H_

#include "forward/include/work_thread.h"
#include "forward/include/forward_socket.h"

namespace forward {

class WorkThread;
class ForwardConn;
class ConnFactory;

class DispatchThread {
 public:
  DispatchThread(const std::string &ip_port,
                 const int &port,
                 const int &work_num,
                 forward::ConnFactory *conn_factory);
  virtual ~DispatchThread();
  int HandlingNewConnection(int conn_fd, std::string ip, uint16_t port);
  void Start();
  void Quit();

 private:
  void ThreadMain();

  std::string ip_;
  int port_;
  int listen_fd_;
  int work_num_;
  int time_out_ms_;
  int distribution_pointer_;
  std::atomic<bool> quit_;
  Socket* socket_ptr_;
  Epoll* epoll_ptr_;
  WorkThread** work_threads_;
  std::thread* thread_ptr_;

  /*
   * Not copy and copy assign
   */
  DispatchThread(const DispatchThread &);
  void operator=(const DispatchThread &);
};

};

#endif // FORWARD_INCLUDE_DISPATCH_THREAD_H_
