
#include "TableSwitch.hxx"

#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

TableSwitch::TableSwitch(QWidget *parent)
{
  DEBTRACE("TableSwitch::TableSwitch");
  setupUi(this);
}

TableSwitch::~TableSwitch()
{
  DEBTRACE("TableSwitch::~TableSwitch");
}
