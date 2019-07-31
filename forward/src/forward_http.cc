#include "forward/include/forward_http.h"

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

ForwardHttpConn::ForwardHttpConn() : ForwardConn(), http_read_status_(kNone){
  rbuf_ = reinterpret_cast<char *>(malloc());
  rbuf_pos_ = 0;
  rbuf_len_ = 0;

  wbuf_pos_ = 0;
  wbuf_len_ = 0;

  request_.clear();
  response_.clear();
}

ForwardHttpConn::~ForwardHttpConn() {
  
}

ReadStatus GetRequest() {
  getFd_();
}

WriteStatus SendReply() {

}

int ClearUp(const std::string msg) {

}
