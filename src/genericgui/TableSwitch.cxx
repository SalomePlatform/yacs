
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
