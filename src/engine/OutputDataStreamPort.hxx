#ifndef __OUTPUTDATASTREAMPORT_HXX__
#define __OUTPUTDATASTREAMPORT_HXX__

#include "OutPort.hxx"
#include "DataStreamPort.hxx"
#include "ConversionException.hxx"

#include <list>

namespace YACS
{
  namespace ENGINE
  {
    class InputDataStreamPort;

    class OutputDataStreamPort : public DataStreamPort, public OutPort
    {
    protected:
      std::list<InputDataStreamPort *> _listOfInputDataStreamPort;
    public:
      static const char NAME[];
    public:
      OutputDataStreamPort(const std::string& name, Node *node, StreamType type);
      std::string getNameOfTypeOfCurrentInstance() const;
      bool edAddInputDataStreamPort(InputDataStreamPort *port) throw(ConversionException);
      void edRemoveInputDataStreamPort(InputDataStreamPort *inputPort);
      bool addInPort(InPort *inPort) throw(Exception);
      void removeInPort(InPort *inPort) throw(Exception);
      bool isLinked();
    private:
      bool isAlreadyInList(InputDataStreamPort *inputPort) const;
    };
  }
}

#endif
