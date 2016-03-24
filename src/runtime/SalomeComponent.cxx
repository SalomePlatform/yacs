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

#include "RuntimeSALOME.hxx"
#include "SalomeComponent.hxx"
#include "SalomeContainer.hxx"
#include "SalomeHPContainer.hxx"
#include "CORBANode.hxx"
#include "AutoLocker.hxx"

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

std::string SalomeComponent::getKindForNode() const
{
  return KIND;
}

//! Unload the component 
void SalomeComponent::unload(Task *askingNode)
{
  //Not implemented
  std::cerr << "SalomeComponent::unload : not implemented " << std::endl;
}

//! Is the component instance already loaded ?
bool SalomeComponent::isLoaded(Task *askingNode) const
{
  if(CORBA::is_nil(_objComponent))
    return false;
  else
    return true;
}

//#ifdef SALOME_KERNEL
//! Load the component 
void SalomeComponent::load(Task *askingNode)
{
  if(_container)
    {
      SalomeContainer *salomeContainer(dynamic_cast<SalomeContainer *>(_container));
      if(salomeContainer)
        {
          _objComponent=salomeContainer->loadComponent(askingNode);
          return ;
        }
      throw Exception("Unrecognized type of Container ! Only Salome are supported by the Salome components !");
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

ComponentInstance *SalomeComponent::cloneAlways() const
{
  return new SalomeComponent(*this);
}

std::string SalomeComponent::getFileRepr() const
{
  ostringstream stream;
  stream << "<component>" << getCompoName() << "</component>";
  return stream.str();
}

bool SalomeComponent::setContainer(Container *cont)
{
  if(!dynamic_cast<SalomeContainer *>(cont))
    throw Exception("SalomeComponent::setContainer : a Salome component must be attached to a Salome container !");
  if(ComponentInstance::setContainer(cont))
    {
      if(_container)
        _container->addComponentName(_compoName);
      return true;
    }
  else
    return false;
}

void SalomeComponent::shutdown(int level)
{
  DEBTRACE("SalomeComponent::shutdown " << level);
  if(_container)
    _container->shutdown(level);
}
