#ifndef MLOG_INCLUDE_MLOG_DEFINE_H_
#define MLOG_INCLUDE_MLOG_DEFINE_H_

#include <string.h>

namespace mlog {

#define F_OK 0
#define DIR_MASK S_IRWXU | S_IRWXG  | S_IRWXO | S_IROTH | S_IXOTH

// DEBUG
/**
 * @author chenzongzhi
 * @version 1.0.0
 * @date 2014-04-25
 */
#define clean_errno() (errno == 0 ? "none" : strerror(errno))
#define log_err(M, ...)                                                                                      \
  {                                                                                                          \
    fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__); \
    exit(-1);                                                                                                \
  }
#define log_warn(M, ...) fprintf(stderr, "[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

} // namespace mlog

#endif // MLOG_INCLUDE_MLOG_DEFINE_H_
