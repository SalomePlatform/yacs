// Copyright (C) 2016-2023  CEA, EDF
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
#ifndef PY2YACSDIALOG_RAW_HXX
#define PY2YACSDIALOG_RAW_HXX

#include "config_py2yacsgui.hxx"
#include "Py2YacsModel.hxx"
#include <QDialog>

#ifdef HAS_PYEDITOR
 class PyEditor_Editor;
#else
 class QTextEdit;
#endif
 class QComboBox;
 class QPushButton;

class PY2YACSGUILIB_EXPORT Py2YacsDialog_raw : public QDialog
{
  Q_OBJECT
  public:
    Py2YacsDialog_raw( QWidget* parent=0);
    YACS::ENGINE::Proc* getYacsSchema();
    QString getYacsFile();
  public slots:
    virtual void onFunctionNamesChange(std::list<std::string> validFunctionNames);
    virtual void onLoad();
    virtual void onExport();
    virtual void checkModel();
    virtual void invalidModel();
    virtual void onApply();
    virtual void onSave();
    virtual void onSaveAs();
  private:
    Py2YacsModel _model;
    QString _yacsFile;
#ifdef HAS_PYEDITOR
    PyEditor_Editor *_pyEditor;
#else
    QTextEdit *_pyEditor;
#endif
    QComboBox *_functionChosen;
    QPushButton *_saveButton;
    QPushButton *_exportButton;
    QPushButton *_okButton;
};

#endif // PY2YACSDIALOG_RAW_HXX
