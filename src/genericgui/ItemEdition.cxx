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

#include "ItemEdition.hxx"
#include "EditionProc.hxx"
#include "EditionBloc.hxx"
#include "EditionComponent.hxx"
#include "EditionContainer.hxx"
#include "EditionControlLink.hxx"
#include "EditionDataLink.hxx"
#include "EditionDataType.hxx"
#include "EditionElementaryNode.hxx"
#include "EditionForEachLoop.hxx"
#include "EditionOptimizerLoop.hxx"
#include "EditionInputPort.hxx"
#include "EditionLoop.hxx"
#include "EditionWhile.hxx"
#include "EditionOutNode.hxx"
#include "EditionOutputPort.hxx"
#include "EditionPresetNode.hxx"
#include "EditionPyFunc.hxx"
#include "EditionSalomeNode.hxx"
#include "EditionScript.hxx"
#include "EditionStudyInNode.hxx"
#include "EditionStudyOutNode.hxx"
#include "EditionSwitch.hxx"

#include "QtGuiContext.hxx"
#include "TypeCode.hxx"
#include "commandsProc.hxx"

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QLayout>


//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <string>
#include <cassert>

using namespace std;

using namespace YACS;
using namespace YACS::HMI;


/*! get name and strings for type of item and category
 */
ItemEditionBase::ItemEditionBase(Subject* subject)
  : GuiObserver(), _subject(subject)
{
  _subject->attach(this);
  _stackId = -1;
  _name = _subject->getName();
  _type = "Unknown";
  _category = "Unknown";
  ProcInvoc* invoc = QtGuiContext::getQtCurrent()->getInvoc();
  if (SubjectNode * sub = dynamic_cast<SubjectNode*>(_subject))
    {
      _category = "Node";
      _type = invoc->getTypeName(invoc->getTypeOfNode(sub->getNode()));
    }
  else if (SubjectDataPort * sub = dynamic_cast<SubjectDataPort*>(_subject))
    {
      _category = "Port";
      _type = invoc->getTypeName(invoc->getTypeOfPort(sub->getPort()));
    }
  else if (SubjectLink * sub = dynamic_cast<SubjectLink*>(_subject))
    {
      _category = "Link";
      _type = "Link";
    }
  else if (SubjectControlLink * sub = dynamic_cast<SubjectControlLink*>(_subject))
    {
      _category = "ControlLink";
      _type = "Control Link";
    }
  else if (SubjectDataType * sub = dynamic_cast<SubjectDataType*>(_subject))
    {
      _category = "Data";
      switch (sub->getTypeCode()->kind())
        {
        case YACS::ENGINE::Double:   _type = "Double"; break;
        case YACS::ENGINE::Int:      _type = "Int"; break;
        case YACS::ENGINE::String:   _type = "String"; break;
        case YACS::ENGINE::Bool:     _type = "Bool"; break;
        case YACS::ENGINE::Objref:   _type = "Objref"; break;
        case YACS::ENGINE::Sequence: _type = "Sequence"; break;
        case YACS::ENGINE::Array:    _type = "Array"; break;
        case YACS::ENGINE::Struct:   _type = "Struct"; break;
        default: _type = "None or Unknown";
        }
    }
  else if (SubjectComponent * sub = dynamic_cast<SubjectComponent*>(_subject))
    {
      _category = "Component";
      _type = "Salome Component";
    }
  else if (SubjectContainerBase * sub = dynamic_cast<SubjectContainerBase*>(_subject))
    {
      _category = "Container";
      _type = sub->getLabelForHuman();
    }
}

ItemEditionBase::~ItemEditionBase()
{
}

void ItemEditionBase::select(bool isSelected)
{
  DEBTRACE("ItemEditionBase::select " << isSelected);
}

void ItemEditionBase::synchronize()
{
}

void ItemEditionBase::update(GuiEvent event, int type, Subject* son)
{
}

Subject* ItemEditionBase::getSubject()
{
  return _subject;
}

