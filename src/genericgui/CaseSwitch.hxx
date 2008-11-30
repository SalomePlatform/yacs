#ifndef _CASESWITCH_HXX_
#define _CASESWITCH_HXX_

#include "ui_CaseSwitch.h"

namespace YACS
{
  namespace HMI
  {
    class CaseSwitch: public QWidget, public Ui::CaseSwitch
    {
      Q_OBJECT

    public slots:
      void on_cb_default_stateChanged(int state);

    public:
      CaseSwitch(QWidget *parent = 0);
      ~CaseSwitch();
      bool isDefaultChecked();
      void setDefaultChecked(bool isDefault);

    protected:
      bool _isDefault;

    };
  }
}

#endif
