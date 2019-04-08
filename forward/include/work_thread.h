#ifndef FORWARD_INCLUDE_WORK_THREAD_H_
#define FORWARD_INCLUDE_WORK_THREAD_H_

#include <atomic>
#include <functional>
#include <map>
#include <thread>

#include "forward/include/forward_conn.h"
#include "forward/include/forward_define.h"
#include "forward/include/forward_epoll.h"

namespace forward {

class ForwardConn;
class ConnFactory;

struct ConnNotify {
  int conn_fd_;
  std::string ip_;
  int16_t port;
};

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
  
  
  int AcceptWork(const int work_fd_, const int events, const std::string &ip,
                 const int16_t port);
  int DelConn(const int &conf_fd);
  void CleanUpExpiredConnection();
  void Start();
  void Quit();

  /*
   * conn queue is used to receive the connection allocated by the  dispatcher thread
   */

  std::mutex conn_queue_mutex;
  std::queue<ConnNotify> conn_queue_;

  int getNotify_send_fd_() {
    return notify_send_fd_;
  }
  int getNotify_receive_fd_() {
    return notify_receive_fd_;
  }
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

  /*
   * These two fd are used to communicate with the dispatcher thread
   */
  int notify_send_fd_;
  int notify_receive_fd_;
    
  
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

#endif  // FORWARD_INCLUDE_WORK_THREAD_H_
