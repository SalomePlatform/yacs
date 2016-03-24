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

#include "SalomeHPComponent.hxx"
#include "RuntimeSALOME.hxx"
#include "SalomeContainer.hxx"
#include "SalomeHPContainer.hxx"
#include "SalomeComponent.hxx" // for KIND
#include "SalomeContainerTmpForHP.hxx"
#include "CORBANode.hxx"

#ifdef SALOME_KERNEL
#include "SALOME_NamingService.hxx"
#include "SALOME_LifeCycleCORBA.hxx"
#endif

#include <omniORB4/CORBA.h>
#include <iostream>
#include <sstream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char SalomeHPComponent::KIND[]="HPSalome";

SalomeHPComponent::SalomeHPComponent(const std::string& name): ComponentInstance(name)
{
  _objComponent=CORBA::Object::_nil();
}

SalomeHPComponent::SalomeHPComponent(const SalomeHPComponent& other):ComponentInstance(other)
{
  _objComponent=CORBA::Object::_nil();
}

SalomeHPComponent::~SalomeHPComponent()
{
}

std::string SalomeHPComponent::getKind() const
{
  return KIND;
}

std::string SalomeHPComponent::getKindForNode() const
{
  return SalomeComponent::KIND;
}

//! Unload the component 
void SalomeHPComponent::unload(Task *askingNode)
{
  //Not implemented
  std::cerr << "SalomeHPComponent::unload : not implemented " << std::endl;
}

//! Is the component instance already loaded ?
bool SalomeHPComponent::isLoaded(Task *askingNode) const
{
  return false;
}

//#ifdef SALOME_KERNEL
//! Load the component 
void SalomeHPComponent::load(Task *askingNode)
{
  if(_container)
    {
      SalomeHPContainer *salomeContainer(dynamic_cast<SalomeHPContainer *>(_container));
      if(salomeContainer)
        {
          YACS::BASES::AutoCppPtr<SalomeContainerTmpForHP> tmpCont(SalomeContainerTmpForHP::BuildFrom(salomeContainer,askingNode));
          _objComponent=tmpCont->loadComponent(askingNode);
          return ;
        }
      throw Exception("Unrecognized type of Container ! Only Salome and HPSalome container are supported by the Salome components !");
    }
  else
    throw Exception("No container on HP component ! Impossible to load !");
}
/*#else
void SalomeComponent::load(Task *askingNode)
{
  throw Exception("YACS has been built without SALOME support");
}
#endif*/

//! Create a ServiceNode with this component instance and no input or output port
/*!
 *   \param name : node name
 *   \return       a new SalomeNode node
 */
ServiceNode* SalomeHPComponent::createNode(const std::string& name)
{
  SalomeNode* node(new SalomeNode(name));
   node->setComponent(this);
   return node;
}

//! Clone the component instance
ComponentInstance* SalomeHPComponent::clone() const
{
  if(_isAttachedOnCloning)
    {
      incrRef();
      return (ComponentInstance*) (this);
    }
  else
    return new SalomeHPComponent(*this);
}

ComponentInstance *SalomeHPComponent::cloneAlways() const
{
  return new SalomeHPComponent(*this);
}

std::string SalomeHPComponent::getFileRepr() const
{
  ostringstream stream;
  stream << "<component>" << getCompoName() << "</component>";
  return stream.str();
}

bool SalomeHPComponent::setContainer(Container *cont)
{
  if(!dynamic_cast<SalomeHPContainer *>(cont))
    throw Exception("SalomeHPComponent::setContainer : a Salome HP component must be attached to a Salome HP container !");
  if(ComponentInstance::setContainer(cont))
    {
      if(_container)
        _container->addComponentName(_compoName);
      return true;
    }
  else
    return false;
}

void SalomeHPComponent::shutdown(int level)
{
  DEBTRACE("SalomeHPComponent::shutdown " << level);
  if(_container)
    _container->shutdown(level);
}
