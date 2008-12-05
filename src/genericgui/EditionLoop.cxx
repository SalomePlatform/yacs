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
#include "EditionLoop.hxx"
#include "FormLoop.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <cassert>
#include <sstream>

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

EditionLoop::EditionLoop(Subject* subject,
                         QWidget* parent,
                         const char* name)
  : EditionNode(subject, parent, name)
{
  _formLoop = new FormLoop(this);
  _wid->gridLayout1->addWidget(_formLoop);
  _formLoop->sb_nsteps->setMinimum(0);
  _formLoop->sb_nsteps->setMaximum(INT_MAX);
  connect(_formLoop->sb_nsteps, SIGNAL(valueChanged(const QString &)),
          this, SLOT(onModifyNbSteps(const QString &)));
}

EditionLoop::~EditionLoop()
{
}

void EditionLoop::onModifyNbSteps(const QString &text)
{
  SubjectForLoop *sfl = dynamic_cast<SubjectForLoop*>(_subject);
  assert(sfl);
  sfl->setNbSteps(text.toStdString());
}

void EditionLoop::synchronize()
{
  _subject->update(SETVALUE, 0, _subject);
}

void EditionLoop::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("EditionLoop::update " <<eventName(event) << " " << type);
  EditionNode::update(event, type, son);
  switch (event)
    {
    case SETVALUE:
      SubjectComposedNode * scn = dynamic_cast<SubjectComposedNode*>(_subject);
      string val = scn->getValue();
      istringstream ss(val);
      DEBTRACE(val);
      int i = 0;
      ss >> i;
      DEBTRACE(i);
      _formLoop->sb_nsteps->setValue(i);
      break;
    }
}
