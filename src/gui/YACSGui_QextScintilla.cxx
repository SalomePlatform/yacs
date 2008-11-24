#include "yacs_config.h"
#include "YACSGui_QextScintilla.h"

#include <qevent.h>

#ifdef HAVE_QEXTSCINTILLA_H

YACSGui_QextScintilla::YACSGui_QextScintilla(QWidget *parent, const char *name, WFlags f)
  : QextScintilla(parent, name, f)
{
}

YACSGui_QextScintilla::~YACSGui_QextScintilla()
{
}

bool YACSGui_QextScintilla::event(QEvent *e)
{
  if (e->type() == QEvent::AccelOverride)
    {
      QKeyEvent *ke = (QKeyEvent*)e;
      if ( ((ke->state() == Qt::NoButton) || (ke->state() == Qt::ShiftButton))
           && (ke->key() != Qt::Key_Escape))
        {
          ke->accept();
          return true;
        }
    }
  return QextScintilla::event(e);
}

#endif
