#include "ThreadPT.hxx"
#include <unistd.h>

using namespace YACS::BASES;

ThreadPT::ThreadPT(ThreadJob funcPtr, void *stack)
{
  void **stackT=(void **) stack;
  pthread_create(&_threadId,0,funcPtr,stackT);
}

bool ThreadPT::operator==(const ThreadPT& other)
{
  return pthread_equal(_threadId, other._threadId) != 0;
}

void ThreadPT::join()
{
  void *ret;
  pthread_join(_threadId, &ret);
}

void ThreadPT::sleep(unsigned long usec)
{
  usleep(usec);
}
