
#include "SchemaReferenceItem.hxx"
#include "Menus.hxx"

#include <QIcon>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

SchemaReferenceItem::SchemaReferenceItem(SchemaItem *parent, QString label, Subject* subject)
  : SchemaItem::SchemaItem(parent, label, subject)
{
  _itemDeco.replace(YLabel, QIcon("icons:ob_service_node.png"));
  _itemForeground.replace(YLabel,QColor("green"));
}

void SchemaReferenceItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SchemaReferenceItem::update");
  SchemaItem::update(event, type, son);
}

void SchemaReferenceItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  ReferenceMenu m;
  m.popupMenu(caller, globalPos);
}
