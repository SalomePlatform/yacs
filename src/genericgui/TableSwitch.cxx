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

#include "TableSwitch.hxx"
#include "QtGuiContext.hxx"
#include "SchemaItem.hxx"

#include <QHeaderView>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

TableSwitch::TableSwitch(QWidget *parent)
{
  DEBTRACE("TableSwitch::TableSwitch");
  setupUi(this);
  if (tv_nodes->horizontalHeader())
      tv_nodes->horizontalHeader()->setStretchLastSection(true);
}

TableSwitch::~TableSwitch()
{
  DEBTRACE("TableSwitch::~TableSwitch");
}

void TableSwitch::setEditableCase(bool isEditable)
{
  DEBTRACE("TableSwitch::setEditablePorts " << isEditable);

  tv_nodes->setColumnHidden(YType,  true);
  tv_nodes->setColumnHidden(YValue, false);

  if (tv_nodes->verticalHeader())
    tv_nodes->verticalHeader()->setVisible(false);

}

void TableSwitch::adjustColumns()
{
  DEBTRACE("TableSwitch::adjustColumns");
  if (!tv_nodes->isColumnHidden(YLabel))
    tv_nodes->resizeColumnToContents(YLabel); 
  if (!tv_nodes->isColumnHidden(YType))
    tv_nodes->resizeColumnToContents(YType); 
}
