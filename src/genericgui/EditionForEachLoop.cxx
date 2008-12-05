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
#include "EditionForEachLoop.hxx"
#include "FormEachLoop.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <cassert>
#include <sstream>

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

EditionForEachLoop::EditionForEachLoop(Subject* subject,
                                       QWidget* parent,
                                       const char* name)
  : EditionNode(subject, parent, name)
{
  _formEachLoop = new FormEachLoop(this);
  _wid->gridLayout1->addWidget(_formEachLoop);
  _formEachLoop->sb_nbranch->setMinimum(1);
  _formEachLoop->sb_nbranch->setMaximum(INT_MAX);
  connect(_formEachLoop->sb_nbranch, SIGNAL(valueChanged(const QString &)),
          this, SLOT(onModifyNbBranches(const QString &)));
}

EditionForEachLoop::~EditionForEachLoop()
{
}

void EditionForEachLoop::onModifyNbBranches(const QString &text)
{
  SubjectForEachLoop *sfe = dynamic_cast<SubjectForEachLoop*>(_subject);
  assert(sfe);
  sfe->setNbBranches(text.toStdString());
}

void EditionForEachLoop::synchronize()
{
  _subject->update(SETVALUE, 0, _subject);
}

void EditionForEachLoop::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("EditionForEachLoop::update " << eventName(event) << " " << type);
  EditionNode::update(event, type, son);
  switch (event)
    {
    case SETVALUE:
      SubjectComposedNode * scn = dynamic_cast<SubjectComposedNode*>(_subject);
      string val = scn->getValue();
      istringstream ss(val);
      DEBTRACE( val);
      int i = 0;
      ss >> i;
      DEBTRACE(i);
      _formEachLoop->sb_nbranch->setValue(i);
      break;
    }
}
