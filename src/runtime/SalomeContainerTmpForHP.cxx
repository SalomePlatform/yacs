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

#include "SalomeContainerTmpForHP.hxx"
#include "ComponentInstance.hxx"
#include "SalomeHPContainer.hxx"
#include "AutoLocker.hxx"
#include "Task.hxx"

using namespace YACS::ENGINE;

CORBA::Object_ptr SalomeContainerTmpForHP::loadComponent(Task *askingNode)
{
  const ComponentInstance *inst(askingNode?askingNode->getComponent():0);
  if(!inst)
    throw Exception("SalomeContainerTmpForHP::loadComponent : asking to load a component on the given task whereas this task has no component !");
  std::string compoName(inst->getCompoName());
  {
    YACS::BASES::AutoLocker<Container> alck(this);//To be sure
    if(!this->isAlreadyStarted(askingNode))
      this->start(askingNode);
  }
  CORBA::Object_ptr objComponent=CORBA::Object::_nil();
  {
    YACS::BASES::AutoLocker<Container> alck(this);//To be sure
    std::string compoName(inst->getCompoName());
    Engines::Container_var container(_launchModeType->getContainer(askingNode));
    objComponent=container->find_component_instance(compoName.c_str(),0);
    if(CORBA::is_nil(objComponent))
      {
        char *reason;
        bool isLoadable(container->load_component_Library(compoName.c_str(), reason));
        if(isLoadable)
          objComponent=SalomeContainerTools::CreateComponentInstance(this,container,inst);
      }
  }
  return objComponent;
}

SalomeContainerTmpForHP *SalomeContainerTmpForHP::BuildFrom(const SalomeHPContainer *cont, const Task *askingNode)
{
  const SalomeContainerHelper *lmt(0);
  std::size_t posIn(0);
  {
    YACS::BASES::AutoLocker<Container> altck(const_cast<SalomeHPContainer *>(cont));
    lmt=cont->getHelperOfTask(askingNode);
    posIn=cont->locateTask(askingNode);
  }
  return new SalomeContainerTmpForHP(*cont,cont->getContainerInfo(),lmt,cont->getComponentNames(),cont->getShutdownLev(),cont,posIn);
}
