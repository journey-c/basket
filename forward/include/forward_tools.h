//
// Created by lvcheng1 on 19-3-5.
//

#ifndef ALLEN_IVERSON_LOCKER_ROOM_TOOLS_H
#define ALLEN_IVERSON_LOCKER_ROOM_TOOLS_H

namespace forward {

extern int32_t GetCpuNum();
extern int64_t GetRemaining_length(const char *buf);
extern int SetNonBlocking(int sock_fd);

}

#endif //ALLEN_IVERSON_LOCKER_ROOM_TOOLS_H
