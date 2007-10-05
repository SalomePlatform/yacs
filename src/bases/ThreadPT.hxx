#ifndef __THREADPT_HXX__
#define __THREADPT_HXX__

#include <pthread.h>

namespace YACS
{
  namespace BASES
  {
    class ThreadPT
    {
    public:
      typedef void *(*ThreadJob)(void*);
    public:
      ThreadPT(ThreadJob funcPtr, void *stack);
      bool operator==(const ThreadPT& other);
      void join();
      static void detach();
      static void exit(void *what);
      static void sleep(unsigned long usec);
    private:
      pthread_t _threadId;
    };
  }
}

#endif