/*! generic item edition based on a FormEditItem widget to be completed 
 *  in derived classes. Apply - cancel edition buttons are not always
 *  relevants.
 */
ItemEdition::ItemEdition(Subject* subject,
                         QWidget* parent,
                         const char* name)
  : FormEditItem(parent), ItemEditionBase(subject)
{
  DEBTRACE("ItemEdition::ItemEdition " << name);

  _haveScript = false;
  _isEdited = false;
  setEdited(false);

  connect(_wid->pb_apply, SIGNAL(clicked()), this, SLOT(onApply()));
  connect(_wid->pb_cancel, SIGNAL(clicked()), this, SLOT(onCancel()));

  // --- type of item and name

  _wid->la_type->setText(_type.c_str());
  _wid->le_name->setText(_name.c_str());
  connect(_wid->le_name, SIGNAL(textChanged(const QString&)),
          this, SLOT(onModifyName(const QString&)));
  _wid->le_name->setReadOnly(true); // to be changed on derived classes depending on context

  _stackId = QtGuiContext::getQtCurrent()->getStackedWidget()->addWidget(this);
  DEBTRACE("_stackId " << _stackId);
  YASSERT(!QtGuiContext::getQtCurrent()->_mapOfEditionItem.count(_subject));
  QtGuiContext::getQtCurrent()->_mapOfEditionItem[_subject] = this;
  //QtGuiContext::getQtCurrent()->getStackedWidget()->raiseWidget(_stackId);
}

ItemEdition::~ItemEdition()
{
  DEBTRACE("ItemEdition::~ItemEdition " << _name);
  if(QtGuiContext::getQtCurrent())
    {
      DEBTRACE("---");
      QtGuiContext::getQtCurrent()->getStackedWidget()->removeWidget(this);
      QtGuiContext::getQtCurrent()->_mapOfEditionItem.erase(_subject);
      DEBTRACE("---");
    }
}

/*! used in derived nodes for synchronisation with schema model.
 */
void ItemEdition::synchronize()
{
}

void ItemEdition::select(bool isSelected)
{
  DEBTRACE("ItemEdition::select " << isSelected);
  if (isSelected)
    {
      synchronize();
      QtGuiContext::getQtCurrent()->getStackedWidget()->setCurrentWidget(this);
    }
}

void ItemEdition::setName(std::string name)
{
  _name = name;
  setEdited(false);
}

void ItemEdition::onApply()
{
  DEBTRACE("onApply");
  string name = _wid->le_name->text().toStdString();
  name = filterName(name);
  bool nameEdited = false;
  if (name != _name)
    {
      nameEdited = true;
      bool ret = _subject->setName(name);
      if (ret)
        {
          nameEdited = false;
          _name = name;
        }
    }
  DEBTRACE(_isEdited << " " << nameEdited);
  _isEdited = nameEdited;
  setEdited(_isEdited);
}

void ItemEdition::onCancel()
{
  DEBTRACE("onCancel");
  _wid->le_name->setText(_name.c_str());
  setEdited(false);
}

std::string ItemEdition::filterName(const std::string& name)
{
  string nameFiltered;
  nameFiltered = "";
  for (int i= 0; i< name.size(); i++)
    {
      unsigned char a = (unsigned char)(name[i]);
      if (   ((a >= '0') && (a <= '9'))
          || ((a >= 'A') && (a <= 'Z'))
          || ((a >= 'a') && (a <= 'z'))
          || ( a == '_'))
        nameFiltered += a;
    }
  DEBTRACE(name << " " << nameFiltered);
  return nameFiltered;
}

void ItemEdition::onModifyName(const QString &text)
{
  if (_name != text.toStdString()) setEdited(true);
}

