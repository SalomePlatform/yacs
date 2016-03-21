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

#include <Python.h>
#include "SalomePythonComponent.hxx"
#include "SalomeComponent.hxx"
#include "SalomePythonNode.hxx"
#include "Exception.hxx"
#include "Container.hxx"

#include <sstream>

using namespace YACS::ENGINE;

const char SalomePythonComponent::KIND[]="SalomePy";

unsigned SalomePythonComponent::_cntForReprS = 0;

SalomePythonComponent::SalomePythonComponent(const std::string &name):ComponentInstance(name),_cntForRepr(_cntForReprS++)
{
}

SalomePythonComponent::SalomePythonComponent(const SalomePythonComponent& other):ComponentInstance(other),_cntForRepr(_cntForReprS++)
{
}

SalomePythonComponent::~SalomePythonComponent()
{
}

void SalomePythonComponent::load(Task *askingNode)
{
  if(_container)
    {
      _container->start(askingNode);
      return;
    }
  //This component has no specified container : use default container policy
  //given by getStringValueToExportInInterp()
  //throw Exception("SalomePythonComponent::load : no container specified !!! To be implemented in executor to allocate default a Container in case of presenceOfDefaultContainer.");
}

void SalomePythonComponent::unload(Task *askingNode)
{
}

bool SalomePythonComponent::isLoaded(Task *askingNode) const
{
  if(!_container)
    return false;
  else
    return _container->isAlreadyStarted(askingNode);
}

std::string SalomePythonComponent::getKind() const
{
  //This is not a bug !!!! SalomeComponent NOT SalomePythonComponent. This is for Container assignation.
  return SalomeComponent::KIND;
}

ComponentInstance* SalomePythonComponent::clone() const
{
  if(_isAttachedOnCloning)
    {
      incrRef();
      return (ComponentInstance*) (this);
    }
  else
    return new SalomePythonComponent(*this);
}

ComponentInstance *SalomePythonComponent::cloneAlways() const
{
  return new SalomePythonComponent(*this);
}

ServiceNode *SalomePythonComponent::createNode(const std::string &name)
{
  ServiceNode* node=new SalomePythonNode(name);
  node->setComponent(this);
  return node;
}

std::string SalomePythonComponent::getFileRepr() const
{
  std::ostringstream stream;
  stream << "<ref>" << "SalomePythonComponent #" << _cntForRepr << "</ref>";
  return stream.str();
}

std::string SalomePythonComponent::getStringValueToExportInInterp(const Task *askingNode) const
{
  if(!_container)
    return "localhost/FactoryServer";
  else
    return _container->getPlacementId(askingNode);
}
