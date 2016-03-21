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

#include "FormContainerDecorator.hxx"
#include "FormContainer.hxx"
#include "FormHPContainer.hxx"

#include "HomogeneousPoolContainer.hxx"
#include "Exception.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;

bool FormContainerDecorator::CHECKED = false;

FormContainerDecorator::FormContainerDecorator(YACS::ENGINE::Container *cont, QWidget *parent):QWidget(parent),_typ(0)
{
  setupUi(this);
  _icon.addFile("icons:icon_down.png");
  _icon.addFile("icons:icon_up.png",QSize(), QIcon::Normal, QIcon::On);
  tb_container->setIcon(_icon);
  connect(this,SIGNAL(typeOfContainerIsKnown(const QString&)),label,SLOT(setText(const QString &)));
  if(!cont)
    return ;
  HomogeneousPoolContainer *hpc(dynamic_cast<HomogeneousPoolContainer *>(cont));
  if(!hpc)
    _typ=new FormContainer(this);
  else
    _typ=new FormHPContainer(this);
  emit typeOfContainerIsKnown(_typ->getTypeStr());
  _typ->FillPanel(cont);
  gridLayout_1->addWidget(_typ);
  connectForTyp();
  tb_container->setChecked(CHECKED);
  on_tb_container_toggled(CHECKED);
}

FormContainerDecorator::~FormContainerDecorator()
{
  delete _typ;
}

void FormContainerDecorator::FillPanel(YACS::ENGINE::Container *container)
{
  checkAndRepareTypeIfNecessary(container);
  _typ->FillPanel(container);
}

QWidget *FormContainerDecorator::getWidget()
{
  return _typ;
}

bool FormContainerDecorator::onApply()
{
  if(!checkOK())
    return false;
  return _typ->onApply();
}

void FormContainerDecorator::onCancel()
{
  if(!checkOK())
    return ;
  _typ->onCancel();
}

void FormContainerDecorator::show()
{
  QWidget::show();
  if(!checkOK())
    return ;
  _typ->show();
  tb_container->setChecked(CHECKED);
  on_tb_container_toggled(CHECKED);
}

void FormContainerDecorator::hide()
{
  QWidget::hide();
  if(!checkOK())
    return ;
  _typ->hide();
}

void FormContainerDecorator::on_tb_container_toggled(bool checked)
{
  DEBTRACE("FormContainer::on_tb_container_toggled " << checked);
  CHECKED = checked;
  if (CHECKED)
    {
      getWidget()->show();
    }
  else
    {
      getWidget()->hide();
    }
}

void FormContainerDecorator::synchronizeCheckContainer()
{
  tb_container->setChecked(CHECKED);
}

std::string FormContainerDecorator::getHostName(int index) const
{
  if(!checkOK())
    return std::string();
  return _typ->cb_resource->itemText(index).toStdString();
}

void FormContainerDecorator::setName(const std::string& name)
{
  if(!checkOK())
    return ;
  _typ->le_name->setText(name.c_str());
}

void FormContainerDecorator::onResMousePressed()
{
  emit(resourceMousePressed());
}

void FormContainerDecorator::onResActivated(int v)
{
  emit(resourceActivated(v));
}

void FormContainerDecorator::onContToggled(bool v)
{
  emit(containerToggled(v));
}

bool FormContainerDecorator::checkOK() const
{
  return _typ;
}

void FormContainerDecorator::checkAndRepareTypeIfNecessary(YACS::ENGINE::Container *container)
{
  if(!container)
    return ;
  YACS::ENGINE::HomogeneousPoolContainer *cont1(dynamic_cast<YACS::ENGINE::HomogeneousPoolContainer *>(container));
  if(_typ)
    {
      bool isTyp1(dynamic_cast<FormHPContainer *>(_typ)!=0);
      if((!cont1 && !isTyp1) || (cont1 && isTyp1))
        return ;
      delete _typ; _typ=0;
    }
  if(!cont1)
    _typ=new FormContainer(this);
  else
    _typ=new FormHPContainer(this);
  gridLayout_1->addWidget(_typ);
  emit typeOfContainerIsKnown(_typ->getTypeStr());
  connectForTyp();
  _typ->FillPanel(container);
}

void FormContainerDecorator::connectForTyp()
{
  connect(_typ->cb_resource,SIGNAL(mousePressed()),this,SLOT(onResMousePressed()));
  connect(_typ->cb_resource,SIGNAL(activated(int)),this,SLOT(onResActivated));
  connect(tb_container,SIGNAL(toggled(bool)),this,SLOT(onContToggled()));
}
