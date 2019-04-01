#ifndef FORWARD_INCLUDE_FORWARD_TOOLS_H_
#define FORWARD_INCLUDE_FORWARD_TOOLS_H_

#include <stdint.h>

namespace forward {

extern int32_t GetCpuNum();
extern int64_t GetRemaining_length(const char *buf);
extern int SetNonBlocking(int sock_fd);

}

#endif // FORWARD_INCLUDE_FORWARD_TOOLS_H_
