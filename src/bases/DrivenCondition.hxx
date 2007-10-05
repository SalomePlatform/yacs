#ifndef __DRIVENCONDITION_HXX__
#define __DRIVENCONDITION_HXX__

// --- Interface is:

// class DrivenCondition
//     {
//     public:
//       DrivenCondition();
//       ~DrivenCondition();
//       //On master thread
//       void waitForAWait();
//       void notifyOneSync();
//       //On slave thread
//       void wait();
// };


#if defined(YACS_PTHREAD)
#include "DrivenConditionPT.hxx"

namespace YACS
{
  namespace BASES
  {
    typedef DrivenConditionPT DrivenCondition;
  }
}
#else
#error
#endif


#endif

