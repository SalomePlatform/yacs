#include "CalStreamPort.hxx"
#include "SalomeComponent.hxx"
#include "CORBANode.hxx"
#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char InputCalStreamPort::NAME[]="InputCalStreamPort";

InputCalStreamPort::InputCalStreamPort(const std::string& name, Node *node, TypeCode* type):
  InputDataStreamPort(name, node, type),
  DataPort(name, node, type),
  Port(node),_depend("T"),_schema("TI"),_delta(0.),_level(-1)
{
}
InputCalStreamPort::InputCalStreamPort(const InputCalStreamPort& other, Node *newHelder):
  InputDataStreamPort(other,newHelder),
  DataPort(other,newHelder),
  Port(other,newHelder),_depend(other._depend),_schema(other._schema),_delta(other._delta),_level(other._level)
{
}

InputCalStreamPort::~InputCalStreamPort()
{
}

void InputCalStreamPort::setDepend(const std::string& depend)
{
  DEBTRACE("InputCalStreamPort::setDepend: " << edGetNumberOfLinks());
  if(edGetNumberOfLinks() > 0)
    {
      throw Exception("Can not modify depend parameter on a connected port");
    }
  if(depend != "T" && depend != "I")
    {
      throw Exception("depend parameter must be T or I");
    }
  _depend=depend;
}

void InputCalStreamPort::setSchema(const std::string& schema)
{
  if(schema != "TI" && schema != "TF" && schema != "DELTA")
    {
      throw Exception("schema parameter must be TI, TF or DELTA");
    }
  _schema=schema;
}
void InputCalStreamPort::setLevel(const std::string& level)
{
  _level =atoi(level.c_str());
}

void InputCalStreamPort::setProperty(const std::string& name, const std::string& value)
{
  if(name == "depend")
    setDepend(value);
  else if(name == "schema")
    setSchema(value);
  else if(name == "level")
    setLevel(value);
  InputDataStreamPort::setProperty(name,value);
}

#ifdef DSC_PORTS
void InputCalStreamPort::initPortProperties()
{

  DEBTRACE(_level);
  if(_level>0)
    {
      CORBA::Object_var objComponent=((SalomeComponent*)((SalomeNode*)_node)->getComponent())->getCompoPtr();
      Engines::Superv_Component_var compo=Engines::Superv_Component::_narrow(objComponent);
      Ports::PortProperties_var port_properties=compo->get_port_properties(getName().c_str());
      CORBA::Any a;
      a <<= (CORBA::Long)_level;
      try
        {
          port_properties->set_property("StorageLevel",a);
        }
      catch(Ports::NotDefined& ex)
        {
          throw Exception("Property StorageLevel not defined on that port: "+getName());
        }
    }
}
#endif

std::string InputCalStreamPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

InputCalStreamPort *InputCalStreamPort::clone(Node *newHelder) const
{
  return new InputCalStreamPort(*this,newHelder);
}

const char OutputCalStreamPort::NAME[]="OutputCalStreamPort";

OutputCalStreamPort::OutputCalStreamPort(const std::string& name, Node *node, TypeCode* type):
  OutputDataStreamPort(name, node, type),
  DataPort(name, node, type),
  Port(node),_depend("T")
{
}

OutputCalStreamPort::OutputCalStreamPort(const OutputCalStreamPort& other, Node *newHelder):
  OutputDataStreamPort(other,newHelder),
  DataPort(other,newHelder),
  Port(other,newHelder),
  _depend(other._depend)
{
}

OutputCalStreamPort::~OutputCalStreamPort()
{
}

void OutputCalStreamPort::setDepend(const std::string& depend)
{
  DEBTRACE("OutputCalStreamPort::setDepend: " << edGetNumberOfOutLinks());
  if(edGetNumberOfOutLinks() > 0)
    {
      throw Exception("Can not modify depend parameter on a connected port");
    }
  if(depend != "T" && depend != "I" )
    {
      throw Exception("depend parameter must be T or I");
    }
  _depend=depend;
}

void OutputCalStreamPort::setSchema(const std::string& schema)
{
  if(schema != "TI" && schema != "TF" && schema != "DELTA")
    {
      throw Exception("schema parameter must be TI, TF or DELTA");
    }
  _schema=schema;
}
void OutputCalStreamPort::setLevel(const std::string& level)
{
  _level =atoi(level.c_str());
}

void OutputCalStreamPort::setProperty(const std::string& name, const std::string& value)
{
  if(name == "depend")
    setDepend(value);
  else if(name == "schema")
    setSchema(value);
  else if(name == "level")
    setLevel(value);
  OutputDataStreamPort::setProperty(name,value);
}

std::string OutputCalStreamPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

OutputCalStreamPort *OutputCalStreamPort::clone(Node *newHelder) const
{
  return new OutputCalStreamPort(*this,newHelder);
}

bool OutputCalStreamPort::addInPort(InPort *inPort) throw(Exception)
{
  DEBTRACE("OutputCalStreamPort::addInPort " << InputCalStreamPort::NAME );
  if(inPort->getNameOfTypeOfCurrentInstance()!=InputCalStreamPort::NAME)
    {
      string what="not compatible type of port requested during building of link FROM ";
      what+=NAME; what+=" TO "; what+=inPort->getNameOfTypeOfCurrentInstance();
      throw Exception(what);
    }

  InputCalStreamPort* port=static_cast<InputCalStreamPort*>(inPort);
  if(port->getDepend() != _depend)
    {
      std::string what= "incompatible depend parameters: "+_depend+" != "+ port->getDepend();
      throw Exception(what);
    }
  bool ret;
  ret= edAddInputDataStreamPort(port);
  return ret;
}


int OutputCalStreamPort::removeInPort(InPort *inPort, bool forward) throw(Exception)
{
  DEBTRACE("OutputCalStreamPort::removeInPort");
  if(inPort->getNameOfTypeOfCurrentInstance()!=InputCalStreamPort::NAME && !forward)
    {
      string what="not compatible type of port requested during destruction of for link FROM ";
      what+=NAME; what+=" TO "; what+=inPort->getNameOfTypeOfCurrentInstance();
      throw Exception(what);
    }
  return edRemoveInputDataStreamPort(static_cast<InputDataStreamPort *>(inPort),forward);
}

