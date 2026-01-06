// Copyright (C) 2017-2026  CEA, EDF
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

#ifndef YDFXRESOURCEMODEL_H
#define YDFXRESOURCEMODEL_H

#include "AbstractResourceModel.hxx"

class YACSEvalYFX;
class YACSEvalListOfResources;
class YACSEvalParamsForCluster;

class YDFXWIDGETS_EXPORT YDFXResourceModel : public AbstractResourceModel
{
public:
    
  YDFXResourceModel(YACSEvalYFX* eval);
  virtual ~YDFXResourceModel();

  virtual bool getParallelizeStatus()const;
  virtual void setParallelizeStatus(bool v);

  virtual std::string getRemoteDir()const;
  virtual std::string getDefaultRemoteDir(std::string machine)const;
  virtual void setRemoteDir(const std::string& v);

  virtual std::string getLocalDir()const;
  virtual void setLocalDir(const std::string& v);

  virtual std::string getWckey()const;
  virtual void setWckey(const std::string& v);

  virtual int getMaxDurationMinutes()const;
  virtual void setMaxDurationMinutes(int v);

  virtual int getMaxDurationHours()const;
  virtual void setMaxDurationHours(int v);

  virtual unsigned int getNbprocs()const;
  virtual void setNbprocs(unsigned int v);

  virtual std::string getWantedMachine()const;
  virtual void setWantedMachine(const std::string& v);
  virtual std::vector<std::string> getFittingMachines()const;

  virtual const std::list<std::string>& getInFiles()const;
  virtual std::list<std::string>& getInFiles();

  virtual bool isMachineInteractive(const std::string& machine)const;

  void reset(YACSEvalYFX* eval);

private:

  YACSEvalListOfResources * resources();
  const YACSEvalListOfResources * resources()const;
  YACSEvalParamsForCluster& getClusterParams();
  const YACSEvalParamsForCluster& getClusterParams()const;
  void getMaxDuration(int& hours, int& minutes)const;
  void setMaxDuration(int hours, int minutes);

private:
  YACSEvalYFX* _eval;
};

#endif // YDFXRESOURCEMODEL_H
