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

#include "EditionOptimizerLoop.hxx"
#include "FormOptimizerLoop.hxx"
#include "Node.hxx"
#include "OptimizerLoop.hxx"
#include "TypeCode.hxx"
#include "QtGuiContext.hxx"
#include "Message.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <sstream>

using namespace std;

using namespace YACS;
using namespace YACS::HMI;
using namespace YACS::ENGINE;

EditionOptimizerLoop::EditionOptimizerLoop(Subject* subject,
                                       QWidget* parent,
                                       const char* name)
  : EditionNode(subject, parent, name)
{
  _formOptimizerLoop = new FormOptimizerLoop(this);
  _nbBranches = 1;
  _wid->gridLayout1->addWidget(_formOptimizerLoop);
  _formOptimizerLoop->sb_nbranch->setMinimum(1);
  _formOptimizerLoop->sb_nbranch->setMaximum(INT_MAX);

  connect(_formOptimizerLoop->sb_nbranch, SIGNAL(editingFinished()),
          this, SLOT(onNbBranchesEdited()));
  connect(_formOptimizerLoop->lineEdit_initValue, SIGNAL(editingFinished()),
          this, SLOT(onModifyInitFile()));
  connect(_formOptimizerLoop->lineEdit_entry, SIGNAL(editingFinished()),
          this, SLOT(onModifyEntry()));
  connect(_formOptimizerLoop->lineEdit_lib, SIGNAL(editingFinished()),
          this, SLOT(onModifyLib()));
}

EditionOptimizerLoop::~EditionOptimizerLoop()
{
}

void EditionOptimizerLoop::onModifyInitFile()
{
  bool isOk = false;
  Node* node=_subjectNode->getNode();
  OptimizerLoop *ol = dynamic_cast<OptimizerLoop*>(node);
  InputPort * dp = ol->edGetAlgoInitPort();
  SubjectDataPort* sdp = QtGuiContext::getQtCurrent()->_mapOfSubjectDataPort[dp];
  isOk=sdp->setValue(_formOptimizerLoop->lineEdit_initValue->text().toStdString());
  DEBTRACE(isOk);
}

void EditionOptimizerLoop::onModifyEntry()
{
  DEBTRACE("EditionOptimizerLoop::onModifyEntry");
  OptimizerLoop *oloop = dynamic_cast<OptimizerLoop*>(_subjectNode->getNode());
  if (oloop->getSymbol() == _formOptimizerLoop->lineEdit_entry->text().toStdString()) return;

  bool isOk = false;
  SubjectOptimizerLoop *ol = dynamic_cast<SubjectOptimizerLoop*>(_subjectNode);
  isOk=ol->setAlgorithm(_formOptimizerLoop->lineEdit_lib->text().toStdString(),
                        _formOptimizerLoop->lineEdit_entry->text().toStdString());
  if(!isOk)
    Message mess;
  DEBTRACE(isOk);
}

void EditionOptimizerLoop::onModifyLib()
{
  DEBTRACE("EditionOptimizerLoop::onModifyLib");
  OptimizerLoop *oloop = dynamic_cast<OptimizerLoop*>(_subjectNode->getNode());
  if(oloop->getAlgLib() == _formOptimizerLoop->lineEdit_lib->text().toStdString()) return;

  bool isOk = false;
  SubjectOptimizerLoop *ol = dynamic_cast<SubjectOptimizerLoop*>(_subjectNode);
  isOk=ol->setAlgorithm(_formOptimizerLoop->lineEdit_lib->text().toStdString(),
                        _formOptimizerLoop->lineEdit_entry->text().toStdString());
  if (!isOk && !_formOptimizerLoop->lineEdit_lib->text().isEmpty() &&
      !_formOptimizerLoop->lineEdit_entry->text().isEmpty())
    Message mess;
  DEBTRACE(isOk);
}

void EditionOptimizerLoop::onNbBranchesEdited()
{
  int newval = _formOptimizerLoop->sb_nbranch->value();
  DEBTRACE("EditionOptimizerLoop::onNbBranchesEdited " << _nbBranches << " --> " << newval);
  if (newval != _nbBranches)
    {
      SubjectOptimizerLoop *sol = dynamic_cast<SubjectOptimizerLoop*>(_subject);
      YASSERT(sol);
      QString text = _formOptimizerLoop->sb_nbranch->cleanText();
      sol->setNbBranches(text.toStdString());
    }
}

void EditionOptimizerLoop::synchronize()
{
  _subject->update(SETVALUE, 0, _subject);
}

void EditionOptimizerLoop::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("EditionOptimizerLoop::update " << eventName(event) << " " << type);
  EditionNode::update(event, type, son);
  switch (event)
    {
    case SETVALUE:
      // Nb branches
      SubjectComposedNode * scn = dynamic_cast<SubjectComposedNode*>(_subject);
      string val = scn->getValue();
      istringstream ss(val);
      DEBTRACE( val);
      int i = 0;
      ss >> i;
      DEBTRACE(i);
      _formOptimizerLoop->sb_nbranch->setValue(i);
      _nbBranches = i;

      OptimizerLoop * ol = dynamic_cast<OptimizerLoop *>(_subjectNode->getNode());

      // Algo init value
      InputPort * dp = ol->edGetAlgoInitPort();
      _formOptimizerLoop->lineEdit_initValue->setText(dp->getAsString().c_str());

      // Algo library and entry
      _formOptimizerLoop->lineEdit_entry->setText(QString::fromStdString(ol->getSymbol()));
      _formOptimizerLoop->lineEdit_lib->setText(QString::fromStdString(ol->getAlgLib()));

      // Type names
      _formOptimizerLoop->label_evalSampleType->setText(ol->edGetSamplePort()->edGetType()->name());
      _formOptimizerLoop->label_evalResultType->setText(ol->edGetPortForOutPool()->edGetType()->name());
      _formOptimizerLoop->label_algoInitType->setText(ol->edGetAlgoInitPort()->edGetType()->name());
      _formOptimizerLoop->label_algoResultType->setText(ol->edGetAlgoResultPort()->edGetType()->name());
      break;
    }
}
