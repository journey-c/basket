#ifndef FORWARD_INCLUDE_FORWARD_DEFINE_H_
#define FORWARD_INCLUDE_FORWARD_DEFINE_H_

#include <string.h>

namespace forward {

// socket
#define DEF_PROTOCOL 0
#define BACKLOG 5

#define DEF_TIME_WHEEL_SIZE 65536
#define DEF_HEART_BEAT 60
#define DEF_CLEAN_INTERVAL 1000
#define DEF_MAX_MSG_LEN 2048

// errno
#define kSUCCESS 0

#define FORWARD_MAX_CLIENTS_NUM 1024
#define EPOLL_DEFINE_TIME_OUT 1000

#define clean_errno() (errno == 0 ? "none" : strerror(errno))
#define log_err(M, ...)                                                                                      \
  {                                                                                                          \
    fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__); \
    exit(-1);                                                                                                \
  }
#define log_warn(M, ...) fprintf(stderr, "[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

enum ReadStatus {
  kReadAll = 0,
  kReadHalf = 1,
  kReadErr = 2,
  kReadClose = 3,
  kFullErr = 4,
  kParseErr = 5,
};

enum WriteStatus {
  kWriteAll = 0,
  kWriteHalf = 1,
  kWriteErr = 2,
};
};

#endif  // FORWARD_INCLUDE_FORWARD_DEFINE_H_
