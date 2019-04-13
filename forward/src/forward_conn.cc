#include <unistd.h>

#include "forward/include/forward_conn.h"
#include "forward/include/forward_tools.h"

namespace forward {

ForwardConn::ForwardConn(int fd_, const std::string &remote_ip_, int16_t remote_port_, WorkThread *thread_)
    : fd_(fd_),
      remote_ip_(remote_ip_),
      remote_port_(remote_port_),
      heart_beat_(DEF_HEART_BEAT),
      last_time_wheel_scale_(-1),
      last_active_time_(0),
      is_reply_(false),
      thread_(thread_) {
}

ForwardConn::~ForwardConn() {
  int ret = thread_->DelConn(fd_);
  if (ret != 0) {
    log_warn("disconnect error");
  }
  close(fd_);
  log_info("disconect");
}

bool ForwardConn::SetNoBlock() {
  return SetNonBlocking(fd_) == -1;
}
}

