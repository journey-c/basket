#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <iostream>

#include "forward_epoll.h"
#include "forward_define.h"

namespace forward {

Epoll::Epoll(const int &initialize_event_container_size)
    : events_(static_cast<size_t >(initialize_event_container_size)) {
  efd_ = epoll_create1(EPOLL_CLOEXEC);
  fcntl(efd_, F_SETFD, fcntl(efd_, F_GETFD) | FD_CLOEXEC);

  if (efd_ < 0) {
    log_err("epoll create fail");
  }
}

Epoll::~Epoll() {
  ::close(efd_);
}

int Epoll::AddEvent(const int &fd, const int &events) {
  struct epoll_event ev;
  memset(&ev, 0, sizeof(ev));
  ev.data.fd = fd;
  ev.events = static_cast<uint32_t >(events);
  return ::epoll_ctl(efd_, EPOLL_CTL_ADD, fd, &ev);
}

int Epoll::DelEvent(const int &fd) {
  struct epoll_event ev;
  memset(&ev, 0, sizeof(ev));
  ev.data.fd = fd;
  return ::epoll_ctl(efd_, EPOLL_CTL_DEL, fd, &ev);
}

int Epoll::ModEvent(const int &fd, const int &old_events,
                    const int &new_events) {
  struct epoll_event ev;
  memset(&ev, 0, sizeof(ev));
  ev.data.fd = fd;
  ev.events = static_cast<uint32_t >(old_events | new_events);
  return ::epoll_ctl(efd_, EPOLL_CTL_MOD, fd, &ev);
}

int Epoll::Wait(std::vector<std::pair<int, uint32_t>> *read_list, const int &time_out_ms) {
  int wait_num =
      ::epoll_wait(Epoll::efd_, events_.begin().base(), static_cast<int>(events_.size()), time_out_ms);
  if (wait_num == -1) {
    if (errno == EINTR) {
      return 0;
    }
    return -1;
  }
  for (int i = 0; i < wait_num; ++i) {
    int fd = events_[i].data.fd;
    uint32_t event = events_[i].events;
    read_list->emplace_back(std::make_pair(fd, event));
  }
  if (wait_num == events_.size()) {
    events_.resize(events_.size() * 2);
  }
  return 0;
}

};