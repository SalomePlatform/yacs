#ifndef __DRIVENCONDITIONPT_HXX__
#define __DRIVENCONDITIONPT_HXX__

#include <pthread.h>

namespace YACS
{
  namespace BASES
  {
    class DrivenConditionPT
    {
    public:
      DrivenConditionPT();
      ~DrivenConditionPT();
      //On master thread
      void waitForAWait();
      void notifyOneSync();
      //On slave thread
      void wait();
    private:
      pthread_cond_t _cond1;
      pthread_cond_t _cond2;
      pthread_mutex_t _mutexDesc1;
      pthread_mutex_t _mutexDesc2;
    };
  }
}

#endif
