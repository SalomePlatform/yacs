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
#include "VisitorSaveGuiSchema.hxx"
#include "Proc.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;
using namespace YACS::ENGINE;

VisitorSaveGuiSchema::VisitorSaveGuiSchema(YACS::ENGINE::Proc* proc)
  : VisitorSaveSalomeSchema(proc)
{
  DEBTRACE("VisitorSaveGuiSchema::VisitorSaveGuiSchema");
}

VisitorSaveGuiSchema::~VisitorSaveGuiSchema()
{
  DEBTRACE("VisitorSaveGuiSchema::~VisitorSaveGuiSchema");
}

void VisitorSaveGuiSchema::visitProc(YACS::ENGINE::Proc *node)
{
  DEBTRACE("VisitorSaveGuiSchema::visitProc");
  VisitorSaveSalomeSchema::visitProc(node);
}

void VisitorSaveGuiSchema::writePresentation(YACS::ENGINE::Proc *proc)
{
  DEBTRACE("VisitorSaveGuiSchema::writePresentation");
}

void VisitorSaveGuiSchema::writeLinks(YACS::ENGINE::Proc *proc)
{
  DEBTRACE("VisitorSaveGuiSchema::writeLinks");
}
