#ifndef FORWARD_INCLUDE_FROWARD_HTTP_H_
#define FORWARD_INCLUDE_FORWARD_HTTP_H_

#include <map>
#include <string>
#include "forward/include/forward_conn.h"

namespace forward {

static std::map<std::string, std::string> http_status_code{{"100", "Continue"},
                                                           {"101", "Switching Protocols "},

                                                           {"200", "OK"},
                                                           {"201", "Created"},
                                                           {"202", "Accepted"},
                                                           {"203", "Non-Authoritative Information"},
                                                           {"204", "No Content"},
                                                           {"205", "Reset Content "},
                                                           {"206", "Partial Content"},

                                                           {"300", "Multiple Choices"},
                                                           {"301", "Moved Permanently"},
                                                           {"302", "Found"},
                                                           {"303", "See Other"},
                                                           {"304", "Not Modified"},
                                                           {"305", "Use Proxy"},
                                                           {"306", "Unused"},
                                                           {"307", "Temporary Redirect"},

                                                           {"400", "Bad Request"},
                                                           {"401", "Unauthorized"},
                                                           {"402", "Payment Required"},
                                                           {"403", "Forbidden"},
                                                           {"404", "Not Found"},
                                                           {"405", "Method Not Allowed"},
                                                           {"406", "Not Acceptable"},
                                                           {"407", "Proxy Authentication Required"},
                                                           {"408", "Request Time-out"},
                                                           {"409", "Conflict"},
                                                           {"410", "Gone"},
                                                           {"411", "Length Required"},
                                                           {"412", "Precondition Failed"},
                                                           {"413", "Request Entity Too Large"},
                                                           {"414", "Request-URI Too Large"},
                                                           {"415", "Unsupported Media Type "},
                                                           {"416", "Requested range not satisfiable "},
                                                           {"417", "Expectation Failed "},

                                                           {"500", "Internal Server Error "},
                                                           {"501", "Not Implemented"},
                                                           {"502", "Bad Gateway"},
                                                           {"503", "Service Unavailable"},
                                                           {"504", "Gateway Time-out"},
                                                           {"505", "HTTP Version not supported"}

};

struct HttpRequest {
  // Head Line
  std::string method_;
  std::string path_;
  std::map<std::string, std::string> get_params_;
  std::string version_;

  // Head
  std::map<std::string, std::string> header_;

  // Body
  std::string body_;
  std::map<std::string, std::string> post_params_;

  void ClearUp();
};

struct HttpResponse {
  // Head Line
  std::string version_;
  std::string status_code_;
  std::string status_msg_;

  // Head
  std::map<std::string, std::string> header_;

  // Body
  std::string body_;

  void ClearUp();
  std::string SerializeResponseMessage() const;
};


class ForwardHttpConn : public ForwardConn {
 public:
  ForwardHttpConn(const int fd_, const std::string &remote_ip_, int16_t remote_port_, forward::WorkThread *thread_);
  ~ForwardHttpConn();

  virtual ReadStatus GetRequest();

  virtual WriteStatus SendReply();

  virtual int ClearUp(const std::string msg);

 private:
  HttpReadStatus http_read_status_;  

  char *rbuf_;
  uint64_t rbuf_pos_;
  uint64_t rbuf_len_;

  char *wbuf_;
  uint64_t wbuf_pos_;
  uint64_t wbuf_len_;

  HttpRequest request_;
  HttpResponse response_;
};

class ForwardHttpConnFactory : public ConnFactory {
 public:
  virtual ~ForwardHttpConnFactory() {
  }
  virtual ForwardConn *NewConn(const int fd, const std::string &remote_ip, int16_t remote_port, forward::WorkThread *thread) const {
    return new ForwardHttpConn(fd, remote_ip, remote_port, thread);
  }
};
}

#endif  // FORWARD_INCLUDE_FORWARD_HTTP_H_
