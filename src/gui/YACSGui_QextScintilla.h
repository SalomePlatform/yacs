#ifndef _YACSGUI_QEXTSCINTILLA_H_
#define _YACSGUI_QEXTSCINTILLA_H_

#ifdef HAVE_QEXTSCINTILLA_H

#include <qextscintilla.h>
#include <qextscintillalexerpython.h>

class YACSGui_QextScintilla: public QextScintilla
{
public:
  YACSGui_QextScintilla(QWidget *parent = 0, const char *name = 0, WFlags f = 0);
  virtual ~YACSGui_QextScintilla();
protected:
  virtual bool event(QEvent *e);
};

#endif
#endif
