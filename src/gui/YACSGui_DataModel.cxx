//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <YACSGui_DataModel.h>
#include <YACSGui_DataObject.h>

#include <CAM_Module.h>
#include <CAM_Application.h>
#include <LightApp_Study.h>


using namespace YACS::ENGINE;

/*!
  Constructor
*/
YACSGui_DataModel::YACSGui_DataModel( CAM_Module* theModule ) :
  LightApp_DataModel(theModule)
{
}

/*!
  Destructor
*/
YACSGui_DataModel::~YACSGui_DataModel()
{
}


/*!
  Add item to the map of procs
*/
void YACSGui_DataModel::add(Proc* theProc, bool isEditable)
{
  myProcs[theProc] = isEditable;
}

/*!
  Remove item from the map of procs
*/
void YACSGui_DataModel::remove(Proc* theProc)
{
  myProcs.erase(theProc);
}

/*!
  Check if the proc is editable
*/
bool YACSGui_DataModel::isEditable(Proc* theProc)
{
  return (*myProcs.find(theProc)).second;
}

/*!
  Builds tree.
*/
void YACSGui_DataModel::build()
{
  YACSGui_ModuleObject* aRoot = dynamic_cast<YACSGui_ModuleObject*>( root() );
  if( !aRoot )  {
    aRoot = new YACSGui_ModuleObject( this, 0 );
    setRoot( aRoot );
  }

  std::list<Proc*>::iterator it = myProcData.begin();
  for(; it != myProcData.end(); it++) {
    Proc* aProc = *it;
    YACSGui_DataObject* aDataObj = new YACSGui_DataObject( aRoot, aProc );
  }
}

 void YACSGui_DataModel::addData(Proc* theProc)
{
  if(myProcData.size() > 0) {
    std::list<Proc*>::iterator result = std::find(myProcData.begin(), myProcData.end(), theProc);
    if (result != myProcData.end())
      return;
  }
  
  myProcData.push_back(theProc);
  update();
}
