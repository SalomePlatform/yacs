#ifndef __INPORT_HXX__
#define __INPORT_HXX__

#include "Port.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class InPort : public virtual Port
    {
    protected:
      InPort(Node *node);
    };
  }
}

#endif
