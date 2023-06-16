// Copyright (C) 2017-2023  CEA/DEN, EDF R&D
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

#ifndef SALOMERESOURCEMODEL_HXX
#define SALOMERESOURCEMODEL_HXX

#include "AbstractResourceModel.hxx"


class YDFXWIDGETS_EXPORT SalomeResourceModel : public AbstractResourceModel
{
public:
  SalomeResourceModel();
  ~SalomeResourceModel();
  virtual bool isMachineInteractive(const std::string& machine)const;
  virtual std::vector< std::string > getFittingMachines()const;
  virtual void setWantedMachine(const std::string& v);
  virtual std::string getWantedMachine()const;
  virtual void setNbprocs(unsigned int v);
  virtual unsigned int getNbprocs()const;
  virtual void setMaxDurationHours(int v);
  virtual int getMaxDurationHours()const;
  virtual void setMaxDurationMinutes(int v);
  virtual int getMaxDurationMinutes()const;
  virtual void setWckey(const std::string& v);
  virtual std::string getWckey()const;
  virtual void setLocalDir(const std::string& v);
  virtual std::string getLocalDir()const;
  virtual void setRemoteDir(const std::string& v);
  virtual std::string getRemoteDir()const;
  virtual std::string getDefaultRemoteDir(std::string machine)const;
  virtual void setParallelizeStatus(bool v);
  virtual bool getParallelizeStatus()const;
  virtual const std::list<std::string>& getInFiles()const;
  virtual std::list<std::string>& getInFiles();

private:
  std::string _wantedMachine;
  unsigned int _nbProcs;
  int _hours;
  int _minutes;
  std::string _wcKey;
  std::string _localDirectory;
  std::string _remoteDirectory;
  bool _parallelizeStatus;
  std::list<std::string> _in_files;
};

#endif // SALOMERESOURCEMODEL_HXX
