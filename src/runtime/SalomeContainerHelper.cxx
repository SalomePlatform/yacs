// Copyright (C) 2006-2023  CEA/DEN, EDF R&D
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

#include "SalomeContainerHelper.hxx"

#include "ServiceNode.hxx"
#include "YacsTrace.hxx"

#include <sstream>
#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

const char SalomeContainerMonoHelper::TYPE_NAME[]="mono";

const char SalomeContainerMonoHelper::DFT_LAUNCH_MODE[]="start";

const char SalomeContainerMultiHelper::TYPE_NAME[]="multi";

const char SalomeContainerMultiHelper::DFT_LAUNCH_MODE[]="getorstart";

SalomeContainerHelper::~SalomeContainerHelper()
{
}

SalomeContainerMonoHelper::SalomeContainerMonoHelper():_trueCont(Engines::Container::_nil())
{

}

std::string SalomeContainerMonoHelper::getType() const
{
  return TYPE_NAME;
}

std::string SalomeContainerMonoHelper::getDftLaunchMode() const
{
  return std::string(DFT_LAUNCH_MODE);
}

SalomeContainerMonoHelper *SalomeContainerMonoHelper::deepCpyOnlyStaticInfo() const
{
  return new SalomeContainerMonoHelper;
}

Engines::Container_var SalomeContainerMonoHelper::getContainer(const Task *askingNode) const
{
  return _trueCont;
}

bool SalomeContainerMonoHelper::isAlreadyStarted(const Task *askingNode) const
{
  if(CORBA::is_nil(_trueCont))
    return false;
  else
    return true;
}

void SalomeContainerMonoHelper::setContainer(const Task *askingNode, Engines::Container_var cont)
{
  _trueCont=cont;
#ifdef REFCNT
    DEBTRACE(_trueCont->_PR_getobj()->pd_refCount );
#endif
}

bool SalomeContainerMonoHelper::isKernelContNull() const
{
  return CORBA::is_nil(_trueCont);
}

void SalomeContainerMonoHelper::shutdown()
{
  if(CORBA::is_nil(_trueCont))
    return ;
  try
  {
      DEBTRACE("shutdown SALOME container: " );
      CORBA::String_var containerName=_trueCont->name();
      DEBTRACE(containerName);
      _trueCont->Shutdown();
      std::cerr << "shutdown SALOME container: " << containerName << std::endl;
  }
  catch(...)
  {
      DEBTRACE("Unknown exception ignored." );
  }
  _trueCont=Engines::Container::_nil();
}

std::string SalomeContainerMonoHelper::getKernelContainerName() const
{
  if(CORBA::is_nil(_trueCont))
    return std::string("NULL");
  CORBA::String_var containerName(_trueCont->name());
  std::string ret(containerName);
  return ret;
}

SalomeContainerMonoHelper::~SalomeContainerMonoHelper()
{
}

std::string SalomeContainerMultiHelper::getType() const
{
  return TYPE_NAME;
}

std::string SalomeContainerMultiHelper::getDftLaunchMode() const
{
  return std::string(DFT_LAUNCH_MODE);
}

SalomeContainerMultiHelper *SalomeContainerMultiHelper::deepCpyOnlyStaticInfo() const
{
  return new SalomeContainerMultiHelper;
}

Engines::Container_var SalomeContainerMultiHelper::getContainer(const Task *askingNode) const
{
  std::unique_lock<std::mutex> lock(_data_mutex);
  const ComponentInstance *inst(askingNode?askingNode->getComponent():0);
  if(inst == nullptr && askingNode != nullptr && askingNode->hasImposedResource())
  {
    std::map<const Task *,Engines::Container_var>::const_iterator it(_containersForTasks.find(askingNode));
    if(it!=_containersForTasks.end())
      return (*it).second;
    else
      return Engines::Container::_nil();
  }
  else
  {
    std::map<const ComponentInstance *,Engines::Container_var>::const_iterator it(_containersForComponents.find(inst));
    if(it!=_containersForComponents.end())
      return (*it).second;
    else
      return Engines::Container::_nil();
  }
}

bool SalomeContainerMultiHelper::isAlreadyStarted(const Task *askingNode) const
{
  std::unique_lock<std::mutex> lock(_data_mutex);
  const ComponentInstance *inst(askingNode?askingNode->getComponent():0);
  if(inst == nullptr && askingNode != nullptr && askingNode->hasImposedResource())
  {
    return _containersForTasks.count(askingNode) > 0;
  }
  else
  {
    if(_containersForComponents.count(inst)==0)
      return false;
    else
      return true;
  }
}

void SalomeContainerMultiHelper::setContainer(const Task *askingNode, Engines::Container_var cont)
{
  std::unique_lock<std::mutex> lock(_data_mutex);
  const ComponentInstance *inst(askingNode?askingNode->getComponent():0);
  if(inst == nullptr && askingNode != nullptr && askingNode->hasImposedResource())
  {
    _containersForTasks[askingNode] = cont;
  }
  else
  {
    _containersForComponents[inst]=cont;
#ifdef REFCNT
    std::map<const ComponentInstance *, Engines::Container_var >::const_iterator it;
    for(it = _containersForComponents.begin(); it != _containersForComponents.end(); ++it)
      {
        DEBTRACE(it->second->_PR_getobj()->pd_refCount );
      }
#endif
  }
}

void SalomeContainerMultiHelper::shutdown()
{
  std::unique_lock<std::mutex> lock(_data_mutex);
  for(std::map<const Task *, Engines::Container_var >::const_iterator it = _containersForTasks.begin();
      it != _containersForTasks.end(); ++it)
  {
    try
    {
        DEBTRACE("shutdown SALOME container: " );
        CORBA::String_var containerName=it->second->name();
        DEBTRACE(containerName);
        it->second->Shutdown();
        std::cerr << "shutdown SALOME container: " << containerName << std::endl;
    }
    catch(CORBA::Exception&)
    {
        DEBTRACE("Unexpected CORBA failure detected." );
    }
    catch(...)
    {
        DEBTRACE("Unknown exception ignored." );
    }
  }
  _containersForTasks.clear();

  for(std::map<const ComponentInstance *, Engines::Container_var >::const_iterator it = _containersForComponents.begin(); it != _containersForComponents.end(); ++it)
    {
      try
      {
          DEBTRACE("shutdown SALOME container: " );
          CORBA::String_var containerName=it->second->name();
          DEBTRACE(containerName);
          it->second->Shutdown();
          std::cerr << "shutdown SALOME container: " << containerName << std::endl;
      }
      catch(CORBA::Exception&)
      {
          DEBTRACE("Unexpected CORBA failure detected." );
      }
      catch(...)
      {
          DEBTRACE("Unknown exception ignored." );
      }
    }
  _containersForComponents.clear();
}

SalomeContainerMultiHelper::~SalomeContainerMultiHelper()
{
}
