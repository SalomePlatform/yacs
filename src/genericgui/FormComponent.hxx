
#ifndef _FORMCOMPONENT_HXX_
#define _FORMCOMPONENT_HXX_

#include "ui_FormComponent.h"

#include <QIcon>
#include <QComboBox>

class ComboBox: public QComboBox
{
  Q_OBJECT
  
public: 
  ComboBox(QWidget *parent =0);
  virtual ~ComboBox();

signals:
  void mousePressed();

protected:
  virtual void mousePressEvent(QMouseEvent *event);
};


class FormComponent: public QWidget, public Ui::fm_component
{
  Q_OBJECT

public:
  FormComponent(QWidget *parent = 0);
  virtual ~FormComponent();

public slots:
  void on_tb_component_toggled(bool checked);

protected:
  bool _checked;

private:
};


#endif
