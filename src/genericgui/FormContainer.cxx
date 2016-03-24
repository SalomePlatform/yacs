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

#include "FormContainer.hxx"
#include "FormAdvParamContainer.hxx"
#include "Container.hxx"
//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <QComboBox>
#include <QList>

#include <sstream>

using namespace std;

FormContainer::FormContainer(QWidget *parent):FormContainerBase(parent),cb_type(new QComboBox(this))
{ 
  gridLayout_2_2->addWidget(cb_type);
  FormContainer::FillPanel(0); // --- set widgets before signal connexion to avoid false modif detection
  connect(cb_type, SIGNAL(activated(const QString&)),this, SLOT(onModifyType(const QString&)));
  connect(ch_aoc,SIGNAL(stateChanged(int)),this,SLOT(onModifyAOC(int)));
}

FormContainer::~FormContainer()
{
}

void FormContainer::FillPanel(YACS::ENGINE::Container *container)
{
  DEBTRACE("FormContainer::FillPanel");
  FormContainerBase::FillPanel(container);
  if(container)
    ch_aoc->setCheckState(container->isAttachedOnCloning()?Qt::Checked:Qt::Unchecked);
  cb_type->clear();
  cb_type->addItem("mono");
  cb_type->addItem("multi");
  if(_properties.count("type") && _properties["type"]=="multi")
    cb_type->setCurrentIndex(1);
}

QString FormContainer::getTypeStr() const
{
  return QString("Container");
}

void FormContainer::onModifyType(const QString &text)
{
  DEBTRACE("onModifyType " << text.toStdString());
  if (!_container)
    return;
  std::string prop=_container->getProperty("type");
  _properties["type"] = text.toStdString();
  if (_properties["type"] == "mono")
    _advancedParams->setModeText("mono");
  else
    _advancedParams->setModeText("multi");
  if (prop != text.toStdString())
    onModified();
}

void FormContainer::onModifyAOC(int val)
{
  if (!_container)
    return;
  bool val2(false);
  if(val==Qt::Unchecked)
    val2=false;
  if(val==Qt::Checked)
    val2=true;
  bool prop(_container->isAttachedOnCloning());
  int prop2((int)val2);
  std::ostringstream oss; oss << prop2;
  _properties[YACS::ENGINE::Container::AOC_ENTRY]=oss.str();
  _container->setAttachOnCloningStatus(val2);
  if(prop!=val2)
    onModified();
}
