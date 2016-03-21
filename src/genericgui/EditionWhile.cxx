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

#include "EditionWhile.hxx"
#include "FormLoop.hxx"
#include "guiObservers.hxx"
#include "QtGuiContext.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <cassert>
#include <sstream>

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

EditionWhile::EditionWhile(Subject* subject,
                         QWidget* parent,
                         const char* name)
  : EditionNode(subject, parent, name)
{
  _formWhile = new FormLoop(this);
  _condition = 0;
  _wid->gridLayout1->addWidget(_formWhile);
  _formWhile->sb_nsteps->setMinimum(0);
  _formWhile->sb_nsteps->setMaximum(1);
  _formWhile->label->setText("Condition");
  if (!QtGuiContext::getQtCurrent()->isEdition())
    _formWhile->setEnabled (false);
  connect(_formWhile->sb_nsteps, SIGNAL(editingFinished()),
          this, SLOT(onConditionEdited()));
}

EditionWhile::~EditionWhile()
{
}

void EditionWhile::onConditionEdited()
{
  int newval = _formWhile->sb_nsteps->value();
  DEBTRACE("EditionWhile::onConditionEdited " << _condition << " --> " << newval);
  if (newval != _condition)
    {
      SubjectWhileLoop *swl = dynamic_cast<SubjectWhileLoop*>(_subject);
      YASSERT(swl);
      QString text = _formWhile->sb_nsteps->cleanText();
      swl->setCondition(text.toStdString());
    }
}

void EditionWhile::synchronize()
{
  _subject->update(SETVALUE, 0, _subject);
}

void EditionWhile::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("EditionWhile::update " << eventName(event) << " " << type);
  EditionNode::update(event, type, son);
  switch (event)
    {
    case SETVALUE:
      SubjectComposedNode * scn = dynamic_cast<SubjectComposedNode*>(_subject);
      string val = scn->getValue();
      if (val == "True") val = "1";
      else if (val == "False") val = "0";
      DEBTRACE(val);
      istringstream ss(val);
      bool i = 0;
      ss >> i;
      DEBTRACE(i);
      _formWhile->sb_nsteps->setValue(i);
      _condition = i;
      break;
    }
}
