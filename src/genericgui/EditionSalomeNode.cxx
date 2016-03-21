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

#include "EditionSalomeNode.hxx"
#include "PropertyEditor.hxx"
#include "FormComponent.hxx"
#include "FormContainer.hxx"
#include "ServiceNode.hxx"
#include "ComponentInstance.hxx"
#include "QtGuiContext.hxx"
#include "Container.hxx"
#include "Message.hxx"

#include <cassert>
#include <map>
#include <string>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

EditionSalomeNode::EditionSalomeNode(Subject* subject,
                                     QWidget* parent,
                                     const char* name)
  : EditionElementaryNode(subject, parent, name)
{
  //   SubjectServiceNode *ssn = dynamic_cast<SubjectServiceNode*>(_subject);
  //   YASSERT(ssn);
  _servNode = dynamic_cast<YACS::ENGINE::ServiceNode*>(_subElemNode->getNode());
  YASSERT(_servNode);
  // --- create property editor panel
  _propeditor=new PropertyEditor(_subject);
  _wid->gridLayout1->addWidget(_propeditor);

  // --- create container and component panels

  _wContainer = new FormContainerDecorator(_servNode->getContainer(),this);
  _wid->gridLayout1->addWidget(_wContainer);

  connect(_wContainer, SIGNAL(resourceMousePressed()), this, SLOT(fillContainerPanel()));
  connect(_wContainer, SIGNAL(resourceActivated(int)), this, SLOT(changeHost(int)));
  connect(_wContainer, SIGNAL(containerToggled(bool)), this, SLOT(fillContainerPanel()));
  // --- to update display of current selection

  _wComponent = new FormComponent(this);
  _wid->gridLayout1->addWidget(_wComponent);

  connect(_wComponent->cb_instance, SIGNAL(mousePressed()),
          this, SLOT(fillComponentPanel()));
  connect(_wComponent->cb_instance, SIGNAL(activated(int)),
          this, SLOT(changeInstance(int)));

  connect(_wComponent->cb_container, SIGNAL(mousePressed()),
          this, SLOT(fillContainerPanel()));
  connect(_wComponent->cb_container, SIGNAL(activated(int)),
          this, SLOT(changeContainer(int)));

  connect(_wComponent->tb_component, SIGNAL(toggled(bool)),
          this, SLOT(fillComponentPanel())); // --- to update display of current selection
  connect(_wComponent->tb_component, SIGNAL(toggled(bool)),
          this, SLOT(fillContainerPanel())); // --- to update display of current selection

  // --- method

  _hbl_method = new QHBoxLayout();
  _la_method = new QLabel(this);
  _hbl_method->addWidget(_la_method);
  _le_method = new QLineEdit(this);
  _hbl_method->addWidget(_le_method);
  _wid->gridLayout1->addLayout(_hbl_method, _wid->gridLayout1->rowCount(), 0, 1, 1);
  _la_method->setText("Method:");

  _le_method->setText((_servNode->getMethod()).c_str());
  _le_method->setReadOnly(true);

  // --- fill component panel

  fillComponentPanel();

  createTablePorts(_wid->gridLayout1);
}

EditionSalomeNode::~EditionSalomeNode()
{
}

void EditionSalomeNode::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("EditionSalomeNode::update ");
  EditionElementaryNode::update(event, type, son);
  SubjectReference* subref= 0;
  switch (event)
    {
    case ADDREF:
      DEBTRACE("ADDREF");
      subref = dynamic_cast<SubjectReference*>(son);
      YASSERT(subref);
      DEBTRACE(subref->getName() << " " << subref->getReference()->getName());
      fillComponentPanel();
      fillContainerPanel();
      break;

    case ASSOCIATE:
      DEBTRACE("ASSOCIATE");       
      fillContainerPanel();
      break;

    case SETVALUE:
      _propeditor->update();
      break;

    default:
      ;
    } 
}

void EditionSalomeNode::synchronize()
{
  EditionElementaryNode::synchronize();
  _wContainer->synchronizeCheckContainer();
  _wComponent->tb_component->setChecked(FormComponent::_checked);
  fillComponentPanel();
  fillContainerPanel();
}

/*! must be updated when associate service to component instance, or when the list of
 *  available component instances changes.
 */
