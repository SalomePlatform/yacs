#ifndef _FORMEACHLOOP_HXX_
#define _FORMEACHLOOP_HXX_

#include "ui_FormEachLoop.h"

namespace YACS
{
  namespace HMI
  {
    class FormEachLoop: public QWidget, public Ui::FormEachLoop
    {
      Q_OBJECT

    public:
      FormEachLoop(QWidget *parent = 0);
      ~FormEachLoop();

    };
  }
}

#endif
