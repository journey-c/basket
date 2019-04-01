#ifndef FORWARD_INCLUDE_FORWARD_SOCKET_H_
#define FORWARD_INCLUDE_FORWARD_SOCKET_H_

#include <string>

namespace forward {

class Socket {
 public:
  explicit Socket(int port_, bool is_block_);
  ~Socket();

  int getSock_fd_() const {
    return sock_fd_;
  }

  void setSock_fd_(int sock_fd_) {
    Socket::sock_fd_ = sock_fd_;
  }

  int getPort_() const {
    return port_;
  }

  void setPort_(int port_) {
    Socket::port_ = port_;
  }

  int Listen(const std::string &bind_ip);

 private:
  int SetNonBlock(int sockfd);
  int sock_fd_;
  int port_;
  bool is_block_;
  bool is_listen_;
};

};

#endif // FORWARD_INCLUDE_FORWARD_SOCKET_H_

