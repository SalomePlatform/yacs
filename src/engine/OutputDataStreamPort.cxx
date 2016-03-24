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

#include "OutputDataStreamPort.hxx"
#include "InputDataStreamPort.hxx"
#include "ComposedNode.hxx"
#include "InPort.hxx"
#include "TypeCode.hxx"
#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char OutputDataStreamPort::NAME[]="OutputDataStreamPort";

OutputDataStreamPort::OutputDataStreamPort(const OutputDataStreamPort& other, Node *newHelder):DataStreamPort(other,newHelder),
                                                                                               OutPort(other,newHelder),
                                                                                               DataPort(other,newHelder),
                                                                                               Port(other,newHelder)
{
}

OutputDataStreamPort::OutputDataStreamPort(const std::string& name, Node *node, TypeCode* type):DataStreamPort(name,node,type),
OutPort(name,node,type),
DataPort(name,node,type),
Port(node)
{
}

OutputDataStreamPort::~OutputDataStreamPort()
{
}

OutputDataStreamPort *OutputDataStreamPort::clone(Node *newHelder) const
{
  return new OutputDataStreamPort(*this,newHelder);
}

std::set<InPort *> OutputDataStreamPort::edSetInPort() const
{
  set<InPort *> s;
  for(set<InputDataStreamPort *>::const_iterator iter=_setOfInputDataStreamPort.begin();iter!=_setOfInputDataStreamPort.end();iter++)
    (*iter)->getAllRepresentants(s);
  return s;
}

bool OutputDataStreamPort::isAlreadyLinkedWith(InPort *with) const
{
  set<InPort *> s;
  set<InputDataStreamPort *>::const_iterator iter;
  for(iter=_setOfInputDataStreamPort.begin();iter!=_setOfInputDataStreamPort.end();iter++)
    if(*iter==with)
      return true;
  for(iter=_setOfInputDataStreamPort.begin();iter!=_setOfInputDataStreamPort.end();iter++)
    (*iter)->getAllRepresentants(s);
  for(set<InPort *>::iterator iter2=s.begin();iter2!=s.end();iter2++)
    if((*iter2)==with)
      return true;
  return false;
}

string OutputDataStreamPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

bool OutputDataStreamPort::edAddInputDataStreamPort(InputDataStreamPort *port)
  throw(ConversionException)
{
  DEBTRACE("OutputDataStreamPort::edAddInputDataStreamPort");
  if(!isAlreadyInSet(port))
    {
      if(!port->edGetType()->isAdaptable(edGetType()))
        {
          string what="Can not connect 2 ports with incompatible types : ";
          what=what+ port->edGetType()->id();
          what=what+" is not a ";
          what=what+ edGetType()->id();
          throw ConversionException(what);
        }
      _setOfInputDataStreamPort.insert(port);
      port->edAddOutputDataStreamPort(this);
      return true;
    }
  else
    return false;
}

int OutputDataStreamPort::edRemoveInputDataStreamPort(InputDataStreamPort *inPort, bool forward) throw(YACS::Exception)
{
  if(forward)
    {
      set<InPort *> s;
      inPort->getAllRepresentants(s);
      for(set<InPort *>::iterator iter=s.begin();iter!=s.end();iter++)
        _node->getRootNode()->edRemoveLink(this,*iter);
      return -1;
    }
  else    
    {
      set<InputDataStreamPort *>::iterator iter=_setOfInputDataStreamPort.find(inPort);
      if(iter!=_setOfInputDataStreamPort.end())
        {
          (*iter)->modified();
          _setOfInputDataStreamPort.erase(iter);
          modified();
          return edGetNumberOfOutLinks();
        }
      else
        throw Exception("OutputDataStreamPort::edRemoveInputPort : link does not exist, unable to remove it");
    }
}

bool OutputDataStreamPort::addInPort(InPort *inPort) throw(YACS::Exception)
{
  DEBTRACE("OutputDataStreamPort::addInPort");
  if(inPort->getNameOfTypeOfCurrentInstance()!=InputDataStreamPort::NAME)
    {
      string what="not compatible type of port requested during building of link FROM ";
      what+=NAME; what+=" TO "; what+=inPort->getNameOfTypeOfCurrentInstance();
      throw Exception(what);
    }
  return edAddInputDataStreamPort(static_cast<InputDataStreamPort*>(inPort));
}

void OutputDataStreamPort::edRemoveAllLinksLinkedWithMe() throw(YACS::Exception)
{
  set<InputDataStreamPort *>::iterator iter;
  set<InputDataStreamPort *> vec(_setOfInputDataStreamPort);
  for( set<InputDataStreamPort *>::iterator iter2=vec.begin();iter2!=vec.end();iter2++)
    edRemoveInputDataStreamPort(*iter2,true);
  _setOfInputDataStreamPort.clear();
}

int OutputDataStreamPort::removeInPort(InPort *inPort, bool forward) throw(YACS::Exception)
{
  DEBTRACE("OutputDataStreamPort::removeInPort");
  if(inPort->getNameOfTypeOfCurrentInstance()!=InputDataStreamPort::NAME && !forward)
    {
      string what="not compatible type of port requested during destruction of for link FROM ";
      what+=NAME; what+=" TO "; what+=inPort->getNameOfTypeOfCurrentInstance();
      throw Exception(what);
    }
  return edRemoveInputDataStreamPort(static_cast<InputDataStreamPort *>(inPort),forward);
}

bool OutputDataStreamPort::isAlreadyInSet(InputDataStreamPort *inPort) const
{
  return _setOfInputDataStreamPort.find(inPort)!=_setOfInputDataStreamPort.end();
}
