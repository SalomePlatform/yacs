#ifndef __TASK_HXX__
#define __TASK_HXX__

#include "define.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class ComponentInstance;

    class Task
    {
    public:
      virtual void begin() = 0;
      virtual bool isReady() = 0;
      virtual void execute() = 0;
      virtual void load() = 0;
      virtual void loaded() = 0;
      virtual void initService() = 0;
      virtual void connectService() = 0;
      virtual void disconnectService() = 0;
      virtual bool isDeployable() const = 0;
      virtual ComponentInstance *getComponent() = 0;
      virtual YACS::StatesForNode getState() const = 0;
      virtual void finished() = 0;
      virtual void aborted() = 0;
    };
  }
}

#endif
