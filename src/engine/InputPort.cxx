#include "InputPort.hxx"
#include "OutPort.hxx"
#include "ComposedNode.hxx"

#include <sstream>
#include <iostream>
#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char InputPort::NAME[]="InputPort";

InputPort::InputPort(const InputPort& other, Node *newHelder):DataFlowPort(other,newHelder),InPort(other,newHelder),
                                                              DataPort(other,newHelder),Port(other,newHelder),
                                                              _initValue(0)
{
  if(other._initValue)
    _initValue=other._initValue->clone();
}

InputPort::InputPort(const std::string& name, Node *node, TypeCode* type)
  : DataFlowPort(name,node,type), InPort(name,node,type),DataPort(name,node,type),Port(node), _initValue(0)
{
}

string InputPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

void InputPort::exInit(bool start)
{
  checkBasicConsistency();
  if(start)
    exRestoreInit();
}

bool InputPort::isEmpty()
{
  return get()==0;
}

//! Specifies if this port has been \b manually set by the call of InputPort::edInit
bool InputPort::edIsManuallyInitialized() const
{
  return _initValue!=0;
}

/*!
 * Perform a quick and not complete check. Use ComposedNode::CheckConsistency instead.
 */
bool InputPort::edIsInitialized() const
{
  return (edIsManuallyInitialized() or _backLinks.size()!=0 );
}

InputPort::~InputPort()
{
  if(_initValue)
    _initValue->decrRef();
}

void InputPort::edInit(Any *value)
{
  InputPort *manuallySet=getRuntime()->adapt(this,
                                             Runtime::RUNTIME_ENGINE_INTERACTION_IMPL_NAME,_type);
  manuallySet->put((const void *) value);
  if(manuallySet!=this)
    delete manuallySet;
  exSaveInit();
}

void InputPort::edInit(const std::string& impl,const void* value)
{
  InputPort *manuallySet=getRuntime()->adapt(this,impl,_type);
  manuallySet->put(value);
  if(manuallySet!=this)
    delete manuallySet;
  exSaveInit();
}

//! Removes eventually previous manual initialisation.
void InputPort::edRemoveManInit()
{
  if(_initValue)
    _initValue->decrRef();
  _initValue=0;
}

//! Check basically that this port has one chance to be specified on time. It's a necessary condition \b not \b sufficient at all.
void InputPort::checkBasicConsistency() const throw(Exception)
{
  if(!edIsManuallyInitialized() and _backLinks.size()==0 )
    {
      ostringstream stream;
      stream << "InputPort::checkBasicConsistency : Port " << _name << " of node with name " << _node->getName() << " neither initialized nor linked back";
      throw Exception(stream.str());
    }
}

std::string InputPort::dump()
{
  string xmldump = "<value><error> NO_SERIALISATION_AVAILABLE </error></value>";
  return xmldump;
}

void InputPort::setStringRef(std::string strRef)
{
  _stringRef = strRef;
}

ProxyPort::ProxyPort(InputPort* p):InputPort("Convertor", p->getNode(), p->edGetType()),DataPort("Convertor", p->getNode(), p->edGetType()),
                                   Port( p->getNode())
{
  _port = p;
}

ProxyPort::~ProxyPort()
{
  //For the moment, there is no case in YACS we have a proxy port in a proxy port
  //So don't test that. _port may be already deleted. The test is not sure.
  /*
  if(_port->isIntermediate())
    delete _port;
    */
}

void ProxyPort::edRemoveAllLinksLinkedWithMe() throw(Exception)
{
  _port->edRemoveAllLinksLinkedWithMe();
}

/*!
 * \note : Should never been called because Node clone process does not duplicate data attributes relative to links.
 *         This part is done afterwards on relink process.
 */
InputPort *ProxyPort::clone(Node *newHelder) const
{
  throw Exception("ProxyPort::clone : internal error - should never happened");
}

void ProxyPort::edNotifyReferencedBy(OutPort *fromPort)
{
  _port->edNotifyReferencedBy(fromPort);
}

void ProxyPort::edNotifyDereferencedBy(OutPort *fromPort)
{
  _port->edNotifyDereferencedBy(fromPort);
}

std::set<OutPort *> ProxyPort::edSetOutPort() const
{
  return _port->edSetOutPort();
}

int ProxyPort::edGetNumberOfLinks() const
{
  return _port->edGetNumberOfLinks();
}

void ProxyPort::exRestoreInit()
{
  _port->exRestoreInit();
}

void ProxyPort::exSaveInit()
{
  _port->exSaveInit();
}

InputPort *ProxyPort::getPublicRepresentant()
{ 
  return _port->getPublicRepresentant();
}

void *ProxyPort::get() const throw(Exception)
{
  return _port->get();
}

void ProxyPort::put(const void *data) throw(ConversionException)
{
  _port->put(data);
}

void ProxyPort::getAllRepresentants(std::set<InPort *>& repr) const
{
  DEBTRACE("ProxyPort::getAllRepresentants");
  _port->getAllRepresentants(repr);
}
