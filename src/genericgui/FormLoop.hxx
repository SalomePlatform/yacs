#ifndef _FORMLOOP_HXX_
#define _FORMLOOP_HXX_

#include "ui_FormLoop.h"

namespace YACS
{
  namespace HMI
  {
    class FormLoop: public QWidget, public Ui::FormLoop
    {
      Q_OBJECT

    public:
      FormLoop(QWidget *parent = 0);
      ~FormLoop();

    };
  }
}

#endif
