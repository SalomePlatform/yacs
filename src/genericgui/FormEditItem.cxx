
#include "FormEditItem.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
// using namespace YACS::HMI;

WidEditItem::WidEditItem(QWidget *parent)
{
  setupUi(this);
}

WidEditItem::~WidEditItem()
{
}


FormEditItem::FormEditItem(QWidget *parent)
  : QScrollArea(parent)
{
  _wid = new WidEditItem(parent);
  this->setWidget(_wid);
  this->setWidgetResizable(true);
}

FormEditItem::~FormEditItem()
{
}

