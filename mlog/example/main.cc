#include "mlog/include/mlog.h"

int main() {
  mlog::InitLog(kTRACE, "./log", "mlog");  
  mlog::Write(kTRACE, {__func__, "ktrace"});
  mlog::Write(kDEBUG, {__func__, "kdebug"});
  mlog::Write(kINFO, {__func__, "kinfo"});
  mlog::Write(kWARN, {__func__, "kwarn"});
  mlog::Write(kERROR, {__func__, "kerror"});
  mlog::Write(kDOT, {__func__, "kdot"});
  mlog::Write(kFATAL, {__func__, "kfatal"});
  return 0;
}
