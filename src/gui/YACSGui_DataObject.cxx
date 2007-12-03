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

#include <OutPort.hxx>
#include <InPort.hxx>

#include <OutGate.hxx>
#include <OutputPort.hxx>
#include <OutputDataStreamPort.hxx>

#include <InlineNode.hxx>
#include <ServiceNode.hxx>
#include <Loop.hxx>
#include <ForEachLoop.hxx>
#include <Switch.hxx>
#include <ComposedNode.hxx>
#include <Proc.hxx>
#include <CORBAComponent.hxx>

using namespace YACS::ENGINE;

// YACSGui_DataObject class:

/*!
 * Constructor
 */
YACSGui_DataObject::YACSGui_DataObject( const _PTR(SObject)& sobj, SUIT_DataObject* parent, Proc* proc )
  :  SalomeApp_DataObject( sobj, parent )
{
  myProc = proc;
}

/*!
 * Destructor
 */
YACSGui_DataObject::~YACSGui_DataObject()
{
}

Proc* YACSGui_DataObject::proc() const
{
  return myProc;
}
