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

#include "FormContainerBase.hxx"
#include "FormAdvParamContainer.hxx"
#include "FormComponent.hxx"
#include "QtGuiContext.hxx"
#include "Container.hxx"

#include <cassert>
#include <cstdlib>
#include <climits>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <QList>
#include <sstream>

using namespace std;
using namespace YACS;
using namespace YACS::HMI;
using namespace YACS::ENGINE;

FormContainerBase::FormContainerBase(QWidget *parent):QWidget(parent),_advancedParams(new FormAdvParamContainer(_properties,this))
{
  setupUi(this);
  _advanced = false;
  gridLayout_2->addWidget(_advancedParams);
  on_ch_advance_stateChanged(0);
  
  FillPanel(0); // --- set widgets before signal connexion to avoid false modif detection
  connect(le_name, SIGNAL(textChanged(const QString&)), this, SLOT(onModifyName(const QString&)));
  connect(cb_resource, SIGNAL(activated(const QString&)), _advancedParams, SLOT(onModifyResource(const QString&)));
}

FormContainerBase::~FormContainerBase()
{
  delete _advancedParams;
}

void FormContainerBase::FillPanel(YACS::ENGINE::Container *container)
{
  DEBTRACE("FormContainer::FillPanel");
  _container = container;
  if (_container)
    {
      _properties = _container->getProperties();
      le_name->setText(_container->getName().c_str());
    }
  else
    {
      _properties.clear();
      le_name->setText("not defined");
    }

  //Resources
  cb_resource->clear();
  cb_resource->addItem("automatic"); // --- when no resource is selected

  //add available resources
  list<string> machines = QtGuiContext::getQtCurrent()->getGMain()->getMachineList();
  list<string>::iterator itm = machines.begin();
  for( ; itm != machines.end(); ++itm)
    {
      cb_resource->addItem(QString((*itm).c_str()));
    }

  std::string resource;
  if(_properties.count("name") && _properties["name"] != "")
    {
      //a resource has been specified
      int index = cb_resource->findText(_properties["name"].c_str());
      if (index > 0)
        {
          //the resource is found: use it
          cb_resource->setCurrentIndex(index);
          resource=_properties["name"];
        }
      else
        {
          //the resource has not been found: add a false item
          std::string item="Unknown resource ("+_properties["name"]+")";
          cb_resource->addItem(item.c_str());
          cb_resource->setCurrentIndex(cb_resource->count()-1);
        }
    }
  else
    cb_resource->setCurrentIndex(0);
  _advancedParams->FillPanel(resource,container);

  if (!QtGuiContext::getQtCurrent()->isEdition())
    {
      //if the schema is in execution do not allow editing
      le_name->setReadOnly(true);
      cb_resource->setEnabled(false);
    }
}

void FormContainerBase::onModified()
{
  DEBTRACE("FormContainerBase::onModified");
  Subject *sub(QtGuiContext::getQtCurrent()->getSelectedSubject());
  if (!sub)
    return;
  YASSERT(QtGuiContext::getQtCurrent()->_mapOfEditionItem.count(sub));
  QWidget *widget(QtGuiContext::getQtCurrent()->_mapOfEditionItem[sub]);
  ItemEdition *item(dynamic_cast<ItemEdition*>(widget));
  YASSERT(item);
  item->setEdited(true);
}

void FormContainerBase::on_ch_advance_stateChanged(int state)
{
  DEBTRACE("FormContainer::on_ch_advance_stateChanged " << state);
  if (state)
    _advancedParams->show();
  else
    _advancedParams->hide();
}

void FormContainerBase::onModifyName(const QString &text)
{
  DEBTRACE("onModifyName " << text.toStdString());
  SubjectContainerBase *scont(QtGuiContext::getQtCurrent()->_mapOfSubjectContainer[_container]);
  YASSERT(scont);
  string name = scont->getName();
  if (name != text.toStdString())
    onModified();
}

bool FormContainerBase::onApply()
{
  SubjectContainerBase *scont(QtGuiContext::getQtCurrent()->_mapOfSubjectContainer[_container]);
  YASSERT(scont);
  bool ret(scont->setName(le_name->text().toStdString()));
  DEBTRACE(ret);
  if (ret)
    ret = scont->setProperties(_properties);
  return ret;
}

void FormContainerBase::onCancel()
{
  SubjectContainerBase *scont(QtGuiContext::getQtCurrent()->_mapOfSubjectContainer[_container]);
  YASSERT(scont);
  FillPanel(scont->getContainer());
}
