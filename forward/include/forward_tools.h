#ifndef FORWARD_INCLUDE_FORWARD_TOOLS_H_
#define FORWARD_INCLUDE_FORWARD_TOOLS_H_

#include <stdint.h>

namespace forward {

extern int SetNonBlocking(int sock_fd);
extern uint64_t GetNowMillis();
}

#endif  // FORWARD_INCLUDE_FORWARD_TOOLS_H_
