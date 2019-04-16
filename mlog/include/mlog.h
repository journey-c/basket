#ifndef MLOG_INCLUDE_MLOD_H_
#define MLOG_INCLUDE_MLOD_H_

#include <sys/types.h>

#include <string>
#include <map>
#include <memory>
#include <mutex>

namespace mlog {

extern std::mutex log_level_str_mutex;
extern std::mutex log_level_file_mutex;

enum LogLevel {
  kTrace = 0,
  kDebug,
  kInfo,
  kWarn,
  kError,
  kDot,
  kFatal,
  kMaxLevel
};

struct LogMeta {
  LogMeta();
  ~LogMeta();

  std::string file_pre_;
  std::string log_dir_;
  LogLevel work_levels_;

  std::map<LogLevel, std::shared_ptr<FILE *> > log_level_file_;
  std::map<LogLevel, std::string> log_level_str_;
};
extern LogMeta log_meta;

int InitLog(const LogLevel level = kInfo, const std::string &log_dir = "./log", const std::string &file_pre = "");

void BackupAndSwitchLog(const std::string file_suf);
void SetLogLevel(LogLevel level);
void SetDir(const std::string &log_dir);
std::string GetLogLevelStr(LogLevel level);
int32_t Write(const LogLevel level, std::string &msg);
int32_t Write(const LogLevel level, const std::initializer_list<std::string> &msgs);

#define    kTRACE     mlog::kTrace
#define    kDEBUG     mlog::kDebug
#define    kINFO      mlog::kInfo
#define    kWARN      mlog::kWarn
#define    kERROR     mlog::kError
#define    kDOT       mlog::kDot
#define    kFATAL     mlog::kFatal

} // namespace mlog

#endif // MLOG_INCLUDE_MLOD_H_
