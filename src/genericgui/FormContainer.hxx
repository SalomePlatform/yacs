
#ifndef _FORMCONTAINER_HXX_
#define _FORMCONTAINER_HXX_

#include "ui_FormContainer.h"

#include <QIcon>

class FormContainer: public QWidget, public Ui::fm_container
{
  Q_OBJECT

public:
  FormContainer(QWidget *parent = 0);
  virtual ~FormContainer();

public slots:
  void on_tb_container_toggled(bool checked);
  void on_ch_advance_stateChanged(int state);

protected:
  bool _checked;
  bool _advanced;

private:
};

#endif
