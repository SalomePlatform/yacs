#ifndef __MUTEXPT_HXX__
#define __MUTEXPT_HXX__

#include <pthread.h>

namespace YACS
{
  namespace BASES
  {
    class MutexPT
    {
    public:
      MutexPT();
      ~MutexPT();
      void lock();
      void unlock();
    private:
      pthread_mutex_t _mutexDesc;
      pthread_mutexattr_t _options;

    };
  }
}

#endif
