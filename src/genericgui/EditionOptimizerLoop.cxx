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
#include "EditionOptimizerLoop.hxx"
#include "FormEachLoop.hxx"
#include "Node.hxx"
#include "OptimizerLoop.hxx"
#include "TypeCode.hxx"
#include "QtGuiContext.hxx"
#include "Message.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <cassert>
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
  _formEachLoop = new FormEachLoop(this);
  _nbBranches = 1;
  _wid->gridLayout1->addWidget(_formEachLoop);
  _formEachLoop->sb_nbranch->setMinimum(1);
  _formEachLoop->sb_nbranch->setMaximum(INT_MAX);
  Node* node=_subjectNode->getNode();
  OptimizerLoop *ol = dynamic_cast<OptimizerLoop*>(node);
  if(ol)
    _formEachLoop->lineEdit->setText(ol->edGetSamplePort()->edGetType()->name());
  connect(_formEachLoop->sb_nbranch, SIGNAL(editingFinished()),
          this, SLOT(onNbBranchesEdited()));

  _formEachLoop->label_3->setText("FileNameInitAlg");
  connect(_formEachLoop->lineEdit_2, SIGNAL(editingFinished()),this,SLOT(onModifyInitFile()));

  QLabel* la_lib = new QLabel(this);
  la_lib->setText("lib");
  _formEachLoop->gridLayout->addWidget(la_lib, 3, 0);
  _le_lib = new QLineEdit(this);
  if(ol)
    _le_lib->setText(QString::fromStdString(ol->getAlgLib()));
  _formEachLoop->gridLayout->addWidget(_le_lib, 3, 2);

  QLabel* la_entry = new QLabel(this);
  la_entry->setText("entry");
  _formEachLoop->gridLayout->addWidget(la_entry, 4, 0);
  _le_entry = new QLineEdit(this);
  if(ol)
    _le_entry->setText(QString::fromStdString(ol->getSymbol()));
  _formEachLoop->gridLayout->addWidget(_le_entry, 4, 2);
  connect(_le_entry, SIGNAL(editingFinished()),this,SLOT(onModifyEntry()));
  connect(_le_lib, SIGNAL(editingFinished()),this,SLOT(onModifyLib()));
}

EditionOptimizerLoop::~EditionOptimizerLoop()
{
}

void EditionOptimizerLoop::onModifyInitFile()
{
  bool isOk = false;
  Node* node=_subjectNode->getNode();
  OptimizerLoop *ol = dynamic_cast<OptimizerLoop*>(node);
  InputPort* dp=ol->getInputPort("FileNameInitAlg");
  SubjectDataPort* sdp = QtGuiContext::getQtCurrent()->_mapOfSubjectDataPort[dp];
  isOk=sdp->setValue(_formEachLoop->lineEdit_2->text().toStdString());
  DEBTRACE(isOk);
}
void EditionOptimizerLoop::onModifyEntry()
{
  DEBTRACE("EditionOptimizerLoop::onModifyEntry");
  OptimizerLoop *oloop = dynamic_cast<OptimizerLoop*>(_subjectNode->getNode());
  if(oloop->getSymbol() == _le_entry->text().toStdString()) return;

  bool isOk = false;
  SubjectOptimizerLoop *ol = dynamic_cast<SubjectOptimizerLoop*>(_subjectNode);
  isOk=ol->setAlgorithm(_le_lib->text().toStdString(),_le_entry->text().toStdString());
  if(!isOk)
    Message mess;
  DEBTRACE(isOk);
}
void EditionOptimizerLoop::onModifyLib()
{
  DEBTRACE("EditionOptimizerLoop::onModifyLib");
  OptimizerLoop *oloop = dynamic_cast<OptimizerLoop*>(_subjectNode->getNode());
  if(oloop->getAlgLib() == _le_lib->text().toStdString()) return;

  bool isOk = false;
  SubjectOptimizerLoop *ol = dynamic_cast<SubjectOptimizerLoop*>(_subjectNode);
  isOk=ol->setAlgorithm(_le_lib->text().toStdString(),_le_entry->text().toStdString());
  if(!isOk)
    Message mess;
  DEBTRACE(isOk);
}

void EditionOptimizerLoop::onNbBranchesEdited()
{
  int newval = _formEachLoop->sb_nbranch->value();
  DEBTRACE("EditionOptimizerLoop::onNbBranchesEdited " << _nbBranches << " --> " << newval);
  if (newval != _nbBranches)
    {
      SubjectOptimizerLoop *sol = dynamic_cast<SubjectOptimizerLoop*>(_subject);
      YASSERT(sol);
      QString text = _formEachLoop->sb_nbranch->cleanText();
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
      SubjectComposedNode * scn = dynamic_cast<SubjectComposedNode*>(_subject);
      string val = scn->getValue();
      istringstream ss(val);
      DEBTRACE( val);
      int i = 0;
      ss >> i;
      DEBTRACE(i);
      _formEachLoop->sb_nbranch->setValue(i);
      _nbBranches = i;

      //init file
      InputPort* dp=_subjectNode->getNode()->getInputPort("FileNameInitAlg");
      _formEachLoop->lineEdit_2->setText(dp->getAsString().c_str());
      //algo lib
      OptimizerLoop *ol = dynamic_cast<OptimizerLoop*>(_subjectNode->getNode());
      _le_entry->setText(QString::fromStdString(ol->getSymbol()));
      _le_lib->setText(QString::fromStdString(ol->getAlgLib()));
      //input type name
      _formEachLoop->lineEdit->setText(ol->edGetSamplePort()->edGetType()->name());
      break;
    }
}