void EditionSalomeNode::fillComponentPanel()
{
  ComponentInstance *compoInst = _servNode->getComponent();
  if (compoInst)
    {
      _wComponent->le_name ->setText(compoInst->getCompoName().c_str());
      Proc* proc = GuiContext::getCurrent()->getProc();

      _wComponent->cb_instance->clear();
      map<string,ComponentInstance*>::const_iterator it = proc->componentInstanceMap.begin();
      for(; it != proc->componentInstanceMap.end(); ++it)
        {
          ComponentInstance *inst=(*it).second;
          QString compoName = inst->getCompoName().c_str();
          if (! QString::compare(compoInst->getCompoName().c_str(), compoName)) // if same component name
            //instanceMap[inst->getInstanceName()] = inst;
            _wComponent->cb_instance->addItem( QString(inst->getInstanceName().c_str()));
        }

      int index = _wComponent->cb_instance->findText(compoInst->getInstanceName().c_str());
      //DEBTRACE("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ " << index);
      _wComponent->cb_instance->setCurrentIndex(index);  
    }
}

void EditionSalomeNode::fillContainerPanel()
{
  DEBTRACE("EditionSalomeNode::fillContainerPanel");
  ComponentInstance *compoInst = _servNode->getComponent();
  if (compoInst)
    {
      Proc* proc = GuiContext::getCurrent()->getProc();

      _wComponent->cb_container->clear();
      map<string,Container*>::const_iterator it = proc->containerMap.begin();
      for(; it != proc->containerMap.end(); ++it)
        _wComponent->cb_container->addItem( QString((*it).first.c_str()));

      Container * cont = compoInst->getContainer();
      if (cont)
        {
          int index = _wComponent->cb_container->findText(cont->getName().c_str());
          _wComponent->cb_container->setCurrentIndex(index);
          _wContainer->FillPanel(cont);
        }
    }
}


void EditionSalomeNode::changeInstance(int index)
{
  string instName = _wComponent->cb_instance->itemText(index).toStdString();
  Proc* proc = GuiContext::getCurrent()->getProc();
  ComponentInstance *newCompoInst = 0;
  ComponentInstance *oldCompoInst = _servNode->getComponent();
  if (proc->componentInstanceMap.count(instName))
    newCompoInst = proc->componentInstanceMap[instName];
  else DEBTRACE("-------------> not found : " << instName);

  if (newCompoInst && (newCompoInst != oldCompoInst))
    {
      YASSERT(GuiContext::getCurrent()->_mapOfSubjectComponent.count(newCompoInst));
      SubjectServiceNode *ssn = dynamic_cast<SubjectServiceNode*>(_subject);
      if(!ssn->associateToComponent(GuiContext::getCurrent()->_mapOfSubjectComponent[newCompoInst]))
        Message mess;
    }
}

void EditionSalomeNode::changeContainer(int index)
{
  string contName = _wComponent->cb_container->itemText(index).toStdString();
  DEBTRACE(contName);
  ComponentInstance *compoInst = _servNode->getComponent();
  Container *oldContainer = compoInst->getContainer();

  Container *newContainer = 0;
  Proc* proc = GuiContext::getCurrent()->getProc();
  if (proc->containerMap.count(contName))
    newContainer = proc->containerMap[contName];
  if (!newContainer)
    {
      DEBTRACE("-------------> not found : " << contName);
      return;
    }
  YASSERT(GuiContext::getCurrent()->_mapOfSubjectContainer.count(newContainer));
  SubjectContainerBase *scnt(GuiContext::getCurrent()->_mapOfSubjectContainer[newContainer]);

  SubjectServiceNode *ssn(dynamic_cast<SubjectServiceNode*>(_subject));
  SubjectComponent *sco(dynamic_cast<SubjectComponent*>(ssn->getSubjectReference()->getReference()));
  YASSERT(sco);
  sco->associateToContainer(scnt);
}

void EditionSalomeNode::changeHost(int index)
{
  string hostName = _wContainer->getHostName(index);
  DEBTRACE(hostName);
}

void EditionSalomeNode::onApply()
{
  DEBTRACE("EditionSalomeNode::onApply");
  bool edited = true;
  if (_wContainer->onApply())
    edited = false;
  _isEdited = _isEdited || edited;
  EditionElementaryNode::onApply();
}

void EditionSalomeNode::onCancel()
{
  DEBTRACE("EditionSalomeNode::onCancel");
  ComponentInstance *compoInst = _servNode->getComponent();
  if (compoInst)
    {
      _wContainer->FillPanel(compoInst->getContainer());
    }
  EditionElementaryNode::onApply();
}
