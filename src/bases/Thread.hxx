#ifndef __THREAD_HXX__
#define __THREAD_HXX__

/* Interface is :
   class Thread
    {
    public:
      typedef void *(*ThreadJob)(void*);
    public:
      Thread(ThreadJob funcPtr, void *stack);
      bool operator==(const Thread& other);
      void join();
      static void sleep(unsigned long usec);
    };
 */

#if defined(YACS_PTHREAD)
#include "ThreadPT.hxx"

namespace YACS
{
  namespace BASES
  {
    typedef ThreadPT Thread;
  }
}
#else
#error
#endif

#endif
