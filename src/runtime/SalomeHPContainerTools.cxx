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

#include "SalomeHPContainerTools.hxx"
#include "SalomeHPContainer.hxx"
#include "AutoLocker.hxx"
#include "Exception.hxx"

#include <algorithm>

using namespace YACS::ENGINE;

void SalomeHPContainerVectOfHelper::resize(std::size_t sz)
{
  std::size_t oldSize(_launchModeType.size());
  if(sz==oldSize)
    return;
  checkNoCurrentWork();
  _whichOccupied.resize(sz); std::fill(_whichOccupied.begin(),_whichOccupied.end(),false);
  _launchModeType.resize(sz);
  for(std::size_t i=oldSize;i<sz;i++)
      _launchModeType[i]=new SalomeContainerMonoHelper;
  _currentlyWorking.clear();
}

std::size_t SalomeHPContainerVectOfHelper::getNumberOfFreePlace() const
{
  return std::count(_whichOccupied.begin(),_whichOccupied.end(),false);
}

void SalomeHPContainerVectOfHelper::allocateFor(const std::vector<const Task *>& nodes)
{
  for(std::vector<const Task *>::const_iterator it=nodes.begin();it!=nodes.end();it++)
    {
      if(!(*it))
        continue;
      if(_currentlyWorking.find(*it)!=_currentlyWorking.end())
        throw Exception("Searching to allocate for a ServiceNode instance already declared as allocated !");
      std::vector<bool>::iterator it2(std::find(_whichOccupied.begin(),_whichOccupied.end(),false));
      if(it2==_whichOccupied.end())
        throw Exception("All ressources are already occupied ! You are expected to wait for released resources !");
      std::size_t pos(std::distance(_whichOccupied.begin(),it2));
      _currentlyWorking[*it]=pos; _whichOccupied[pos]=true;
    }
}

void SalomeHPContainerVectOfHelper::release(const Task *node)
{
  if(!node)
    return ;
  std::map< const Task *,std::size_t >::iterator it(_currentlyWorking.find(node));
  if(it==_currentlyWorking.end())
    throw Exception("Request to release a resource not declared as working !");
  _whichOccupied[(*it).second]=false;
  _currentlyWorking.erase(it);
}

std::size_t SalomeHPContainerVectOfHelper::locateTask(const Task *node) const
{
  std::map< const Task *,std::size_t >::const_iterator it(_currentlyWorking.find(node));
  if(it==_currentlyWorking.end())
    throw Exception("current Node to be located is not marked as launched !");
  std::size_t ret((*it).second);
  checkPosInVec(ret);
  return ret;
}

const SalomeContainerMonoHelper *SalomeHPContainerVectOfHelper::getHelperOfTaskThreadSafe(const SalomeHPContainer *cont, const Task *node) const
{
  YACS::BASES::AutoLocker<Container> alck(const_cast<SalomeHPContainer *>(cont));
  return _launchModeType[locateTask(node)];
}

const SalomeContainerMonoHelper *SalomeHPContainerVectOfHelper::getHelperOfTask(const Task *node) const
{
  return _launchModeType[locateTask(node)];
}

SalomeContainerMonoHelper *SalomeHPContainerVectOfHelper::getHelperOfTaskThreadSafe(SalomeHPContainer *cont, const Task *node)
{
  YACS::BASES::AutoLocker<Container> alck(cont);
  return _launchModeType[locateTask(node)];
}

SalomeContainerMonoHelper *SalomeHPContainerVectOfHelper::getHelperOfTask(const Task *node)
{
  return _launchModeType[locateTask(node)];
}

void SalomeHPContainerVectOfHelper::checkNoCurrentWork() const
{
  for(std::map<const Task *,std::size_t >::const_iterator it=_currentlyWorking.begin();it!=_currentlyWorking.end();it++)
    if((*it).first)
      throw Exception("Something wrong a node is still declared to be using the ressource !");
  for(std::vector< YACS::BASES::AutoRefCnt<SalomeContainerMonoHelper> >::const_iterator it=_launchModeType.begin();it!=_launchModeType.end();it++)
    if((*it)->isAlreadyStarted(0))
      throw Exception("Some of the containers have be started ! Please shutdown them before !");
}

void SalomeHPContainerVectOfHelper::checkPosInVec(std::size_t pos) const
{
  if(pos<0 || pos>=_launchModeType.size())
    throw Exception("The task has been found, but its id is not in the correct range ! resize of of container size during run ?");
}
