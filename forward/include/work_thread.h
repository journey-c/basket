//
// Created by lvcheng1 on 19-2-24.
//

#ifndef SNAPPY_SERVER_WORK_THREAD_H
#define SNAPPY_SERVER_WORK_THREAD_H

#include <map>
#include <thread>
#include <atomic>

#include <functional>

#include "forward_epoll.h"
#include "forward_define.h"

namespace forward {

class WorkThread;

class WorkThread {
 public:
  WorkThread();
  virtual ~WorkThread();

  void setHeart_beat__s_(int heart_beat_ms_) {
    WorkThread::heart_beat_s_ = heart_beat_s_;
  }
  int AcceptWork(const int &word_fd_, const int &events);
  void NewConn(const int &conn_fd);
  int DelConn(const int &conf_fd);
  void CleanUpExpiredConnection();
  void Start();
  void Quit();

 private:
  class Connector {
   public:
    Connector(const std::function<int(const int)> &call_back, const int &fd);
    ~Connector();

    void Shutdown() {
      is_connecting_.store("false");
    }
    bool isIs_connecting_() const {
      return is_connecting_;
    }
    void setDisconnect_callback_(const std::function<int(const int)> &disconnect_callback_) {
      Connector::disconnect_callback_ = disconnect_callback_;
    }
   private:
    std::function<int(const int)> disconnect_callback_;
    std::atomic<bool> is_connecting_;
    int fd_;
    /*
     * not copy and copy assign
     */
    Connector(const Connector &);
    void operator=(const Connector &);
  };

  void ThreadMain();

  std::thread::id thread_id_;
  int clean_interval_;
  int heart_beat_s_;
  int time_wheel_scale_;
  std::map<int, std::weak_ptr<Connector>> fd_connector_map_;
  std::vector<std::vector<std::shared_ptr<Connector>>> time_wheel_;
  std::atomic<bool> quit_;
  std::unique_ptr<Epoll> ep_ptr_;
  std::unique_ptr<std::thread> thread_ptr_;
  /*
   * not copy and copy assign
   */
  WorkThread(const WorkThread &);
  void operator=(const WorkThread &);
};

};

#endif //SNAPPY_SERVER_WORK_THREAD_H
