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
#include "EditionContainer.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include "FormContainer.hxx"
#include "QtGuiContext.hxx"
#include "Proc.hxx"

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
  connect(_wContainer->cb_host, SIGNAL(mousePressed()),
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

  _wContainer->le_name->setText(_subject->getName().c_str());
  _wContainer->le_instance->setReadOnly(true);

  _wContainer->cb_host->clear();
  _wContainer->cb_host->addItem(""); // --- when no host selected

  list<string> machines = QtGuiContext::getQtCurrent()->getGMain()->getMachineList();
  list<string>::iterator itm = machines.begin();
  for( ; itm != machines.end(); ++itm)
    {
      _wContainer->cb_host->addItem(QString((*itm).c_str()));
    }
}
