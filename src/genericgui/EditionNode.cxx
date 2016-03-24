// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
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

#include "EditionNode.hxx"
#include "QtGuiContext.hxx"
#include "Proc.hxx"

#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;
using namespace YACS::ENGINE;

EditionNode::EditionNode(Subject* subject,
                         QWidget* parent,
                         const char* name)
  : ItemEdition(subject, parent, name)
{
  DEBTRACE("EditionNode::EditionNode");
  if (QtGuiContext::getQtCurrent()->isEdition())
    _wid->le_name->setReadOnly(false);

  _subjectNode = dynamic_cast<SubjectNode*>(_subject);
  YASSERT(_subjectNode);

  _le_fullName = 0;
  if (!dynamic_cast<SubjectProc*>(_subject))
    {
      _le_fullName = new QLineEdit(this);
      _le_fullName->setToolTip("Full name of the node in schema");
      _le_fullName->setReadOnly(true);
      _wid->gridLayout1->addWidget(_le_fullName);

      YACS::ENGINE::Proc* proc = QtGuiContext::getQtCurrent()->getProc();
      _le_fullName->setText((proc->getChildName(_subjectNode->getNode())).c_str());
    }
}

EditionNode::~EditionNode()
{
}

void EditionNode::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("EditionNode::update ");
  ItemEdition::update(event, type, son);
  YACS::ENGINE::Proc* proc = QtGuiContext::getQtCurrent()->getProc();
  switch (event)
    {
    case RENAME:
      if (_le_fullName)
        _le_fullName->setText((proc->getChildName(_subjectNode->getNode())).c_str());
      break;
    default:
      ;
    }
}
