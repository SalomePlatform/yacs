#ifndef _LOGVIEWER_HXX_
#define _LOGVIEWER_HXX_

#include "ui_LogViewer.h"

#include <string>

class LogViewer: public QDialog, public Ui::logViewerDialog
{
  Q_OBJECT

public:
  LogViewer(std::string label, QWidget *parent = 0);
  virtual ~LogViewer();
  void setText(std::string text);
  void readFile(std::string fileName);
protected:
  std::string _label;
};

#endif
