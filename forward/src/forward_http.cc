#include "forward/include/forward_http.h"
#include "forward/include/forward_define.h"

void HttpRequest::ClearUp() {
  method_ = "";
  path_ = "";
  get_params_.clear();
  version_ = "";
  header_.clear();
  body_ = "";
  post_params_.clear();
}

void HttpResponse::ClearUp() {
  version_ = "";
  status_code_ = "";
  status_msg_ = "";
  header_.clear();
  body_ = "";
}

std::string HttpResponse::SerializeResponseMessage() const {
}

ForwardHttpConn::ForwardHttpConn(const int fd_, const std::string &remote_ip_, int16_t remote_port_, forward::WorkThread *thread_)
    : ForwardConn(fd_, remote_ip_, remote_port_, thread_), conn_status_(kHearder), keep_live_(DEF_HEART_BEAT) {
  rbuf_ = reinterpret_cast<char *>(malloc(DEF_HTTP_BUFFER_LEN));
  rbuf_pos_ = 0;
  rbuf_len_ = 0;

  wbuf_ = reinterpret_cast<char *>(malloc(DEF_HTTP_BUFFER_LEN));
  wbuf_pos_ = 0;
  wbuf_len_ = 0;

  request_.ClearUp();
  response_.ClearUp();
}

ForwardHttpConn::~ForwardHttpConn() {
  delete rbuf_;
  delete wbuf_;
}

ReadStatus GetRequest() {
  ssize_t ret;
  switch(conn_status_) {
    case kHearder:
      ret = read(getFd_(), rbuf_ + rbuf_pos_, );
      break;
    case kBody:
      break;
    case kComplete:
      HandlingRequest(request_, response_); 
      setIs_reply_(true);
      request_.ClearUp();
      return kReadAll;
    default:
      return kReadErr;
  }
}

WriteStatus SendReply() {
  response_.ClearUp();
  return kWriteAll;
}

int ClearUp(const std::string msg) {
}
