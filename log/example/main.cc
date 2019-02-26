#include <unistd.h>

#include <iostream>
#include <string>
#include <thread>

#include "mlog.h" 

int main(int argv, char *argc[]) {
  int result = mlog::InitLog(kTRACE, "./log", "mlog");
  if (result) {
    std::cout << "Init Log failed" << std::endl;
  } else {
    std::cout << "Init Log Success" << std::endl;
  }

  // std::thread t1(mlog::BackupAndSwitchLog);
  // std::thread t2(mlog::BackupAndSwitchLog);

  // t1.join();
  // t2.join();

  std::string msg = "test";

  result = mlog::Write(kTRACE, msg);
  if (result) {
    std::cout << "write failed" << std::endl;
  } else {
    std::cout << "write susses" << std::endl;
  }

  result = mlog::Write(kTRACE, "test1");
  if (result) {
    std::cout << "write failed" << std::endl;
  } else {
    std::cout << "write susses" << std::endl;
  }

  return 0;
}
