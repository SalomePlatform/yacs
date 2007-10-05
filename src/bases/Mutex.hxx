#ifndef __MUTEX_HXX__
#define __MUTEX_HXX__

// --- Interface is:

// class Mutex
// {
// public:
//   Mutex();
//   void lock();
//   void unlock();
// };

// class Condition
// {
// public:
//   Condition();
//   void notify_one();
//   void notify_all();
//   void wait(Mutex& mutex);
// };


#if defined(YACS_PTHREAD)
#include "MutexPT.hxx"

namespace YACS
{
  namespace BASES
  {
    typedef MutexPT Mutex;
    typedef ConditionPT Condition;
  }
}
#else
#error
#endif

#endif
