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

void SalomeContainerMonoHelper::shutdown()
{
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
  const ComponentInstance *inst(askingNode?askingNode->getComponent():0);
  std::map<const ComponentInstance *,Engines::Container_var>::const_iterator it(_trueContainers.find(inst));
  if(it!=_trueContainers.end())
    return (*it).second;
  else
    return Engines::Container::_nil();
}

bool SalomeContainerMultiHelper::isAlreadyStarted(const Task *askingNode) const
{
  const ComponentInstance *inst(askingNode?askingNode->getComponent():0);
  if(_trueContainers.count(inst)==0)
    return false;
  else
    return true;
}

void SalomeContainerMultiHelper::setContainer(const Task *askingNode, Engines::Container_var cont)
{
  const ComponentInstance *inst(askingNode?askingNode->getComponent():0);
  _trueContainers[inst]=cont;
#ifdef REFCNT
    std::map<const ComponentInstance *, Engines::Container_var >::const_iterator it;
    for(it = _trueContainers.begin(); it != _trueContainers.end(); ++it)
      {
        DEBTRACE(it->second->_PR_getobj()->pd_refCount );
      }
#endif
}

void SalomeContainerMultiHelper::shutdown()
{
  for(std::map<const ComponentInstance *, Engines::Container_var >::const_iterator it = _trueContainers.begin(); it != _trueContainers.end(); ++it)
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
  _trueContainers.clear();
}

SalomeContainerMultiHelper::~SalomeContainerMultiHelper()
{
}
