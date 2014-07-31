// Copyright (C) 2006-2014  CEA/DEN, EDF R&D
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

#include "RuntimeSALOME.hxx"
#include "SalomeComponent.hxx"
#include "SalomeContainer.hxx"
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

const char SalomeComponent::KIND[]="Salome";

//! SalomeComponent constructor
SalomeComponent::SalomeComponent(const std::string& name): ComponentInstance(name)
{
  _objComponent=CORBA::Object::_nil();
}

//! SalomeComponent copy constructor
SalomeComponent::SalomeComponent(const SalomeComponent& other):ComponentInstance(other)
{
  _objComponent=CORBA::Object::_nil();
}

SalomeComponent::~SalomeComponent()
{
}

std::string SalomeComponent::getKind() const
{
  return KIND;
}

//! Unload the component 
void SalomeComponent::unload(ServiceNode *askingNode)
{
  //Not implemented
  std::cerr << "SalomeComponent::unload : not implemented " << std::endl;
}

//! Is the component instance already loaded ?
bool SalomeComponent::isLoaded(ServiceNode *askingNode)
{
  if(CORBA::is_nil(_objComponent))
    return false;
  else
    return true;
}

#ifdef SALOME_KERNEL
//! Load the component 
void SalomeComponent::load(ServiceNode *askingNode)
{
  if(_container)
    {
      _objComponent=((SalomeContainer*)_container)->loadComponent(askingNode);
      return;
    }
  //throw Exception("SalomeComponent::load : no container specified !!! To be implemented in executor to allocate default a Container in case of presenceOfDefaultContainer.");
  //This component has no specified container : use default container policy
  SALOME_NamingService ns(getSALOMERuntime()->getOrb());
  SALOME_LifeCycleCORBA LCC(&ns);
  Engines::ContainerParameters params;
  LCC.preSet(params);
  params.resource_params.name = "localhost";
  params.container_name ="FactoryServer";
  _objComponent=LCC.LoadComponent(params,_compoName.c_str());
}
#else
void SalomeComponent::load(ServiceNode *askingNode)
{
  throw Exception("YACS has been built without SALOME support");
}
#endif

//! Create a ServiceNode with this component instance and no input or output port
/*!
 *   \param name : node name
 *   \return       a new SalomeNode node
 */
ServiceNode* SalomeComponent::createNode(const std::string& name)
{
   SalomeNode* node=new SalomeNode(name);
   node->setComponent(this);
   return node;
}

//! Clone the component instance
ComponentInstance* SalomeComponent::clone() const
{
  if(_isAttachedOnCloning)
    {
      incrRef();
      return (ComponentInstance*) (this);
    }
  else
    return new SalomeComponent(*this);
}

std::string SalomeComponent::getFileRepr() const
{
  ostringstream stream;
  stream << "<component>" << getCompoName() << "</component>";
  return stream.str();
}

void SalomeComponent::setContainer(Container *cont)
{
  if (cont == _container) return;

  if(cont)
    cont->checkCapabilityToDealWith(this);

  if(_container)
    _container->decrRef();
  _container=cont;
  if(_container)
  {
    _container->incrRef();
    ((SalomeContainer*)_container)->addComponentName(_compoName);
  }
}

void SalomeComponent::shutdown(int level)
{
  DEBTRACE("SalomeComponent::shutdown " << level);
  if(_container)
    _container->shutdown(level);
}
