#ifndef __SEMAPHORE_HXX__
#define __SEMAPHORE_HXX__

/* Interface is :
   class Semaphore
    {
    public:
      Semaphore(int initValue=0);
      ~Semaphore();
      void post();
      void wait();
      int getvalue();
    };
 */

#if defined(YACS_PTHREAD)
#include "SemaphorePT.hxx"

namespace YACS
{
  namespace BASES
  {
    typedef SemaphorePT Semaphore;
  }
}
#else
#error
#endif

#endif
