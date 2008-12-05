//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "ComponentInstance.hxx"
#include "Container.hxx"

#include <iostream>
#include <sstream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char ComponentInstance::KIND[]="";
int ComponentInstance::_total = 0;


const char ComponentInstance::NULL_FILE_REPR[]="No repr specified for ComponentInstance";

void ComponentInstance::setContainer(Container *cont)
{
  if (cont == _container) return;
  
  if(cont)
    cont->checkCapabilityToDealWith(this);
  if(_container)
    _container->decrRef();
  _container=cont;
  if(_container)
    _container->incrRef();
}

ComponentInstance::ComponentInstance(const std::string& name):_compoName(name),_isAttachedOnCloning(false),_container(0)
{
  _numId = _total++;
  stringstream instName;
  instName << _compoName << "_" << _numId;
  _instanceName = instName.str();
}

ComponentInstance::ComponentInstance(const ComponentInstance& other):_compoName(other._compoName),
                                                                     _container(0),
                                                                     _isAttachedOnCloning(other._isAttachedOnCloning)
{
  _numId = _total++;
  stringstream instName;
  instName << _compoName << "_" << _numId;
  _instanceName = instName.str();
  if(other._container)
    _container=other._container->clone();
}

ComponentInstance::~ComponentInstance()
{
  if(_container)
    _container->decrRef();
}

/*!
 * By calling this method the current container 'this' is not destined to be deeply copied on clone call.
 */
void ComponentInstance::attachOnCloning() const
{
  _isAttachedOnCloning=true;
}

std::string ComponentInstance::getFileRepr() const
{
  return NULL_FILE_REPR;
}

/*!
 * By calling this method the current container 'this' will be deeply copied on clone call.
 */
void ComponentInstance::dettachOnCloning() const
{
  _isAttachedOnCloning=false;
}

bool ComponentInstance::isAttachedOnCloning() const
{
  return _isAttachedOnCloning;
}

string ComponentInstance::getKind() const
{
  return KIND;
}