void ItemEdition::setEdited(bool isEdited)
{
  DEBTRACE("ItemEdition::setEdited " << isEdited);
  if (isEdited)
    {
      QtGuiContext::getQtCurrent()->_setOfModifiedSubjects.insert(_subject);
      _subject->update(EDIT, 1, _subject);
      if (QtGuiContext::getQtCurrent()->getSubjectProc())
        QtGuiContext::getQtCurrent()->getSubjectProc()->update(EDIT, 1, _subject);
    }
  else
    {
      QtGuiContext::getQtCurrent()->_setOfModifiedSubjects.erase(_subject);
      _subject->update(EDIT, 0, _subject);
      if (QtGuiContext::getQtCurrent()->_setOfModifiedSubjects.empty() && QtGuiContext::getQtCurrent()->getSubjectProc())
        QtGuiContext::getQtCurrent()->getSubjectProc()->update(EDIT, 0, _subject);
    }

  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
  SchemaItem* schemaItem = QtGuiContext::getQtCurrent()->_mapOfSchemaItem[_subject];
  if (schemaItem)
    {
      QModelIndex index = schemaItem->modelIndex();
      model->setData(index, QVariant()); // just to emit dataChanged signal
    }
  
  _wid->pb_apply->setEnabled(isEdited);
  _wid->pb_cancel->setEnabled(isEdited);
}

/*! when loading a schema, creation of all edition widgets is time and memory consuming,
 *  so, widget edition creation is differed until user select an item in tree
 *  or 2D vue.
 */ 
