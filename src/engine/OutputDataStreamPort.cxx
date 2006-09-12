#include "OutputDataStreamPort.hxx"
#include "InputDataStreamPort.hxx"
#include "TypeCheckerDataStream.hxx"

using namespace YACS::ENGINE;

const char OutputDataStreamPort::NAME[]="OutputDataStreamPort";

OutputDataStreamPort::OutputDataStreamPort(const std::string& name, Node *node, StreamType type):DataStreamPort(name,node,type),OutPort(node),Port(node)
{
}

std::string OutputDataStreamPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

bool OutputDataStreamPort::edAddInputDataStreamPort(InputDataStreamPort *port) throw(ConversionException)
{
  if(!TypeCheckerDataStream::areStaticallyCompatible(edGetType(),port->edGetType()))
    throw ConversionException(TypeCheckerDataStream::edGetTypeInPrintableForm(edGetType()),TypeCheckerDataStream::edGetTypeInPrintableForm(port->edGetType()));
  if(!isAlreadyInList(port))
    {
      _listOfInputDataStreamPort.push_back(port);
      return true;
    }
  else
    return false;
}

void OutputDataStreamPort::edRemoveInputDataStreamPort(InputDataStreamPort *inputPort)
{
  if(isAlreadyInList(inputPort))
    _listOfInputDataStreamPort.remove(inputPort);
}

bool OutputDataStreamPort::addInPort(InPort *inPort) throw(Exception)
{
}

void OutputDataStreamPort::removeInPort(InPort *inPort) throw(Exception)
{
}

bool OutputDataStreamPort::isLinked()
{
  return _listOfInputDataStreamPort.empty();
}

bool OutputDataStreamPort::isAlreadyInList(InputDataStreamPort *inputPort) const
{
  bool ret=false;
  for(std::list<InputDataStreamPort *>::const_iterator iter=_listOfInputDataStreamPort.begin();iter!=_listOfInputDataStreamPort.end();iter++)
    if((*iter)==inputPort)
      ret=true;
  return ret;
}
