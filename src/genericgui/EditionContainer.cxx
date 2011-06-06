// Copyright (C) 2006-2011  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include "FormContainer.hxx"
#include "QtGuiContext.hxx"
#include "guiObservers.hxx"
#include "Proc.hxx"
#include "Message.hxx"

#include <cassert>

using namespace std;

using namespace YACS;
using namespace YACS::HMI;
using namespace YACS::ENGINE;

EditionContainer::EditionContainer(Subject* subject,
                                   QWidget* parent,
                                   const char* name)
  : ItemEdition(subject, parent, name)
{
  _wContainer = new FormContainer(this);
  _wid->gridLayout1->addWidget(_wContainer);
  connect(_wContainer->cb_resource, SIGNAL(mousePressed()),
          this, SLOT(fillContainerPanel()));
  connect(_wContainer->tb_container, SIGNAL(toggled(bool)),
          this, SLOT(fillContainerPanel())); // --- to update display of current selection
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
      _wContainer->le_name->setText((son->getName()).c_str());
      fillContainerPanel();
      break;
    default:
      ;
    }
}

void EditionContainer::fillContainerPanel()
{
  DEBTRACE("EditionContainer::fillContainerPanel");
  SubjectContainer *scont = dynamic_cast<SubjectContainer*>(_subject);
  YASSERT(scont);
  _wContainer->FillPanel(scont->getContainer());
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
  SubjectContainer *scont = dynamic_cast<SubjectContainer*>(_subject);
  YASSERT(scont);
  _wContainer->FillPanel(scont->getContainer());
  ItemEdition::onCancel();
}
