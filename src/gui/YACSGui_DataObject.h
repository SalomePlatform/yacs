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

#include <Port.hxx>
#include <Node.hxx>
#include <ServiceNode.hxx>
#include <Proc.hxx>
#include <SalomeContainer.hxx>
#include <ComponentInstance.hxx>

//#include <LightApp_DataObject.h>
#include <SalomeApp_DataObject.h>
#include "SALOMEDSClient.hxx"

/*!
 * YACS module data object
 */
class YACSGui_DataObject : public SalomeApp_DataObject
{
public:
  //enum ObjectType { SchemaObject, RunObject, NodeObject };
  YACSGui_DataObject( const _PTR(SObject)&, SUIT_DataObject* = 0, YACS::ENGINE::Proc*  = 0 );
  virtual ~YACSGui_DataObject();

  YACS::ENGINE::Proc* proc() const;

protected:
  //ObjectType myType;
  YACS::ENGINE::Proc*   myProc;
};

#endif
