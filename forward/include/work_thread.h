//
// Created by lvcheng1 on 19-2-24.
//

#ifndef SNAPPY_SERVER_WORK_THREAD_H
#define SNAPPY_SERVER_WORK_THREAD_H

#include <thread>
#include <functional>
#include <atomic>

#include "forward_epoll.h"
#include "forward_define.h"

namespace forward {

class WorkThread {
 public:
  WorkThread();
  virtual ~WorkThread();
  int Acceptwork(const int &word_fd_, const int &events);
  void Start();
  void Quit();

 private:
  void ThreadMain();

  std::thread::id thread_id_;
  int time_out_ms_;

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
