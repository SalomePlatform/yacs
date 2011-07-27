//  Copyright (C) 2006-2011  CEA/DEN, EDF R&D
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

#ifndef _EDITIONOPTIMIZERLOOP_HXX_
#define _EDITIONOPTIMIZERLOOP_HXX_

#include "EditionNode.hxx"

namespace YACS
{
  namespace HMI
  {
    class FormOptimizerLoop;

    class EditionOptimizerLoop: public EditionNode
    {
      Q_OBJECT

    public slots:
      virtual void onModifyInitFile();
      virtual void onModifyEntry();
      virtual void onModifyLib();
      virtual void onNbBranchesEdited();

    public:
      EditionOptimizerLoop(Subject* subject,
                         QWidget* parent = 0,
                         const char* name = 0);
      virtual ~EditionOptimizerLoop();
      virtual void synchronize();
      virtual void update(GuiEvent event, int type, Subject* son);

    protected:
      FormOptimizerLoop *_formOptimizerLoop;
      int _nbBranches;
    };
  }
}
#endif
