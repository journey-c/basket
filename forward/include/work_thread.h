#ifndef FORWARD_INCLUDE_WORK_THREAD_H_
#define FORWARD_INCLUDE_WORK_THREAD_H_

#include <map>
#include <thread>
#include <atomic>
#include <functional>

#include "forward/include/forward_epoll.h"
#include "forward/include/forward_define.h"
#include "forward/include/forward_conn.h"

namespace forward {

class ForwardConn;
class ConnFactory;

class WorkThread {
 public:
  explicit WorkThread(ConnFactory *conn_factory);
  virtual ~WorkThread();

  const std::string &getThread_name() const {
    return thread_name;
  }
  void setThread_name(const std::string &thread_name) {
    WorkThread::thread_name = thread_name;
  }
  void setConn_factory_(ConnFactory *conn_factory_) {
    WorkThread::conn_factory_ = conn_factory_;
  }

  int AcceptWork(const int work_fd_, const int events, const std::string &ip, const int16_t port);
  int DelConn(const int &conf_fd);
  void CleanUpExpiredConnection();
  void Start();
  void Quit();

 private:
  void ThreadMain();

  std::string thread_name;
  std::thread::id thread_id_;
  int clean_interval_;
  int time_wheel_size_;
  int time_wheel_scale_;
  std::map<int, std::weak_ptr<forward::ForwardConn>> fd_connector_map_;
  std::vector<std::vector<std::shared_ptr<forward::ForwardConn>>> time_wheel_;
  std::atomic<bool> quit_;
  Epoll *ep_ptr_;
  std::thread *thread_ptr_;

  ConnFactory *conn_factory_;
/*
 * not copy and copy assign
 */
  WorkThread(const WorkThread &);
  void operator=(const WorkThread &);
};

};

#endif // FORWARD_INCLUDE_WORK_THREAD_H_
