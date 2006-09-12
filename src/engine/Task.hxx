#ifndef __TASK_HXX__
#define __TASK_HXX__

namespace YACS
{
  namespace ENGINE
  {
    class Task
    {
    public:
      virtual void begin() = 0;
      virtual bool isReady() = 0;
      virtual void execute() = 0;
      virtual void finished() = 0;
    };
  }
}

#endif
