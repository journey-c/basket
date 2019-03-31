//
// Created by lvcheng1 on 19-3-12.
//

#ifndef BASKET_FORWARD_FORWARD_CONN_H
#define BASKET_FORWARD_FORWARD_CONN_H

#include <string>

#include "forward/include/work_thread.h"

namespace forward {

class WorkThread;

class ForwardConn {
 public:
  explicit ForwardConn(int fd_,
                       const std::string &remote_ip_,
                       int16_t remote_port_,
                       WorkThread *thread_);
  virtual ~ForwardConn();

  virtual int GetRequest() = 0;

  virtual int SendReply() = 0;

  int getFd_() const {
    return fd_;
  }
  WorkThread *getThread_() const {
    return thread_;
  }
  void setHeart_beat_(const int heart_beat) {
    heart_beat_ = heart_beat;
  } 
  int32_t getHeart_beat_() const {
    return heart_beat_;
  }
  bool isIs_reply_() const {
    return is_reply_;
  }
  void setIs_reply_(bool is_reply_) {
    ForwardConn::is_reply_ = is_reply_;
  }

  bool SetNoBlock();

 private:
  int fd_;
  std::string remote_ip_;
  int16_t remote_port_;
  int32_t heart_beat_;
  bool is_reply_;
  /*
   * Which thread this connection belongs to
   */
  WorkThread *thread_;

  /*
   * No allowed copy and copy assign operator
   */
  ForwardConn(const ForwardConn &);
  void operator=(const ForwardConn &);
};

class ConnFactory {
 public:
  virtual ~ConnFactory() {}
  virtual ForwardConn *NewConn(const int fd_, const std::string &remote_ip_, int16_t remote_port_,
                               forward::WorkThread *thread_) const = 0;
};
}

#endif // BASKET_FORWARD_FORWARD_CONN_H
