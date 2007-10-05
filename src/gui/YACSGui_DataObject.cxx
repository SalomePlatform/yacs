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
#include <YACSGui_DataObject.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <CAM_Module.h>

using namespace YACS::ENGINE;

/*!
 * Constructor
 */
YACSGui_DataObject::YACSGui_DataObject( SUIT_DataObject* theParent, Proc* theProc )
  : LightApp_DataObject( theParent ),
    CAM_DataObject( theParent ),
    myObject( theProc )
{
}

/*!
 * Destructor
 */
YACSGui_DataObject::~YACSGui_DataObject()
{
}

/*!
 * Returns the data object icon
 */
QPixmap YACSGui_DataObject::icon() const
{
  return QPixmap();
}

/*!
 * Returns the data object name
 */
QString YACSGui_DataObject::name() const
{
  QString aName;
  
  if (myObject)
    aName = myObject->getName();
  
  return aName;
}

/*!
 * Returns the data object entry
 */
QString YACSGui_DataObject::entry() const
{
   return QString("");
}


/*!
 * Constructor
 */
YACSGui_ModuleObject::YACSGui_ModuleObject ( CAM_DataModel* theDataModel,   
					     SUIT_DataObject* theParent )
  : YACSGui_DataObject(theParent), 
    LightApp_ModuleObject( theDataModel, theParent ),
    CAM_DataObject( theParent )
{
} 

/*!
 * Gets name of the root
 */
QString YACSGui_ModuleObject::name() const
{
  return CAM_RootObject::name();
}


/*!
 * Gets an icon for the root
 */
QPixmap YACSGui_ModuleObject::icon() const
{
  QPixmap aRes;
  if ( dataModel() ) {
    QString anIconName = dataModel()->module()->iconName();
    if ( !anIconName.isEmpty() )
      aRes = SUIT_Session::session()->resourceMgr()->loadPixmap("YACSGui", anIconName, false);
  }
  return aRes;
}

/*!
 * Gets a tootip for the root
 */
QString YACSGui_ModuleObject::toolTip() const
{
  return QObject::tr( "YACS_ROOT_TOOLTIP" );
}

/*!
 * Inherited method, redefined
 */
QString YACSGui_ModuleObject::entry() const
{
  return "YACS_ModuleObject";
}
