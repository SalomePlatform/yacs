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
#include "SchemaLinkItem.hxx"
#include "Menus.hxx"

#include <QIcon>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

SchemaLinkItem::SchemaLinkItem(SchemaItem *parent, QString label, Subject* subject)
  : SchemaItem::SchemaItem(parent, label, subject)
{
  _itemDeco.replace(YLabel, QIcon("icons:new_link.png"));
}

void SchemaLinkItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SchemaLinkItem::update");
  SchemaItem::update(event, type, son);
}

void SchemaLinkItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  LinkMenu m;
  m.popupMenu(caller, globalPos);
}


QVariant SchemaLinkItem::editionWhatsThis(int column) const
{
return "Links between ports or nodes under that block level appears in this folder.\n" \
  "You can add data links (between ports) and control links (between nodes) " \
  "by drag from origin (out node or out port) to destination (in node or in port) " \
  "either in 2D graphic view or tree view.";
}
