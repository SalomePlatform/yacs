#include "OutputDataStreamPort.hxx"
#include "InputDataStreamPort.hxx"
//#include "TypeCheckerDataStream.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char OutputDataStreamPort::NAME[]="OutputDataStreamPort";

OutputDataStreamPort::OutputDataStreamPort(const string& name, Node *node, TypeCode* type):DataStreamPort(name,node,type),OutPort(node),Port(node)
{
}

string OutputDataStreamPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

bool OutputDataStreamPort::edAddInputDataStreamPort(InputDataStreamPort *port) throw(ConversionException)
{
//   if(!TypeCheckerDataStream::areStaticallyCompatible(edGetType(),port->edGetType()))
//     throw ConversionException(TypeCheckerDataStream::edGetTypeInPrintableForm(edGetType()),TypeCheckerDataStream::edGetTypeInPrintableForm(port->    throw ConversionException(TypeCheckerDataStream::edGetTypeInPrintableForm(edGetType()),TypeCheckerDataStream::edGetTypeInPrintableForm(port->edGetType()));
  if(!isAlreadyInSet(port))
    {
      _setOfInputDataStreamPort.insert(port);
      return true;
    }
  else
    return false;
}

void OutputDataStreamPort::edRemoveInputDataStreamPort(InputDataStreamPort *inputPort) throw(Exception)
{
  if(isAlreadyInSet(inputPort))
    _setOfInputDataStreamPort.erase(inputPort);
//   else
//     throw Exception("OutputDataStreamPort::edRemoveInputDataStreamPort : link does not exist, unable to remove it");
}

//Idem OutputDataStreamPort::edRemoveInputDataStreamPort but no exception thrown if inputPort is not known
void OutputDataStreamPort::edRemoveInputDataStreamPortOneWay(InputDataStreamPort *inputPort)
{
  _setOfInputDataStreamPort.erase(inputPort);
}

bool OutputDataStreamPort::addInPort(InPort *inPort) throw(Exception)
{
}

void OutputDataStreamPort::removeInPort(InPort *inPort) throw(Exception)
{
}

bool OutputDataStreamPort::isLinked()
{
  return _setOfInputDataStreamPort.empty();
}

bool OutputDataStreamPort::isAlreadyInSet(InputDataStreamPort *inputPort) const
{
  bool ret=false;
  for(set<InputDataStreamPort *>::const_iterator iter=_setOfInputDataStreamPort.begin();iter!=_setOfInputDataStreamPort.end();iter++)
    if((*iter)==inputPort)
      ret=true;
  return ret;
}
