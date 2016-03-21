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

#include "OutputPort.hxx"
#include "ComposedNode.hxx"
#include "InputPort.hxx"
#include "InPropertyPort.hxx"
#include "Runtime.hxx"
#include "Node.hxx"

#include <sstream>
#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char OutputPort::NAME[]="OutputPort";

OutputPort::OutputPort(const OutputPort& other, Node *newHelder):DataFlowPort(other,newHelder),OutPort(other,newHelder),
                                                                 DataPort(other,newHelder),Port(other,newHelder)
{
}

OutputPort::OutputPort(const std::string& name, Node *node, TypeCode* type):DataFlowPort(name,node,type),OutPort(name,node,type),
                                                                       DataPort(name,node,type),Port(node)
{
}

string OutputPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

void OutputPort::edRemoveAllLinksLinkedWithMe() throw(YACS::Exception)
{
  set<InputPort *>::iterator iter;
  set<InputPort *> vec(_setOfInputPort);
  for( set<InputPort *>::iterator iter2=vec.begin();iter2!=vec.end();iter2++)
    edRemoveInputPort(*iter2,true);
  _setOfInputPort.clear();
}

void OutputPort::exInit()
{
}

void OutputPort::put(const void *data) throw(ConversionException)
{
  for(set<InputPort *>::iterator iter = _setOfInputPort.begin(); iter != _setOfInputPort.end(); iter++)
    (*iter)->put(data);
}

/**
 * check if output type is an input type and if a data converter exists before link
 */
bool OutputPort::edAddInputPort(InputPort *phyPort) throw(YACS::Exception)
{
  DEBTRACE("OutputPort::edAddInputPort");
  if(!isAlreadyInSet(phyPort))
    {
      InputPort *pwrap = getRuntime()->adapt(phyPort,
                                             _node->getImplementation(),
                                             this->edGetType());
      _setOfInputPort.insert(pwrap);
      modified();
      phyPort->modified();
      return true;
    }
  else
    return false;
}

bool OutputPort::edAddInPropertyPort(InPropertyPort *phyPort) throw(YACS::Exception)
{
  DEBTRACE("OutputPort::edAddInPropertyPort");
  if(!isAlreadyInSet(phyPort))
    {
      InputPort *pwrap = getRuntime()->adapt(phyPort,
                                             _node->getImplementation(),
                                             this->edGetType());
      _setOfInputPort.insert(pwrap);
      modified();
      phyPort->modified();
      return true;
    }
  else
    return false;
}

/**
 * Remove a link by performing not only the deletion in _setOfInputPort but also dereference to the target inputPort.
 * If 'forward' == true the forward deletion 
 * If 'forward' == false no forward deletion performed, oneway deletion without update 'inputPort' side.
 */
int OutputPort::edRemoveInputPort(InputPort *inputPort, bool forward) throw(YACS::Exception)
{
  if(forward)
    {
      set<InPort *> s;
      inputPort->getAllRepresentants(s);
      DEBTRACE("+++");
      for(set<InPort *>::iterator iter=s.begin();iter!=s.end();iter++)
        {
          DEBTRACE("---");
          _node->getRootNode()->edRemoveLink(this,*iter);
        }
      return -1;
    }
  else
    {
#ifdef NOCOVARIANT
      InPort *publicRepr=inputPort->getPublicRepresentant();
#else
      InputPort *publicRepr=inputPort->getPublicRepresentant();
#endif
      set<InputPort *>::iterator iter;
      for(iter=_setOfInputPort.begin();iter!=_setOfInputPort.end();iter++)
        if((*iter)->getPublicRepresentant()==publicRepr)
          break;
      if(iter!=_setOfInputPort.end())
        {
          (*iter)->modified();
          if((*iter)->isIntermediate())
            delete (*iter);
          _setOfInputPort.erase(iter);
          modified();
          return edGetNumberOfOutLinks();
        }
      else
        throw Exception("OutputPort::edRemoveInputPort : link does not exist, unable to remove it");
    }
}

