// Copyright (C) 2006-2024  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef _EDITIONNODE_HXX_
#define _EDITIONNODE_HXX_

#include "ItemEdition.hxx"
#include <QLineEdit>

namespace YACS
{

  namespace HMI
  {
    class SubjectNode;

    class EditionNode: public ItemEdition
    {
      Q_OBJECT

    public slots:

    public:
      EditionNode(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionNode();
      virtual void update(GuiEvent event, int type, Subject* son);

    protected:
      QLineEdit *_le_fullName;
      SubjectNode *_subjectNode;
    };
  }
}
#endif
