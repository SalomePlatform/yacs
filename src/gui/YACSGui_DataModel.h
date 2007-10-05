// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com

#ifndef YACSGui_DataModel_HeaderFile
#define YACSGui_DataModel_HeaderFile

#include <Proc.hxx>

#include <LightApp_DataModel.h>

#include <map>

class YACSGui_DataModel : public LightApp_DataModel
{

public:
  YACSGui_DataModel(CAM_Module* theModule);
  virtual ~YACSGui_DataModel();

  void add(YACS::ENGINE::Proc*, bool);
  void remove(YACS::ENGINE::Proc*);

  void addData(YACS::ENGINE::Proc*);

  bool isEditable(YACS::ENGINE::Proc*);

protected:
  virtual void build();

private:
  typedef std::map<YACS::ENGINE::Proc*, bool> ProcMap;
  ProcMap myProcs;

  std::list<YACS::ENGINE::Proc*> myProcData;
};

#endif
