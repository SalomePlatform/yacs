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

#include "FormComponent.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <QList>

using namespace std;

bool FormComponent::_checked = false;

ComboBox::ComboBox(QWidget *parent)
  : QComboBox(parent)
{
}

ComboBox::~ComboBox()
{
}
    
void ComboBox::mousePressEvent(QMouseEvent *event)
{
  emit mousePressed();
  QComboBox::mousePressEvent(event);
}


FormComponent::FormComponent(QWidget *parent)
{
  setupUi(this);

  le_name->setReadOnly(true);

  gridLayout3->removeWidget(cb_instance);
  delete cb_instance;
  cb_instance = new ComboBox(fr_component);
  gridLayout3->addWidget(cb_instance, 1, 1, 1, 1);

  gridLayout3->removeWidget(cb_container);
  delete cb_container;
  cb_container = new ComboBox(fr_component);
  gridLayout3->addWidget(cb_container, 2, 1, 1, 1);

  QIcon icon;
  icon.addFile("icons:icon_down.png");
  icon.addFile("icons:icon_up.png",
                QSize(), QIcon::Normal, QIcon::On);
  tb_component->setIcon(icon);
  on_tb_component_toggled(FormComponent::_checked);
}

FormComponent::~FormComponent()
{
}

void FormComponent::on_tb_component_toggled(bool checked)
{
  DEBTRACE("FormComponent::on_tb_component_toggled " << checked);
  _checked = checked;
  if (_checked) fr_component->show();
  else fr_component->hide();
}
