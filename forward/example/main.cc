#include <iostream>

#include <unistd.h>

#include "forward/include/dispatch_thread.h"
#include "forward/include/forward_conn.h"

class MyConn : public forward::ForwardConn {
 public:
  explicit MyConn(int fd, const std::string &remote_ip, int16_t remote_port,
                  forward::WorkThread *thread)
      : ForwardConn(fd, remote_ip, remote_port, thread),
        msg("") {
  }
  ~MyConn() override {
    std::cout << "Myconn" << std::endl;
  }

  int GetRequest() override {
    ForwardConn::setHeart_beat_(5);
    char buf[1024];
    auto ret = read(getFd_(), buf, sizeof(buf) / sizeof(buf[0]));
    if (ret < 0) {
      std::cerr << "read error" << ret << std::endl;
      return static_cast<int>(ret);
    }
    msg = buf;
    setIs_reply_(true);
    return 0;
  }

  int SendReply() override {
    auto ret = write(getFd_(), msg.c_str(), msg.size());
    if (ret < 0) {
      std::cerr << "write error" << std::endl;
      return static_cast<int>(ret);
    }
    return 0;
  }

 private:
  std::string msg;
};

class MyConnFactory : public forward::ConnFactory {
 public:
  forward::ForwardConn *NewConn(const int fd_,
                                const std::string &remote_ip_,
                                int16_t remote_port_,
                                forward::WorkThread *thread_) const override {
    return new MyConn(fd_, remote_ip_, remote_port_, thread_);
  }
};


int main() {
  auto *mf = new MyConnFactory();
  forward::DispatchThread *dt = new forward::DispatchThread("0.0.0.0",
                                                            8080,
                                                            1,
                                                            mf);
  dt->Start();
  for (;;) {
  }
  return 0;
}
