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

#include "EditionForEachLoop.hxx"
#include "FormEachLoop.hxx"
#include "Node.hxx"
#include "ForEachLoop.hxx"
#include "TypeCode.hxx"
#include "QtGuiContext.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <cassert>
#include <sstream>

using namespace std;

using namespace YACS;
using namespace YACS::HMI;
using namespace YACS::ENGINE;

EditionForEachLoop::EditionForEachLoop(Subject* subject,
                                       QWidget* parent,
                                       const char* name)
  : EditionNode(subject, parent, name)
{
  _formEachLoop = new FormEachLoop(this);
  _nbBranches = 1;
  _wid->gridLayout1->addWidget(_formEachLoop);
  _formEachLoop->sb_nbranch->setMinimum(1);
  _formEachLoop->sb_nbranch->setMaximum(INT_MAX);
  if (!QtGuiContext::getQtCurrent()->isEdition())
    _formEachLoop->setEnabled (false);
  Node* node=_subjectNode->getNode();
  ForEachLoop *fe = dynamic_cast<ForEachLoop*>(node);
  if(fe)
    _formEachLoop->lineEdit->setText(fe->edGetSamplePort()->edGetType()->name());
  connect(_formEachLoop->sb_nbranch, SIGNAL(editingFinished()),
          this, SLOT(onNbBranchesEdited()));

  connect(_formEachLoop->lineEdit_2, SIGNAL(editingFinished()),this,SLOT(onModifyCollection()));

}

EditionForEachLoop::~EditionForEachLoop()
{
}

void EditionForEachLoop::onModifyCollection()
{
  bool isOk = false;
  Node* node=_subjectNode->getNode();
  ForEachLoop *fe = dynamic_cast<ForEachLoop*>(node);
  InputPort* dp=fe->edGetSeqOfSamplesPort();
  SubjectDataPort* sdp = QtGuiContext::getQtCurrent()->_mapOfSubjectDataPort[dp];
  isOk=sdp->setValue(_formEachLoop->lineEdit_2->text().toStdString());
  DEBTRACE(isOk);
}

void EditionForEachLoop::onNbBranchesEdited()
{
  int newval = _formEachLoop->sb_nbranch->value();
  DEBTRACE("EditionForEachLoop::onNbBranchesEdited " << _nbBranches << " --> " << newval);
  if (newval != _nbBranches)
    {
      SubjectForEachLoop *sfe = dynamic_cast<SubjectForEachLoop*>(_subject);
      YASSERT(sfe);
      QString text = _formEachLoop->sb_nbranch->cleanText();
      sfe->setNbBranches(text.toStdString());
    }
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
      _nbBranches = i; 

      //smplscollection
      InputPort* dp=_subjectNode->getNode()->getInputPort("SmplsCollection");
      _formEachLoop->lineEdit_2->setText(dp->getAsString().c_str());
      break;
    }
}
