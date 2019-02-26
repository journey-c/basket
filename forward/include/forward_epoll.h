#ifndef BASKET_INCLUDE_EPOLL_H_
#define BASKET_INCLUDE_EPOLL_H_

#include <memory>
#include <vector>

namespace forward {

class Epoll {
 public:
  Epoll();
  ~Epoll();

  int AddEvent(const int &fd, const int &events);
  int DelEvent(const int &fd);
  int ModEvent(const int &fd, const int &old_events, const int &new_events);
  int Wait(std::vector<std::pair<int, uint32_t>> *read_list, const int &time_out_ms);

 private:
  int efd_;
  std::vector<struct epoll_event> events_;
};

};

#endif  // BASKET_INCLUDE_EPOLL_H_
