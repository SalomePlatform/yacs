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

