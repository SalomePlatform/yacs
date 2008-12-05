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
#include "EditionSalomeNode.hxx"
#include "FormComponent.hxx"
#include "FormContainer.hxx"
#include "ServiceNode.hxx"
#include "ComponentInstance.hxx"
#include "QtGuiContext.hxx"
#include "Container.hxx"

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

  // --- create container and component panels

  _wContainer = new FormContainer(this);
  _wid->gridLayout1->addWidget(_wContainer);

  connect(_wContainer->cb_host, SIGNAL(mousePressed()),
          this, SLOT(fillContainerPanel()));
  connect(_wContainer->cb_host, SIGNAL(activated(int)),
          this, SLOT(changeHost(int)));
  connect(_wContainer->tb_container, SIGNAL(toggled(bool)),
          this, SLOT(fillContainerPanel())); // --- to update display of current selection

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
//   SubjectServiceNode *ssn = dynamic_cast<SubjectServiceNode*>(_subject);
//   assert(ssn);
  _servNode =
    dynamic_cast<YACS::ENGINE::ServiceNode*>(_subElemNode->getNode());
  assert(_servNode);
  _le_method->setText((_servNode->getMethod()).c_str());
  _le_method->setReadOnly(true);

  // --- fill component panel

  fillComponentPanel();

  createTablePorts();
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
      assert(subref);
      DEBTRACE(subref->getName() << " " << subref->getReference()->getName());
      fillComponentPanel();
      break;

    case ASSOCIATE:
      DEBTRACE("ASSOCIATE");       
      fillContainerPanel();
      break;

    default:
      ;
    } 
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
      map<pair<string,int>,ComponentInstance*>::const_iterator it = proc->componentInstanceMap.begin();
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
  ComponentInstance *compoInst = _servNode->getComponent();
  if (compoInst)
    {
      Proc* proc = GuiContext::getCurrent()->getProc();

      _wComponent->cb_container->clear();
      map<string,Container*>::const_iterator it = proc->containerMap.begin();
      for(; it != proc->containerMap.end(); ++it)
        _wComponent->cb_container->addItem( QString((*it).first.c_str()));

      int index = _wComponent->cb_container->findText(compoInst->getContainer()->getName().c_str());
      _wComponent->cb_container->setCurrentIndex(index);  

      _wContainer->le_name->setText(compoInst->getContainer()->getName().c_str());
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
}


void EditionSalomeNode::changeInstance(int index)
{
  string instName = _wComponent->cb_instance->itemText(index).toStdString();
  int i = instName.find_last_of('_');
  if (i<0) return;

  DEBTRACE(instName << " "  << i);
  string compoName = instName;
  compoName.erase(i);
  string inst = instName;
  inst.erase(0,i+1);
  DEBTRACE(instName << " " << compoName << " " << inst);
  i = atoi(inst.c_str());
  pair<string,int> aKey(compoName,i);

  Proc* proc = GuiContext::getCurrent()->getProc();
  ComponentInstance *newCompoInst = 0;
  ComponentInstance *oldCompoInst = _servNode->getComponent();
  if (proc->componentInstanceMap.count(aKey))
    newCompoInst = proc->componentInstanceMap[aKey];
  else DEBTRACE("-------------> not found : " << instName);

  if (newCompoInst && (newCompoInst != oldCompoInst))
    {
      assert(GuiContext::getCurrent()->_mapOfSubjectComponent.count(newCompoInst));
      SubjectServiceNode *ssn = dynamic_cast<SubjectServiceNode*>(_subject);
      ssn->associateToComponent(GuiContext::getCurrent()->_mapOfSubjectComponent[newCompoInst]);
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
  assert(GuiContext::getCurrent()->_mapOfSubjectContainer.count(newContainer));
  SubjectContainer *scnt = GuiContext::getCurrent()->_mapOfSubjectContainer[newContainer];

  SubjectServiceNode *ssn = dynamic_cast<SubjectServiceNode*>(_subject);
  SubjectComponent *sco =
    dynamic_cast<SubjectComponent*>(ssn->getSubjectReference()->getReference());
  assert (sco);
  sco->associateToContainer(scnt);
}

void EditionSalomeNode::changeHost(int index)
{
  string hostName = _wContainer->cb_host->itemText(index).toStdString();
  DEBTRACE(hostName);
}
