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

#include "EditionComponent.hxx"
#include "PropertyEditor.hxx"
#include "QtGuiContext.hxx"
#include "Container.hxx"
#include "ComponentInstance.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;
using namespace YACS::ENGINE;


EditionComponent::EditionComponent(Subject* subject,
                                   QWidget* parent,
                                   const char* name)
  : ItemEdition(subject, parent, name)
{

  QHBoxLayout* hboxLayout = new QHBoxLayout();
  hboxLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
  QLabel* la_container = new QLabel("Container:");
  hboxLayout->addWidget(la_container);
  _cb_container = new ComboBox();
  hboxLayout->addWidget(_cb_container);
  _wid->gridLayout1->addLayout(hboxLayout, 3, 0, 1, 1);
  _propeditor=new PropertyEditor(_subject);
  _wid->gridLayout1->addWidget(_propeditor);

  connect(_cb_container, SIGNAL(mousePressed()),
          this, SLOT(fillContainerPanel()));
  connect(_cb_container, SIGNAL(activated(int)),
          this, SLOT(changeContainer(int)));

  fillContainerPanel();
}

EditionComponent::~EditionComponent()
{
}

void EditionComponent::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("EditionComponent::update " <<eventName(event) << " " << type);
  if(event == SETVALUE)
    _propeditor->update();

  if(event == ADDREF || event == ASSOCIATE)
    fillContainerPanel();
}

void EditionComponent::fillContainerPanel()
{
  Proc* proc = GuiContext::getCurrent()->getProc();
  _cb_container->clear();
  std::map<string,Container*>::const_iterator it = proc->containerMap.begin();
  for(; it != proc->containerMap.end(); ++it)
    _cb_container->addItem( QString((*it).first.c_str()));

  SubjectComponent *scompo=dynamic_cast<SubjectComponent*>(_subject);
  ComponentInstance *compoInst = scompo->getComponent();
  Container * cont = compoInst->getContainer();
  if (cont)
    {
      int index = _cb_container->findText(cont->getName().c_str());
      _cb_container->setCurrentIndex(index);
    }
}

void EditionComponent::changeContainer(int index)
{
  string contName = _cb_container->itemText(index).toStdString();
  DEBTRACE(contName);
  SubjectComponent *scompo=dynamic_cast<SubjectComponent*>(_subject);
  ComponentInstance *compoInst = scompo->getComponent();

  Proc* proc = GuiContext::getCurrent()->getProc();

  if (proc->containerMap.count(contName)==0)
    {
      DEBTRACE("-------------> not found : " << contName);
      return;
    }
  Container *newContainer =proc->containerMap[contName];
  SubjectContainerBase *scnt = GuiContext::getCurrent()->_mapOfSubjectContainer[newContainer];
  scompo->associateToContainer(scnt);
  scompo->select(true);
}

