// Copyright (C) 2016-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef PY2YACSDIALOG_HXX
#define PY2YACSDIALOG_HXX

#include "config_py2yacsgui.hxx"
#include <QtWidgets>

class PyEditor_Window;
class PY2YACSGUILIB_EXPORT Py2YacsDialog : public QDialog
{
  Q_OBJECT
  public:
    Py2YacsDialog( QWidget* parent=0);
    QString getYacsFile();

  public slots:
    virtual void onExport();

  private:
    QString _yacsFile;
    PyEditor_Window *_pyEditorWindow;
    QTextEdit * _errorMessages;
    QPushButton *_okButton;
};

#endif // PY2YACSDIALOG_HXX
