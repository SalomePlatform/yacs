// Copyright (C) 2017-2024  CEA, EDF
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

#include "YDFXResourceModel.hxx"

#include "YACSEvalYFX.hxx"
#include "YACSEvalResource.hxx"
//#include "ResourcesManager.hxx" 

#include <algorithm>
#include <sstream>
#include <cstdlib>

YDFXResourceModel::YDFXResourceModel(YACSEvalYFX* eval)
: AbstractResourceModel(),
  _eval(eval)
{
}

YDFXResourceModel::~YDFXResourceModel()
{
}

bool YDFXResourceModel::getParallelizeStatus()const
{
  return _eval->getParallelizeStatus();
}

void YDFXResourceModel::setParallelizeStatus(bool v)
{
  _eval->setParallelizeStatus(v);
}

std::string YDFXResourceModel::getRemoteDir()const
{
  return getClusterParams().getRemoteWorkingDir();
}

void YDFXResourceModel::setRemoteDir(const std::string& v)
{
  getClusterParams().setRemoteWorkingDir(v);
}

std::string YDFXResourceModel::getDefaultRemoteDir(std::string machine)const
{
  return "/tmp";
}

std::string YDFXResourceModel::getLocalDir()const
{
  return getClusterParams().getLocalWorkingDir();
}

void YDFXResourceModel::setLocalDir(const std::string& v)
{
  getClusterParams().setLocalWorkingDir(v);
}

std::string YDFXResourceModel::getWckey()const
{
  return getClusterParams().getWCKey();
}

void YDFXResourceModel::setWckey(const std::string& v)
{
  getClusterParams().setWCKey(v);
}

int YDFXResourceModel::getMaxDurationMinutes()const
{
  int minutes;
  int hours;
  getMaxDuration(hours, minutes);
  return minutes;
}

void YDFXResourceModel::setMaxDurationMinutes(int v)
{
  int minutes;
  int hours;
  getMaxDuration(hours, minutes);
  setMaxDuration(hours, v);
}

int YDFXResourceModel::getMaxDurationHours()const
{
  int minutes;
  int hours;
  getMaxDuration(hours, minutes);
  return hours;
}

void YDFXResourceModel::setMaxDurationHours(int v)
{
  int minutes;
  int hours;
  getMaxDuration(hours, minutes);
  setMaxDuration(v, minutes);
}

unsigned int YDFXResourceModel::getNbprocs()const
{
  return getClusterParams().getNbProcs();
}

void YDFXResourceModel::setNbprocs(unsigned int v)
{
  getClusterParams().setNbProcs(v);
}

std::string YDFXResourceModel::getWantedMachine()const
{
  const std::string default_machine("localhost");
  std::vector<std::string> chosenMachines = resources()->getAllChosenMachines();
  std::vector<std::string> fittingMachines = resources()->getAllFittingMachines();
  std::string wantedMachine="";
  std::vector<std::string>::const_iterator it;
  bool foundMachine = false;
  it = chosenMachines.begin();
  // take the first chosen machine which is also fitting.
  while(!foundMachine && it!=chosenMachines.end())
  {
    if(std::find(fittingMachines.begin(), fittingMachines.end(), *it)
       != fittingMachines.end())
    {
      foundMachine = true;
      wantedMachine = *it;
    }
  }
  if(!foundMachine)
  {
    // take localhost if fitting
    if(std::find(fittingMachines.begin(), fittingMachines.end(),
                 default_machine) != fittingMachines.end())
    {
      foundMachine = true;
      wantedMachine = default_machine;
    }
    else if(fittingMachines.size() > 0)
    {
      foundMachine = true;
      wantedMachine = fittingMachines[0];
    }
  }
  return wantedMachine;
}

void YDFXResourceModel::setWantedMachine(const std::string& v)
{
  resources()->setWantedMachine(v);
}

std::vector<std::string> YDFXResourceModel::getFittingMachines()const
{
  return resources()->getAllFittingMachines();
}

const std::list<std::string>& YDFXResourceModel::getInFiles()const
{
  return getClusterParams().getInFiles();
}

std::list<std::string>& YDFXResourceModel::getInFiles()
{
  return getClusterParams().getInFiles();
}
  
bool YDFXResourceModel::isMachineInteractive(const std::string& machine)const
{
  return resources()->isMachineInteractive(machine);
}

void YDFXResourceModel::reset(YACSEvalYFX* eval)
{
  _eval = eval;
}

YACSEvalListOfResources* YDFXResourceModel::resources()
{
  return _eval->giveResources();
}

const YACSEvalListOfResources* YDFXResourceModel::resources()const
{
  return _eval->giveResources();
}

YACSEvalParamsForCluster& YDFXResourceModel::getClusterParams()
{
  return _eval->giveResources()->getAddParamsForCluster();
}

const YACSEvalParamsForCluster& YDFXResourceModel::getClusterParams()const
{
  return _eval->giveResources()->getAddParamsForCluster();
}

void YDFXResourceModel::getMaxDuration(int& hours, int& minutes)const
{
  std::string duration = getClusterParams().getMaxDuration();
  hours=0;
  minutes=0;
  if(duration.size() > 0)
  {
    std::stringstream ss(duration);
    std::string value;
    std::getline(ss, value, ':');
    hours = atoi(value.c_str());
    std::getline(ss, value, ':');
    minutes = atoi(value.c_str());
  }
  if(hours<0)
    hours = 0;
  if(hours>99)
    hours = 99;
  if(minutes<0)
    minutes = 0;
  if(minutes > 59)
    minutes = 59;
}

void YDFXResourceModel::setMaxDuration(int hours, int minutes)
{
  getClusterParams().setMaxDuration(AbstractResourceModel::getMaxDuration());
}