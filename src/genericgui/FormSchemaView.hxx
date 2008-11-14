
#ifndef _FORMSCHEMAVIEW_HXX_
#define _FORMSCHEMAVIEW_HXX_

#include "ui_FormSchemaView.h"

class FormSchemaView: public QWidget, public Ui::fm_viewSchema
{
  Q_OBJECT

public:
  FormSchemaView(QWidget *parent = 0);
  virtual ~FormSchemaView();

public slots:
void on_dial_valueChanged(int val);
void on_hs_position_valueChanged(int val);
void on_vs_position_valueChanged(int val);
void on_vs_zoom_valueChanged(int val);

protected:
  int _prevRotate;
  int _prevHsPos;
  int _prevVsPos;
  int _prevZoom;

private:
};


#endif
