#include <iostream>
#include <stdio.h>

#include "mlog/include/mlog.h"
#include "mlog/include/mlog_define.h"

int main() {
  if (mlog::InitLog(kTRACE, "./log", "mlog")) {
    std::cout << "初始化失败" << std::endl;
    exit(-1);
  }
  mlog::Write(kTRACE, {__func__, "ktrace"});
  mlog::Write(kDEBUG, {__func__, "kdebug"});
  mlog::Write(kINFO, {__func__, "kinfo"});
  mlog::Write(kWARN, {__func__, "kwarn"});
  mlog::Write(kERROR, {__func__, "kerror"});
  mlog::Write(kDOT, {__func__, "kdot"});
  mlog::Write(kFATAL, {__func__, "kfatal"});

  mlog::BackupAndSwitchLog("20190101");

  mlog::Write(kTRACE, {__func__, "newktrace"});
  mlog::Write(kDEBUG, {__func__, "newkdebug"});
  mlog::Write(kINFO, {__func__, "newkinfo"});
  mlog::Write(kWARN, {__func__, "newkwarn"});
  mlog::Write(kERROR, {__func__, "newkerror"});
  mlog::Write(kDOT, {__func__, "newkdot"});
  mlog::Write(kFATAL, {__func__, "newkfatal"});
  return 0;
}
