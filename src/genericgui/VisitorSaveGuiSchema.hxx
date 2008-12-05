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
#ifndef _VISITORSAVEGUISCHEMA_HXX_
#define _VISITORSAVEGUISCHEMA_HXX_

#include "VisitorSaveSalomeSchema.hxx"

namespace YACS
{
  namespace ENGINE
  {
     class Proc;
  };

  namespace HMI
  {
    class VisitorSaveGuiSchema : public YACS::ENGINE::VisitorSaveSalomeSchema
    {
    public:
      VisitorSaveGuiSchema(YACS::ENGINE::Proc* proc);
      virtual ~VisitorSaveGuiSchema();
      
      virtual void visitProc(YACS::ENGINE::Proc *node);
      
    protected:
      virtual void writePresentation(YACS::ENGINE::Proc *proc);
      virtual void writeLinks(YACS::ENGINE::Proc *proc);
    };

  };
};

#endif
