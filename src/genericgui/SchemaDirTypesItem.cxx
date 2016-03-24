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

#include "SchemaDirTypesItem.hxx"
#include "SchemaDataTypeItem.hxx"
#include "SchemaModel.hxx"
#include "QtGuiContext.hxx"
#include "ItemMimeData.hxx"
#include "GuiEditor.hxx"

#include <QIcon>
#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

SchemaDirTypesItem::SchemaDirTypesItem(SchemaItem *parent, QString label, Subject* subject)
  : SchemaItem(parent, label, subject)
{
  _itemDeco.replace(YLabel, QIcon("icons:folder_cyan.png"));
}

void SchemaDirTypesItem::addTypeItem(Subject* subject)
{
  DEBTRACE("SchemaDirTypesItem::addTypeItem");
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
  int nbsons = childCount();
  SubjectDataType *sdt = dynamic_cast<SubjectDataType*>(subject);
  YASSERT(sdt);
  DEBTRACE(nbsons);
  model->beginInsertRows(modelIndex(), nbsons, nbsons);
  SchemaItem *item = new SchemaDataTypeItem(this,
                                            sdt->getAlias().c_str(),
                                            sdt);
  model->endInsertRows();
}

void SchemaDirTypesItem::removeTypeItem(Subject* subject)
{
  DEBTRACE("SchemaDirTypesItem::removeTypeItem");
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
  YASSERT(QtGuiContext::getQtCurrent()->_mapOfSchemaItem.count(subject));
  SchemaItem *toRemove = QtGuiContext::getQtCurrent()->_mapOfSchemaItem[subject];
  int position = toRemove->row();
  DEBTRACE(position);
  model->beginRemoveRows(modelIndex(), position, position);
  removeChild(toRemove);
  model->endRemoveRows();
}

Qt::ItemFlags SchemaDirTypesItem::flags(const QModelIndex &index)
{
  //DEBTRACE("SchemaDirTypesItem::flags");
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
}

bool SchemaDirTypesItem::dropMimeData(const QMimeData* data, Qt::DropAction action)
{
  DEBTRACE("SchemaDirTypesItem::dropMimeData");
  if (!data) return false;
  const ItemMimeData* myData = dynamic_cast<const ItemMimeData*>(data);
  if (!myData) return false;
  bool ret =false;
  if (myData->hasFormat("yacs/cataType"))
    {
      ret =true;
      QtGuiContext::getQtCurrent()->getGMain()->_guiEditor->AddTypeFromCatalog(myData);
    }
  return ret;
}

QVariant SchemaDirTypesItem::editionWhatsThis(int column) const
{
return "All the Data types required in this schema must be in this folder.\n" \
  "You can add data types here by import from catalogs "                \
  "(builtin catalog, session catalog, or a catalog built from another schema).\n" \
  "To add a data type, drag a data type item from catalogs panel and drop it here.";
}

