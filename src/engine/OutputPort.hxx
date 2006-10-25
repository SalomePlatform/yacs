#ifndef __OUTPUTPORT_HXX__
#define __OUTPUTPORT_HXX__

//#include <Python.h>
//#include <omniORB4/CORBA.h>

#include "TypeCode.hxx"
#include "OutPort.hxx"
#include "DataFlowPort.hxx"
#include "ConversionException.hxx"

#include <set>

namespace YACS
{
  namespace ENGINE
  {
    class InputPort;
    class ElementaryNode;
    class Runtime;

    class OutputPort : public DataFlowPort, public OutPort
    {
      friend class ElementaryNode;  // for disconnect...
      friend class Runtime;         // for port creation
    public:
      ~OutputPort();

      std::string getNameOfTypeOfCurrentInstance() const;
      std::set<InputPort *> edSetInputPort();
      bool isLinked();
      bool isAlreadyInSet(InputPort *inputPort) const;

      virtual bool addInPort(InPort *inPort) throw(Exception);
      bool edAddInputPort(InputPort *inputPort) throw(ConversionException);
      virtual void removeInPort(InPort *inPort) throw(Exception);
      void edRemoveInputPort(InputPort *inputPort) throw(Exception);

      void exInit();

      virtual void put(const void *data) throw(ConversionException);

      static const char NAME[];

    protected:
      OutputPort(const std::string& name, Node *node, TypeCode* type);
      void edRemoveInputPortOneWay(InputPort *inputPort);
      std::set<InputPort *> _setOfInputPort;
    };
  }
}

#endif
