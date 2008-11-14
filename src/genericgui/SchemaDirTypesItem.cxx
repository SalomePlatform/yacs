
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
  : SchemaItem::SchemaItem(parent, label, subject)
{
  _itemDeco.replace(YLabel, QIcon("icons:folder_cyan.png"));
}

void SchemaDirTypesItem::addTypeItem(Subject* subject)
{
  DEBTRACE("SchemaDirTypesItem::addTypeItem");
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
  int nbsons = childCount();
  SubjectDataType *sdt = dynamic_cast<SubjectDataType*>(subject);
  assert(sdt);
  model->beginInsertRows(modelIndex(), nbsons, nbsons);
  SchemaItem *item = new SchemaDataTypeItem(this,
                                            sdt->getAlias().c_str(),
                                            sdt);
  model->endInsertRows();
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
