#ifndef __OUTPUTPORT_HXX__
#define __OUTPUTPORT_HXX__

#include "OutPort.hxx"
#include "DataFlowPort.hxx"
#include "ConversionException.hxx"

#include <list>

namespace YACS
{
  namespace ENGINE
  {
    class InputPort;

    class OutputPort : public DataFlowPort, public OutPort
    {
    protected:
      std::list<InputPort *> _listOfInputPort;
    public:
      static const char NAME[];
    public:
      OutputPort(const std::string& name, Node *node, DynType type);
      ~OutputPort();
      std::string getNameOfTypeOfCurrentInstance() const;
      Data foGet() const;
      void exInit();
      void exPut(Data data) throw(ConversionException);
      bool edAddInputPort(InputPort *inputPort) throw(ConversionException);
      std::list<InputPort *> edListInputPort();
      void edRemoveInputPort(InputPort *inputPort) throw(Exception);
      bool addInPort(InPort *inPort) throw(Exception);
      void removeInPort(InPort *inPort) throw(Exception);
      bool isLinked();
      bool isAlreadyInList(InputPort *inputPort) const;
    };
  }
}

#endif
