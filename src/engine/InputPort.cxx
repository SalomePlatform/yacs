// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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

InputPort::InputPort(const InputPort& other, Node *newHelder)
  : DataFlowPort(other, newHelder),
    InPort(other, newHelder),
    DataPort(other, newHelder),
    Port(other, newHelder),
    _initValue(0),
    _canBeNull(other._canBeNull)
{
  if(other._initValue)
    _initValue=other._initValue->clone();
}

InputPort::InputPort(const std::string& name, Node *node, TypeCode* type, bool canBeNull)
  : DataFlowPort(name, node, type),
    InPort(name, node, type),
    DataPort(name, node, type),
    Port(node),
    _initValue(0),
    _canBeNull(canBeNull)
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
  return (edIsManuallyInitialized() || _backLinks.size()!=0 );
}

InputPort::~InputPort()
{
  if(_initValue)
    _initValue->decrRef();
}

void InputPort::edInit(Any *value)
{
  InputPort *manuallySet=getRuntime()->adapt(this,
                                             Runtime::RUNTIME_ENGINE_INTERACTION_IMPL_NAME,_type,true);
  try
    {
      manuallySet->put((const void *) value);
      if(manuallySet!=this)
        delete manuallySet;
    }
  catch(ConversionException&)
    {
      if(manuallySet!=this)
        delete manuallySet;
      throw;
    }
  exSaveInit();
  modified();
}

//! Initialize the port with an object (value) coming from a world with implementation impl
/*!
 *  You should be careful when using this method : the caller must set the context according to implementation
 *  For instance, if implementation is Python, the caller must hold the Global Interpreter Lock (also known as GIL).
 */
void InputPort::edInit(const std::string& impl,const void* value)
{
  InputPort *manuallySet=getRuntime()->adapt(this,impl,_type,true);
  try
    {
      manuallySet->put(value);
      if(manuallySet!=this)
        delete manuallySet;
    }
  catch(ConversionException&)
    {
      if(manuallySet!=this)
        delete manuallySet;
      throw;
    }
  exSaveInit();
  modified();
}

//! Removes eventually previous manual initialisation.
void InputPort::edRemoveManInit()
{
  if(_initValue)
    _initValue->decrRef();
  _initValue=0;
}

//! Check basically that this port has one chance to be specified on time. It's a necessary condition \b not \b sufficient at all.
void InputPort::checkBasicConsistency() const throw(YACS::Exception)
{
  if(!_canBeNull && !edIsManuallyInitialized() && _backLinks.size()==0 )
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

std::string InputPort::getHumanRepr()
{
  return dump();
}

void InputPort::setStringRef(std::string strRef)
{
  _stringRef = strRef;
}

bool InputPort::canBeNull() const
{
  return _canBeNull;
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

void ProxyPort::edRemoveAllLinksLinkedWithMe() throw(YACS::Exception)
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

#ifdef NOCOVARIANT
InPort *ProxyPort::getPublicRepresentant()
#else
InputPort *ProxyPort::getPublicRepresentant()
#endif
{ 
  return _port->getPublicRepresentant();
}

void *ProxyPort::get() const
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
