//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D, OPEN CASCADE
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  File   : YACSGui_Selection.cxx
//  Author : Alexander BORODIN
//  Module : YACS
//
#include <YACSGui_Selection.h>
#include <YACSGui_Module.h>
#include <YACSGui_DataModel.h>

#include <LightApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Study.h>
#include <SALOME_ListIO.hxx>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

//!Constructor.
/*!
 */

YACSGui_Selection::YACSGui_Selection ()
  :LightApp_Selection()
{  
}

//!Destructor
/*!
 */
YACSGui_Selection::~YACSGui_Selection ()
{  
}

//!redefined method param
/*!
 */
QtxValue YACSGui_Selection::param( const int k, const QString& name) const
{
  DEBTRACE("YACSGui_Selection::param  name = " << name.latin1());
  QtxValue v;

  if ( name == "ObjectType" )
  {
    DEBTRACE("ObjectType= " <<  myCurrentObjectType.latin1());
    v = myCurrentObjectType;
    return v;
  }  
  
  return LightApp_Selection::param( k, name );
}

//!detection of type of current selected object
/*!
 */
void YACSGui_Selection::init( const QString& client, LightApp_SelectionMgr* mgr)
{
  LightApp_Selection::init(client, mgr);

  myCurrentObjectType = "";

  YACSGui_Module* module = dynamic_cast<YACSGui_Module*>( mgr->application()->activeModule() );
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( mgr->application()->activeStudy() );
  if ( study && module )
  {
    _PTR(Study) studyDS = study->studyDS();
    if ( studyDS )
    {
      SALOME_ListIO selObjects;
      mgr->selectedObjects( selObjects, false );

      //consider only single selection
      if ( selObjects.Extent() == 1 )
      {
        Handle(SALOME_InteractiveObject) io = selObjects.First();
        YACSGui_DataModel::ObjectType type = ((YACSGui_DataModel*)module->dataModel())->objectType( io->getEntry() );
        myCurrentObjectType = YACSGui_DataModel::Type2Str( type );
      }
    }
  }
}
