#ifndef _EXECUTOR_SWIG_HXX_
#define _EXECUTOR_SWIG_HXX_

#include "Executor.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class ExecutorSwig: public Executor
    {
    public:
      void RunPy(Scheduler *graph,
                 int debug=0,
                 bool isPyThread = true,
                 bool fromscratch=true);
      void waitPause();
    };
  }
}

#endif
