#ifndef __OUTPORT_HXX__
#define __OUTPORT_HXX__

#include "Port.hxx"
#include "Exception.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class InPort;
    class OutPort : public virtual Port
    {
    protected:
      OutPort(Node *node);
    public:
      virtual bool addInPort(InPort *inPort) throw(Exception) = 0;
      virtual void removeInPort(InPort *inPort) throw(Exception) = 0;
    };
  }
}

#endif
