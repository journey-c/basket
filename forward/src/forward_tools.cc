#include <fcntl.h>
#include <stdint.h>
#include <sys/param.h>
#include <sys/time.h>
#include <unistd.h>
#include <fstream>

#include "forward/include/forward_tools.h"

namespace forward {

int SetNonBlocking(int sock_fd) {
  int flags;
  if ((flags = fcntl(sock_fd, F_GETFL, 0)) < 0) {
    close(sock_fd);
    return -1;
  }
  flags |= O_NONBLOCK;
  if (fcntl(sock_fd, F_SETFL, flags) < 0) {
    close(sock_fd);
    return -1;
  }
  return flags;
}

uint64_t GetNowMillis() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
}
