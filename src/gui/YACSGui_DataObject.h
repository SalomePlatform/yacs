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

#ifndef YACSGui_DataObject_HeaderFile
#define YACSGui_DataObject_HeaderFile

#include <Proc.hxx>

#include <LightApp_DataObject.h>

/*!
 * YACS module data object
 */
class YACSGui_DataObject : public virtual LightApp_DataObject
{
public:
  YACSGui_DataObject( SUIT_DataObject*, YACS::ENGINE::Proc* = 0 );
  virtual ~YACSGui_DataObject();
  
  virtual QString entry() const;
  
  QString name() const;
  QPixmap icon() const;
    
private:
  YACS::ENGINE::Proc* myObject;
};

/*!
 * YACS module root object
 */

class YACSGui_ModuleObject : public virtual YACSGui_DataObject,
			     public virtual LightApp_ModuleObject
{
public:
  YACSGui_ModuleObject ( CAM_DataModel*, SUIT_DataObject* = 0 );

  virtual QString name() const;
  virtual QString entry() const;
  
  QPixmap icon() const;
  QString toolTip() const;
};

#endif
