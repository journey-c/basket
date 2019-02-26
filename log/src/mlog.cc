#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <cstdarg>
#include <string.h>

#include <iostream>
#include <string>

#include "mlog.h"
#include "mlog_define.h"

namespace mlog {

static const int kLogItemLen = 1024 * 1024;

std::mutex log_level_str_mutex;
std::mutex log_level_file_mutex;

LogMeta log_meta;

LogMeta::LogMeta() {
  std::lock_guard<std::mutex> log_level_str_guard(log_level_str_mutex);

  log_level_str_.insert(std::make_pair(kTrace, "Trace"));
  log_level_str_.insert(std::make_pair(kDebug, "Debug"));
  log_level_str_.insert(std::make_pair(kInfo, "Info"));
  log_level_str_.insert(std::make_pair(kWarn, "Warn"));
  log_level_str_.insert(std::make_pair(kError, "Error"));
  log_level_str_.insert(std::make_pair(kDot, "Dot"));
  log_level_str_.insert(std::make_pair(kFatal, "Fatal"));
}

LogMeta::~LogMeta() {
  log_level_str_.clear();
}

int DoCreateDir(const char *path, mode_t mode) {
  struct stat st;
  int status = 0;
  // 目录不存在才创建
  if (stat(path, &st)) {
    if (mkdir(path, mode) != 0) {
      return -1;
    }
  }
  return 0;
}

int CreateDir(const std::string &path, mode_t mode) {
  std::string tmp_str;
  int status = 0;
  for (int i = 0; i < (int) path.size(); i++) {
    if (path[i] == '/' && !tmp_str.empty()) {
      status = DoCreateDir(tmp_str.c_str(), mode);
      if (status)
        return status;
    }
    tmp_str += path[i];
  }
  status = DoCreateDir(tmp_str.c_str(), mode);
  return status;
}

int CreateLogFile() {
  std::lock_guard<std::mutex> log_level_file_guard(log_level_file_mutex);

  log_meta.log_level_file_.clear();

  FILE *file;
  for (int32_t idx = kTrace; idx <= kMaxLevel; idx++) {
    std::string file_name = log_meta.log_level_str_[static_cast<LogLevel>(idx)];
    std::string file_path = log_meta.log_dir_ + "/" + log_meta.file_pre_ + "_" + file_name + ".log";
    file = fopen(file_path.c_str(), "a+");
    if (file == NULL)
      return -1;
    log_meta.log_level_file_.insert(std::make_pair(static_cast<LogLevel>(idx), std::make_shared<FILE *>(file)));
  }
}

int InitLog(const LogLevel level, const std::string &log_dir,
            const std::string &file_pre) {
  int status = 0;
  std::string log_path = log_dir;
  if (log_path[log_path.size() - 1] == '/')
    log_path.erase(log_path.size() - 1);

  if (access(log_path.c_str(), F_OK)) {
    status = CreateDir(log_path, DIR_MASK);
    if (status) {
      return status;
    }
  }

  log_meta.file_pre_ = file_pre;
  log_meta.log_dir_ = log_path;
  log_meta.work_levels_ = level;

  status = CreateLogFile();
  return status;
}

int BackupAndSwitchLog() {
  std::lock_guard<std::mutex> log_level_str_guard(log_level_str_mutex);
  std::string dir = log_meta.log_dir_;
  if (dir.empty())
    return 0;

  char buf[64];
  time_t now = time(NULL);
  strftime(buf, sizeof(buf), "%Y%m%d", localtime(&now));

  if (dir[dir.size() - 1] == '/')
    dir.erase(dir.size() - 1);

  dir += "/backup";

  if (access(dir.c_str(), F_OK)) {
    int status = CreateDir(dir, DIR_MASK);
    if (status != 0) {
      return status;
    }
  }
  std::string old_name;
  std::string new_name;
  for (int32_t idx = kTrace; idx <= kMaxLevel; idx++) {
    old_name = log_meta.log_dir_ + "/" + log_meta.file_pre_ + "_" + log_meta.log_level_str_[static_cast<LogLevel>(idx)]
        + ".log";
    new_name =
        log_meta.log_dir_ + "/backup/" + log_meta.file_pre_ + "_" + log_meta.log_level_str_[static_cast<LogLevel>(idx)]
            + ".log" + "." + buf;
    int status = rename(old_name.c_str(), new_name.c_str());
    if (status)
      return status;
  }
  CreateLogFile();
  return 0;
}

void SetLogLevel(LogLevel level) {
  log_meta.work_levels_ = level;
}

void SetDir(const std::string &log_dir) {
  log_meta.log_dir_ = log_dir;
  if (log_meta.log_dir_[log_meta.log_dir_.size() - 1] == '/')
    log_meta.log_dir_.erase(log_meta.log_dir_.size() - 1);
}

std::string GetLogLevelStr(LogLevel level) {
  return log_meta.log_level_str_[level];
}

int32_t Write(const LogLevel level, std::string &msg) {
  int32_t ret = 0;

  if (level < log_meta.work_levels_)
    return ret;

  std::lock_guard<std::mutex> log_level_file_guard(log_level_file_mutex);
  std::shared_ptr<FILE *> write_file = log_meta.log_level_file_[level];

  if (msg[msg.size() - 1] != '\n')
    msg.append("\n", 1);

  size_t status = fwrite(msg.c_str(), msg.size(), 1, *write_file);

  if (status < 1)
    return ret;

  return ret;
}

int32_t Write(const LogLevel level, const std::initializer_list<std::string> &msgs) {
  if (msgs.size() == 0) {
    return 0;
  }

  static char time_buf[64];

  time_t now = time(NULL);

  strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S    ", localtime(&now));
  std::string msg(time_buf, strlen(time_buf));

  int32_t ret = 0;

  if (level < log_meta.work_levels_)
    return ret;

  std::lock_guard<std::mutex> log_level_file_guard(log_level_file_mutex);
  std::shared_ptr<FILE *> write_file = log_meta.log_level_file_[level];

  for (auto elem : msgs) {
    msg += elem + '\t';
  }

  if (msg[msg.size() - 1] != '\n')
    msg.append("\n", 1);

  size_t status = fwrite(msg.c_str(), msg.size(), 1, *write_file);
  if (status < 1)
    return -1;

  return 0;
}

} // namespace mlog