void ItemEdition::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("ItemEdition::update " <<eventName(event) << " " << type);
  if (QtGuiContext::getQtCurrent()->isLoading())
    {
      // --- create only EditionProc while loading to store status
      if (event == NEWROOT)
        ItemEdition *item =  new EditionProc(son,
                                             QtGuiContext::getQtCurrent()->getStackedWidget(),
                                             son->getName().c_str());
      return;
    }

  ItemEdition *item = 0;
  switch (event)
    {
    case NEWROOT:
      item =  new EditionProc(son,
                              QtGuiContext::getQtCurrent()->getStackedWidget(),
                              son->getName().c_str());
      break;
    case ADD:
      switch (type)
        {
        case YACS::HMI::SALOMEPROC:
          item =  new EditionProc(son,
                                  QtGuiContext::getQtCurrent()->getStackedWidget(),
                                  son->getName().c_str());
          break;
        case YACS::HMI::BLOC:
          item =  new EditionBloc(son,
                                  QtGuiContext::getQtCurrent()->getStackedWidget(),
                                  son->getName().c_str());
          break;
        case YACS::HMI::FOREACHLOOP:
          item =  new EditionForEachLoop(son,
                                         QtGuiContext::getQtCurrent()->getStackedWidget(),
                                         son->getName().c_str());
          break;
        case YACS::HMI::OPTIMIZERLOOP:
          item =  new EditionOptimizerLoop(son, QtGuiContext::getQtCurrent()->getStackedWidget(),
                                           son->getName().c_str());
          break;
        case YACS::HMI::FORLOOP:
          item =  new EditionLoop(son,
                                  QtGuiContext::getQtCurrent()->getStackedWidget(),
                                  son->getName().c_str());
          break;
        case YACS::HMI::WHILELOOP:
          item =  new EditionWhile(son,
                                   QtGuiContext::getQtCurrent()->getStackedWidget(),
                                   son->getName().c_str());
          break;
        case YACS::HMI::SWITCH:
          item =  new EditionSwitch(son,
                                    QtGuiContext::getQtCurrent()->getStackedWidget(),
                                    son->getName().c_str());
          break;
        case YACS::HMI::PYTHONNODE:
          item =  new EditionScript(son,
                                    QtGuiContext::getQtCurrent()->getStackedWidget(),
                                    son->getName().c_str());
          break;
        case YACS::HMI::PYFUNCNODE:
          item =  new EditionPyFunc(son,
                                    QtGuiContext::getQtCurrent()->getStackedWidget(),
                                    son->getName().c_str());
          break;
        case YACS::HMI::CORBANODE:
        case YACS::HMI::SALOMENODE:
          item =  new EditionSalomeNode(son,
                                        QtGuiContext::getQtCurrent()->getStackedWidget(),
                                        son->getName().c_str());
          break;
        case YACS::HMI::PRESETNODE:
          item =  new EditionPresetNode(son,
                                        QtGuiContext::getQtCurrent()->getStackedWidget(),
                                        son->getName().c_str());
          break;
        case YACS::HMI::OUTNODE:
          item =  new EditionOutNode(son,
                                     QtGuiContext::getQtCurrent()->getStackedWidget(),
                                     son->getName().c_str());
          break;
        case YACS::HMI::STUDYINNODE:
          item =  new EditionStudyInNode(son,
                                         QtGuiContext::getQtCurrent()->getStackedWidget(),
                                         son->getName().c_str());
          break;
        case YACS::HMI::STUDYOUTNODE:
          item =  new EditionStudyOutNode(son,
                                          QtGuiContext::getQtCurrent()->getStackedWidget(),
                                          son->getName().c_str());
          break;
        case YACS::HMI::INPUTPORT:
        case YACS::HMI::INPUTDATASTREAMPORT:
          item =  new EditionInputPort(son,
                                       QtGuiContext::getQtCurrent()->getStackedWidget(),
                                       son->getName().c_str());
          break;
        case YACS::HMI::OUTPUTPORT:
        case YACS::HMI::OUTPUTDATASTREAMPORT:
          item =  new EditionOutputPort(son,
                                        QtGuiContext::getQtCurrent()->getStackedWidget(),
                                        son->getName().c_str());
          break;
        case YACS::HMI::CONTAINER:
          item =  new EditionContainer(son,QtGuiContext::getQtCurrent()->getStackedWidget(),son->getName().c_str());
          break;
        case YACS::HMI::COMPONENT:
          item =  new EditionComponent(son,
                                       QtGuiContext::getQtCurrent()->getStackedWidget(),
                                       son->getName().c_str());

          break;
        case YACS::HMI::DATATYPE:
          item =  new EditionDataType(son,
                                      QtGuiContext::getQtCurrent()->getStackedWidget(),
                                      son->getName().c_str());
          break;
        case YACS::HMI::REFERENCE:
        case YACS::HMI::CPPNODE:
        case YACS::HMI::SALOMEPYTHONNODE:
        case YACS::HMI::XMLNODE:
        case YACS::HMI::SPLITTERNODE:
        case YACS::HMI::DFTODSFORLOOPNODE:
        case YACS::HMI::DSTODFFORLOOPNODE:
          item =  new ItemEdition(son,
                                  QtGuiContext::getQtCurrent()->getStackedWidget(),
                                  son->getName().c_str());
          break;
        default:
          DEBTRACE("ItemEdition::update() ADD, type not handled:" << type);
        }
      break;
    case ADDLINK:
      switch (type)
        {
        case YACS::HMI::DATALINK:
          item =  new EditionDataLink(son,
                                      QtGuiContext::getQtCurrent()->getStackedWidget(),
                                      son->getName().c_str());
          break;
        default:
          DEBTRACE("ItemEdition::update() ADDLINK, type not handled:" << type);
        }
      break;
    case ADDCONTROLLINK:
      switch (type)
        {
        case YACS::HMI::CONTROLLINK:
          item =  new EditionControlLink(son,
                                         QtGuiContext::getQtCurrent()->getStackedWidget(),
                                         son->getName().c_str());
          break;
        default:
          DEBTRACE("ItemEdition::update() ADDCONTROLLINK, type not handled:" << type);
        }
      break;
    case RENAME:
      _name = _subject->getName();
      _wid->le_name->setText(_name.c_str());
      break;
    case SYNCHRO:
      synchronize();
      break;
//     default:
//       DEBTRACE("ItemEdition::update(), event not handled: " << eventName(event));
    }
}

ItemEditionRoot::ItemEditionRoot(Subject* subject,
                                 QWidget* parent,
                                 const char* name)
  : ItemEdition(subject, parent, name)
{
  DEBTRACE("ItemEditionRoot::ItemEditionRoot")
}

ItemEditionRoot::~ItemEditionRoot()
{
  _subjectSet.clear(); // --- avoid destruction loop on delete context
}
