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
#include "FormContainer.hxx"
#include "FormComponent.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <QList>

using namespace std;

FormContainer::FormContainer(QWidget *parent)
{
  setupUi(this);
  _checked = false;
  _advanced = false;

  gridLayout2->removeWidget(cb_host);
  delete cb_host;
  cb_host = new ComboBox(gb_basic);
  gridLayout2->addWidget(cb_host, 2, 1, 1, 1);

  QIcon icon;
  icon.addFile("icons:icon_down.png");
  icon.addFile("icons:icon_up.png",
                QSize(), QIcon::Normal, QIcon::On);
  tb_container->setIcon(icon);
  on_tb_container_toggled(false);
  on_ch_advance_stateChanged(0);
}

FormContainer::~FormContainer()
{
}

void FormContainer::on_tb_container_toggled(bool checked)
{
  //DEBTRACE("FormContainer::on_tb_container_toggled " << checked);
  _checked = checked;
  if (_checked) fr_container->show();
  else fr_container->hide();
}

void FormContainer::on_ch_advance_stateChanged(int state)
{
  //DEBTRACE("FormContainer::on_ch_advance_stateChanged " << state);
  if (state) gb_advance->show();
  else gb_advance->hide();
}
