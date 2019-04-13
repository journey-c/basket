#include <fcntl.h>
#include <stdint.h>
#include <sys/param.h>
#include <sys/time.h>
#include <unistd.h>
#include <fstream>

#include "forward/include/forward_tools.h"

namespace forward {

#define MQTT_REMAINING_LEN_MASK 1099511627520u

int32_t GetCpuNum() {
  std::ifstream infile("/proc/cpuinfo");
  if (!infile) {
    return -1;
  }
  std::string line;
  int pos = 0;
  int cpu_num = 0;
  while (!infile.eof()) {
    std::getline(infile, line);
    if (line.empty() || "processor" != line.substr(0, 9)) {
      continue;
    }
    pos = static_cast<int>(line.find_first_of("0123456789"));
    cpu_num = std::stoi(line.substr(static_cast<unsigned long>(pos), line.size() - pos));
  }
  return cpu_num + 1;
}

int64_t GetRemaining_length(const char *buf) {
  int64_t ret = 0;
  std::string str;
  int pos = 0;
  int iscontinue = 1;
  int64_t rl = (*buf) & MQTT_REMAINING_LEN_MASK;
  while (rl) {
    if (iscontinue < 0) {
      break;
    }
    if (pos == 0) {
      iscontinue--;
      if (rl % 2) {
        iscontinue++;
      }
    } else {
      ret *= 2;
      ret += (rl % 2);
    }
    rl /= 2;
    pos++;
    pos %= 8;
  }

  return ret;
}

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
