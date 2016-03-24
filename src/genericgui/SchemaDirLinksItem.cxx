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

#include "SchemaDirLinksItem.hxx"
#include "SchemaLinkItem.hxx"
#include "SchemaModel.hxx"
#include "QtGuiContext.hxx"
#include "GuiEditor.hxx"

#include <QIcon>
#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

SchemaDirLinksItem::SchemaDirLinksItem(SchemaItem *parent, QString label, Subject* subject)
  : SchemaItem(parent, label, subject)
{
  _itemDeco.replace(YLabel, QIcon("icons:folder_cyan.png"));
}

void SchemaDirLinksItem::addLinkItem(Subject* subject)
{
  DEBTRACE("SchemaDirLinksItem::addTypeItem");
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
  int nbsons = childCount();
  model->beginInsertRows(modelIndex(), nbsons, nbsons);
  SchemaItem *item = new SchemaLinkItem(this,
                                        subject->getName().c_str(),
                                        subject);
  model->endInsertRows();
}

Qt::ItemFlags SchemaDirLinksItem::flags(const QModelIndex &index)
{
  //DEBTRACE("SchemaDirLinksItem::flags");
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant SchemaDirLinksItem::editionWhatsThis(int column) const
{
return "<p>Links between ports or nodes under that block level appears in this folder.\n" \
  "You can add data links (between ports) and control links (between nodes) " \
  "by drag from origin (out node or out port) to destination (in node or in port) " \
  "either in 2D graphic view or tree view. <a href=\"modification.html#create-a-link\">More...</a></p>";
}

