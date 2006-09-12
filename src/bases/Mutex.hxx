#ifndef __MUTEX_HXX__
#define __MUTEX_HXX__

/* Interface is :
   class Mutex
    {
    public:
      Mutex();
      void lock();
      void unlock();
    };
 */

#if defined(YACS_PTHREAD)
#include "MutexPT.hxx"

namespace YACS
{
  namespace BASES
  {
    typedef MutexPT Mutex;
  }
}
#else
#error
#endif

#endif
