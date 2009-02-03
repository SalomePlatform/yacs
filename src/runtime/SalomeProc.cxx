//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
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
#include "SalomeProc.hxx"
#include "Runtime.hxx"
#include "TypeCode.hxx"
#include "VisitorSaveSalomeSchema.hxx"
#include <iostream>

using namespace YACS::ENGINE;

TypeCode * SalomeProc::createInterfaceTc(const std::string& id, const std::string& name,
                                   std::list<TypeCodeObjref *> ltc)
{
  std::string myName;
  if(id == "") myName = "IDL:" + name + ":1.0";
  else myName = id;
  return Proc::createInterfaceTc(myName,name,ltc);
}
TypeCode * SalomeProc::createStructTc(const std::string& id, const std::string& name)
{
  std::string myName;
  if(id == "") myName = "IDL:" + name + ":1.0";
  else myName = id;
  return Proc::createStructTc(myName,name);
}

SalomeProc::~SalomeProc()
{
}

//! Save Proc in XML schema file
/*!
 * \param xmlSchemaFile: the file name
 */
void SalomeProc::saveSchema(std::string xmlSchemaFile)
{
  VisitorSaveSalomeSchema vss(this);
  vss.openFileSchema(xmlSchemaFile);
  accept(&vss);
  vss.closeFileSchema();
}


