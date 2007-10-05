#include "ThreadPT.hxx"
#include "Exception.hxx"
#include <unistd.h>

using namespace YACS::BASES;

ThreadPT::ThreadPT(ThreadJob funcPtr, void *stack)
{
  int err;
  void **stackT=(void **) stack;
  err=pthread_create(&_threadId,0,funcPtr,stackT);
  if(err!=0)throw Exception("Error in thread creation");
}

bool ThreadPT::operator==(const ThreadPT& other)
{
  return pthread_equal(_threadId, other._threadId) != 0;
}

//! Detach thread to release resources on exit
void ThreadPT::detach()
{
  pthread_detach(pthread_self());
}

void ThreadPT::exit(void *what)
{
  pthread_exit(what);
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
