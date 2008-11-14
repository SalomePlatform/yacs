
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
  : SchemaItem::SchemaItem(parent, label, subject)
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
