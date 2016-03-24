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

#include "SchemaReferenceItem.hxx"
#include "Menus.hxx"
#include "QtGuiContext.hxx"

#include <QIcon>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

SchemaReferenceItem::SchemaReferenceItem(SchemaItem *parent, QString label, Subject* subject)
  : SchemaItem(parent, label, subject)
{
  _itemDeco.replace(YLabel, QIcon("icons:ob_service_node.png"));
  _itemForeground.replace(YLabel,QColor("green"));
}

void SchemaReferenceItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SchemaReferenceItem::update "<< eventName(event) <<" "<<type<<" "<<son);
  if(event==RENAME)
    {
      SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
      _label = son->getName().c_str();
      _itemData.replace(YLabel, _label);
      model->setData(modelIndex(YLabel), 0);  // --- to emit dataChanged signal
    }
  else
    SchemaItem::update(event, type, son);
}

void SchemaReferenceItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  ReferenceMenu m;
  m.popupMenu(caller, globalPos);
}
