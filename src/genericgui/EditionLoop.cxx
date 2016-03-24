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

#include "EditionLoop.hxx"
#include "FormLoop.hxx"
#include "Node.hxx"
#include "OutputPort.hxx"
#include "QtGuiContext.hxx"

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
  _nbsteps = 0;
  _wid->gridLayout1->addWidget(_formLoop);

  QHBoxLayout* _hbl_index = new QHBoxLayout();
  QLabel* _la_index = new QLabel(this);
  _hbl_index->addWidget(_la_index);
  _la_index->setText("index:");
  _le_index = new QLineEdit(this);
  _le_index->setText(QString::number(0));
  _le_index->setReadOnly(true);
  _hbl_index->addWidget(_le_index);
  _formLoop->gridLayout->addLayout(_hbl_index, 1, 0);

  _formLoop->sb_nsteps->setMinimum(0);
  _formLoop->sb_nsteps->setMaximum(INT_MAX);
  if (!QtGuiContext::getQtCurrent()->isEdition())
    _formLoop->setEnabled (false);
  
  connect(_formLoop->sb_nsteps, SIGNAL(editingFinished()),
          this, SLOT(onNbStepsEdited()));
}

EditionLoop::~EditionLoop()
{
}

void EditionLoop::onNbStepsEdited()
{
  int newval = _formLoop->sb_nsteps->value();
  DEBTRACE("EditionLoop::onNbStepsEdited " << _nbsteps << " --> " << newval);
  if (newval != _nbsteps)
    {
      SubjectForLoop *sfl = dynamic_cast<SubjectForLoop*>(_subject);
      YASSERT(sfl);
      QString text = _formLoop->sb_nsteps->cleanText();
      sfl->setNbSteps(text.toStdString());
    }
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
      {
        SubjectComposedNode * scn = dynamic_cast<SubjectComposedNode*>(_subject);
        string val = scn->getValue();
        istringstream ss(val);
        DEBTRACE(val);
        int i = 0;
        ss >> i;
        DEBTRACE(i);
        _formLoop->sb_nsteps->setValue(i);
        _nbsteps = i;

        YACS::ENGINE::OutputPort* odp=scn->getNode()->getOutputPort("index");
        SubjectDataPort* sodp = QtGuiContext::getQtCurrent()->_mapOfSubjectDataPort[odp];
        _le_index->setText(QString::fromStdString(sodp->getExecValue()));
        break;
      }
    case UPDATEPROGRESS:
      {
        SubjectComposedNode * scn = dynamic_cast<SubjectComposedNode*>(_subject);
        YACS::ENGINE::OutputPort* odp=scn->getNode()->getOutputPort("index");
        SubjectDataPort* sodp = QtGuiContext::getQtCurrent()->_mapOfSubjectDataPort[odp];
        _le_index->setText(QString::fromStdString(sodp->getExecValue()));
        break;
      }
    }
}
