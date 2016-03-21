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

#include "EditionContainer.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include "FormContainerDecorator.hxx"
#include "QtGuiContext.hxx"
#include "guiObservers.hxx"
#include "Proc.hxx"
#include "Message.hxx"

#include <cassert>

using namespace std;

using namespace YACS;
using namespace YACS::HMI;
using namespace YACS::ENGINE;

EditionContainer::EditionContainer(Subject* subject, QWidget* parent, const char* name):ItemEdition(subject, parent, name)
{
  _wContainer = new FormContainerDecorator(getContainer(),this);
  _wid->gridLayout1->addWidget(_wContainer);
  connect(_wContainer, SIGNAL(resourceMousePressed()), this, SLOT(fillContainerPanel()));
  connect(_wContainer->tb_container, SIGNAL(toggled(bool)), this, SLOT(fillContainerPanel())); // --- to update display of current selection
  _wContainer->tb_container->toggle();
}

EditionContainer::~EditionContainer()
{
}

void EditionContainer::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("EditionContainer::update");
  ItemEdition::update(event, type, son);
  switch (event)
    {
    case RENAME:
    case UPDATE:
      _wContainer->setName(son->getName());
      fillContainerPanel();
      break;
    default:
      ;
    }
}

void EditionContainer::fillContainerPanel()
{
  DEBTRACE("EditionContainer::fillContainerPanel");
  _wContainer->FillPanel(getContainer());
}

void EditionContainer::onApply()
{
  DEBTRACE("EditionContainer::onApply");
  bool edited = true;
  if (!_wContainer->onApply())
    {
      _isEdited = true;
      Message mess(GuiContext::getCurrent()->_lastErrorMessage);
      return;
    }
  edited = false;
  _isEdited = _isEdited || edited;
  ItemEdition::onApply();
}

void EditionContainer::onCancel()
{
  DEBTRACE("EditionContainer::onCancel");
  _wContainer->onCancel();
  ItemEdition::onCancel();
}

YACS::ENGINE::Container *EditionContainer::getContainer()
{
  SubjectContainerBase *scont(dynamic_cast<SubjectContainerBase*>(_subject));
  YASSERT(scont);
  return scont->getContainer();
}
