
#ifndef _FORMEDITITEM_HXX_
#define _FORMEDITITEM_HXX_

#include <QScrollArea>

#include "ui_FormEditItem.h"

class WidEditItem: public QWidget, public Ui::fm_editItem
{
  Q_OBJECT

public:
  WidEditItem(QWidget *parent = 0);
  virtual ~WidEditItem();

public slots:

protected:

private:
};

class FormEditItem: public QScrollArea
{
  Q_OBJECT

public:
  FormEditItem(QWidget *parent = 0);
  virtual ~FormEditItem();

public slots:

protected:
  WidEditItem* _wid;

private:
};

#endif
