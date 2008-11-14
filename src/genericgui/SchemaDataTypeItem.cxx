#include "SchemaDataTypeItem.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

SchemaDataTypeItem::SchemaDataTypeItem(SchemaItem *parent, QString label, Subject* subject)
  : SchemaItem::SchemaItem(parent, label, subject)
{
}

void SchemaDataTypeItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SchemaDataTypeItem::update");
  SchemaItem::update(event, type, son);
}
 