OutputPort::~OutputPort()
{
  set<InputPort *>::iterator iter;
  for(iter=_setOfInputPort.begin();iter!=_setOfInputPort.end();iter++)
    {
      delete (*iter);
    }
}

bool OutputPort::isAlreadyLinkedWith(InPort *with) const
{
  InPort *publicRepr=with->getPublicRepresentant();
  set<InPort *> s;
  set<InputPort *>::const_iterator iter;
  for(iter=_setOfInputPort.begin();iter!=_setOfInputPort.end();iter++)
    {
    if((*iter)->getPublicRepresentant() == publicRepr)
      return true;
    }
  for(iter=_setOfInputPort.begin();iter!=_setOfInputPort.end();iter++)
    (*iter)->getAllRepresentants(s);
  for(set<InPort *>::iterator iter2=s.begin();iter2!=s.end();iter2++)
    {
    if((*iter2)->getPublicRepresentant() == publicRepr)
      return true;
    }
  return false;
}

bool OutputPort::isAlreadyInSet(InputPort *inputPort) const
{
#ifdef NOCOVARIANT
  InPort *publicRepr=inputPort->getPublicRepresentant();
#else
  InputPort *publicRepr=inputPort->getPublicRepresentant();
#endif
  for(set<InputPort *>::const_iterator iter=_setOfInputPort.begin();iter!=_setOfInputPort.end();iter++)
    if((*iter)->getPublicRepresentant()==publicRepr)
      return true;
  return false;
}

bool
OutputPort::isConnected() const
{
  return !_setOfInputPort.empty();
}


/**
 * check compatibility of port class ( an inputPort ) before trying to create the link.
 */
bool OutputPort::addInPort(InPort *inPort) throw(YACS::Exception)
{
  DEBTRACE("OutputPort::addInPort");
  if(inPort->getNameOfTypeOfCurrentInstance()!=InputPort::NAME &&
     inPort->getNameOfTypeOfCurrentInstance()!=InPropertyPort::NAME)
    {
      string what="not compatible type of port requested during building of link FROM ";
      what+=NAME; what+=" TO "; what+=inPort->getNameOfTypeOfCurrentInstance();
      throw Exception(what);
    }
  if (inPort->getNameOfTypeOfCurrentInstance() == InputPort::NAME)
  {
    return edAddInputPort(static_cast<InputPort*>(inPort));
  }
  else
  {
    return edAddInPropertyPort(static_cast<InPropertyPort*>(inPort));
  }
}

/**
 * check compatibility of port class ( an inputPort ) before trying to remove link WITHOUT forward.
 */
int OutputPort::removeInPort(InPort *inPort, bool forward) throw(YACS::Exception)
{
  if(inPort->getNameOfTypeOfCurrentInstance()!=InputPort::NAME && !forward)
    {
      string what="not compatible type of port requested during destruction of for link FROM ";
      what+=NAME; what+=" TO "; what+=inPort->getNameOfTypeOfCurrentInstance();
      throw Exception(what);
    }
  return edRemoveInputPort(static_cast<InputPort*>(inPort),forward);
}

std::set<InPort *> OutputPort::edSetInPort() const
{
  set<InPort *> s;
  for(set<InputPort *>::const_iterator iter=_setOfInputPort.begin();iter!=_setOfInputPort.end();iter++)
    (*iter)->getAllRepresentants(s);
  return s;
}

std::string OutputPort::dump()
{
  string xmldump = "<value><error> NO_SERIALISATION_AVAILABLE </error></value>";
  return xmldump;
}


//! Returns physical links linked to this. Contrary to edSetInPort that returns semantic links.
const std::set<InputPort *>& OutputPort::getSetOfPhyLinks() const
{
  return _setOfInputPort;
}

//! Check validity of output port. Nothing on base class
void OutputPort::checkBasicConsistency() const throw(YACS::Exception)
{
}
