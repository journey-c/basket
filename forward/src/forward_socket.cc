#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "forward_socket.h"
#include "forward_define.h"

namespace forward {

Socket::Socket(int port_, bool is_block_)
    : port_(port_), is_block_(is_block_), is_listen_(false) {
}

Socket::~Socket() {
  close(sock_fd_);
}

int Socket::Listen(const std::string &bind_ip) {
  int status;
  struct sockaddr_in serverAddr;

  sock_fd_ = socket(AF_INET, SOCK_STREAM, DEF_PROTOCOL);
  if (sock_fd_ < kSUCCESS) {

    return sock_fd_;
  }

  int flags = fcntl(sock_fd_, F_GETFL, 0);
  if (flags == -1) {
    log_err("Get file status flags failed");
  }

  if (fcntl(sock_fd_, F_SETFL, flags | FD_CLOEXEC) == -1) {
    log_err("Set file status flags failed");
  }

  bzero(&serverAddr, sizeof(serverAddr));

  serverAddr.sin_family = AF_INET;
  if (bind_ip.empty()) {
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  } else {
    serverAddr.sin_addr.s_addr = inet_addr(bind_ip.c_str());
  }
  serverAddr.sin_port = htons(port_);

  status = bind(sock_fd_, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
  if (status < kSUCCESS) {
    log_err("bind error");
  }

  status = listen(sock_fd_, BACKLOG);
  if (status < 0) {
    log_err("listen error");
  }

  is_listen_ = true;

  if (!is_block_) {
    status = SetNonBlock(sock_fd_);
    if (status == -1) {
      log_err("set listen_fd non block failed");
    }
  }

  return status;
}

int Socket::SetNonBlock(int sockfd) {
  int flags;
  if ((flags = fcntl(sockfd, F_GETFL, 0)) < 0) {
    close(sockfd);
    return -1;
  }

  flags |= O_NONBLOCK;
  if (fcntl(sockfd, F_SETFL, flags) < 0) {
    close(sockfd);
    return -1;
  }
  return flags;
}

};
