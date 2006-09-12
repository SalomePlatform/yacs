#ifndef __SEMAPHOREPT_HXX__
#define __SEMAPHOREPT_HXX__

#include <semaphore.h>

namespace YACS
{
  namespace BASES
  {
    class SemaphorePT
    {
    public:
      SemaphorePT(int initValue=0);
      ~SemaphorePT();
      void post();
      void wait();
      int getValue();
    private:
      sem_t _semDesc;
    };
  }
}

#endif
