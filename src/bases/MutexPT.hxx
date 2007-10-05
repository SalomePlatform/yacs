#ifndef __MUTEXPT_HXX__
#define __MUTEXPT_HXX__

#include <pthread.h>

namespace YACS
{
  namespace BASES
  {
    class ConditionPT;

    class MutexPT
    {
    public:
      MutexPT();
      ~MutexPT();
      void lock();
      void unlock();
      friend class ConditionPT;
    private:
      pthread_mutex_t _mutexDesc;
      pthread_mutexattr_t _options;

    };

    class ConditionPT
    {
    public:
      ConditionPT();
      ~ConditionPT();
      void notify_one();
      void notify_all();
      void wait(MutexPT& mutex);
    private:
      pthread_cond_t _cond;
    };
  }
}

#endif
