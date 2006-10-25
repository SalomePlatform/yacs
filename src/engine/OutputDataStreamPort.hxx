#ifndef __OUTPUTDATASTREAMPORT_HXX__
#define __OUTPUTDATASTREAMPORT_HXX__

#include "OutPort.hxx"
#include "DataStreamPort.hxx"
#include "ConversionException.hxx"

#include <set>

namespace YACS
{
  namespace ENGINE
  {
    class ElementaryNode;
    class InputDataStreamPort;

    class OutputDataStreamPort : public DataStreamPort, public OutPort
    {
      friend class ElementaryNode;
    protected:
      std::set<InputDataStreamPort *> _setOfInputDataStreamPort;
    public:
      static const char NAME[];
    public:
      OutputDataStreamPort(const std::string& name, Node *node, TypeCode* type);
      std::string getNameOfTypeOfCurrentInstance() const;
      bool edAddInputDataStreamPort(InputDataStreamPort *port) throw(ConversionException);
      void edRemoveInputDataStreamPort(InputDataStreamPort *inputPort) throw(Exception);
      bool addInPort(InPort *inPort) throw(Exception);
      void removeInPort(InPort *inPort) throw(Exception);
      bool isLinked();
    protected:
      void edRemoveInputDataStreamPortOneWay(InputDataStreamPort *inputPort);
    private:
      bool isAlreadyInSet(InputDataStreamPort *inputPort) const;
    };
  }
}

#endif
