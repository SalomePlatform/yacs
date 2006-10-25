#include "OutputPort.hxx"
#include "InputPort.hxx"
#include "Runtime.hxx"

#include <sstream>
#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

const char OutputPort::NAME[]="OutputPort";

OutputPort::OutputPort(const string& name, Node *node, TypeCode* type):DataFlowPort(name,node,type),OutPort(node),Port(node)
{
}

string OutputPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

void OutputPort::exInit()
{
//   _data.exInit();
}


void OutputPort::put(const void *data) throw(ConversionException)
{
//   _data = (void *)data;
  cerr << _name << endl;
  cerr << _impl << endl;
  stringstream msg;
  msg << "Not implemented (" << __FILE__ << ":" << __LINE__ << ")";
  throw Exception(msg.str());
 }


/**
 * check if output type is an input type and if a data converter exists before link
 */

bool OutputPort::edAddInputPort(InputPort *inputPort) throw(ConversionException)
{
  InputPort *pwrap = getRuntime()->adapt(inputPort->getImpl(),
					     inputPort,
					     this->getImpl(),
					     this->type());

  if(!isAlreadyInSet(pwrap))
    {
      _setOfInputPort.insert(pwrap);
      inputPort->edNotifyReferenced();
      return true;
    }
  else
    {
      if ( dynamic_cast<ProxyPort*> (pwrap) )
	{
	  cerr << "ProxyPort destruction, while creating the same link twice..." << endl;
	  delete pwrap;
	}
      return false;
    }
}

set<InputPort *> OutputPort::edSetInputPort()
{
  return _setOfInputPort;
}

void OutputPort::edRemoveInputPort(InputPort *inputPort) throw(Exception)
{
  if(isAlreadyInSet(inputPort))
    _setOfInputPort.erase(inputPort);
  else
    throw Exception("OutputPort::edRemoveInputPort : link does not exist, unable to remove it");
}

//Idem OutputPort::edRemoveInputPort but without any check.
void OutputPort::edRemoveInputPortOneWay(InputPort *inputPort)
{
  _setOfInputPort.erase(inputPort);
}

OutputPort::~OutputPort()
{
}

bool OutputPort::isAlreadyInSet(InputPort *inputPort) const
{
  bool ret=false;
  for(set<InputPort *>::const_iterator iter=_setOfInputPort.begin();iter!=_setOfInputPort.end() && !ret;iter++)
    if((*iter)==inputPort)
      ret=true;
  return ret;
}

/**
 * check compatibility of port class ( an inputPort) before trying to create the link
 */

bool OutputPort::addInPort(InPort *inPort) throw(Exception)
{
  if(inPort->getNameOfTypeOfCurrentInstance()!=InputPort::NAME)
    {
      string what="not compatible type of port requested during building of link FROM ";
      what+=NAME; what+=" TO "; what+=inPort->getNameOfTypeOfCurrentInstance();
      throw Exception(what);
    }
  return edAddInputPort(static_cast<InputPort*>(inPort));
}

void OutputPort::removeInPort(InPort *inPort) throw(Exception)
{
  if(inPort->getNameOfTypeOfCurrentInstance()!=InputPort::NAME)
    {
      string what="not compatible type of port requested during destruction of for link FROM ";
      what+=NAME; what+=" TO "; what+=inPort->getNameOfTypeOfCurrentInstance();
      throw Exception(what);
    }
  edRemoveInputPort(static_cast<InputPort*>(inPort));
}

bool OutputPort::isLinked()
{
  return _setOfInputPort.empty();
}
