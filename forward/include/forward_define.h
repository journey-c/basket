#ifndef FORWARD_INCLUDE_FORWARD_DEFINE_H_
#define FORWARD_INCLUDE_FORWARD_DEFINE_H_

#include <string.h>

namespace forward {

// socket
#define DEF_PROTOCOL 0
#define BACKLOG 2048

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



// http
#define GET "GET"    // 请求指定的页面信息，并返回实体主体。
#define HEAD "HEAD"  //类似于 GET 请求，只不过返回的响应中没有具体的内容，用于获取报头
#define POST "POST"  //向指定资源提交数据进行处理请求（例如提交表单或者上传文件）。数据被包含在请求体中。POST
                     //请求可能会导致新的资源的建立和/或已有资源的修改。
#define PUT "PUT"          // 从客户端向服务器传送的数据取代指定的文档的内容。
#define DELETE "DELETE"    // 请求服务器删除指定的页面。
#define CONNECT "CONNECT"  // HTTP/1.1 协议中预留给能够将连接改为管道方式的代理服务器。
#define OPTIONS "OPTIONS"  // 允许客户端查看服务器的性能。
#define TRACE "TRACE"      // 回显服务器收到的请求，主要用于测试或诊断。
#define PATCH "PATCH"      // 是对 PUT 方法的补充，用来对已知资源进行局部更新。
};

enum HttpReadStatus {
  kNone = -1,
  kHearder = 0,
  kBody = 1,
  kComplete = 2,
};

#define DEFINE_HTTP_READ_BUFFER_LEN 1024*1024

#endif  // FORWARD_INCLUDE_FORWARD_DEFINE_H_
