#ifndef _TABLESWITCH_HXX_
#define _TABLESWITCH_HXX_

#include "ui_TableSwitch.h"

namespace YACS
{
  namespace HMI
  {
    class TableSwitch: public QWidget, public Ui::TableSwitch
    {
      Q_OBJECT

    public:
      TableSwitch(QWidget *parent = 0);
      ~TableSwitch();

    };
  }
}

#endif
