
#ifndef _FORMEDITTREE_HXX_
#define _FORMEDITTREE_HXX_

#include "ui_FormEditTree.h"

class FormEditTree: public QWidget, public Ui::fm_schemaTree
{
  Q_OBJECT

public:
  FormEditTree(QWidget *parent = 0);
  virtual ~FormEditTree();

public slots:

protected:

private:
};


#endif
