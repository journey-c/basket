//
// Created by lvcheng1 on 19-2-20.
//

#include "dispatch_thread.h"

int main() {
  forward::DispatchThread *dispatchThread = new forward::DispatchThread("0.0.0.0", 8080, 5, false);
  dispatchThread->Start();
  return 0;
}