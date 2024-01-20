// Copyright (C) 2016-2024  CEA, EDF
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

#include "Py2YacsDialog.hxx"
#include <PyEditor_Window.h>
#include <PyEditor_Widget.h>
#include <py2yacs.hxx>

Py2YacsDialog::Py2YacsDialog( QWidget* parent)
: QDialog(parent),
  _yacsFile(),
  _pyEditorWindow(0),
  _errorMessages(0),
  _okButton(0),
  _exportXml(true),
  _pyScript()
{
  QVBoxLayout *mainLayout = new QVBoxLayout;
  _pyEditorWindow = new PyEditor_Window;

  _errorMessages = new QTextEdit(this);
  _errorMessages->setReadOnly(true);
  _errorMessages->hide();
  QSplitter * splitterW = new QSplitter(Qt::Vertical);
  splitterW->addWidget(_pyEditorWindow);
  splitterW->addWidget(_errorMessages);
  mainLayout->addWidget(splitterW);
  
  QHBoxLayout *validationLayout = new QHBoxLayout;
  _okButton = new QPushButton(tr("py -> &YACS"));
  QPushButton * cancelButton = new QPushButton(tr("&Cancel"));
  validationLayout->addWidget(_okButton);
  validationLayout->addWidget(cancelButton);
  mainLayout->addLayout(validationLayout);

  setLayout(mainLayout);
  setWindowTitle(tr("Python to YACS schema editor"));

  connect(cancelButton,SIGNAL(clicked()),this, SLOT(reject()));
  connect(_okButton,SIGNAL(clicked()),this, SLOT(onExport()));

  // PyEditor_Window has a button "exit".
  // Trigger "cancel" when the editor is closed.
  _pyEditorWindow->setAttribute(Qt::WA_DeleteOnClose);
  connect(_pyEditorWindow,SIGNAL(destroyed()),this, SLOT(reject()));
}

void Py2YacsDialog::onExport()
{
  PyEditor_Widget* pyEdit = dynamic_cast<PyEditor_Widget*>
                                             (_pyEditorWindow->centralWidget());
  if(!pyEdit)
  {
    reject();
    return;
  }

  Py2yacs converter;
  _pyScript = pyEdit->text().toStdString();
  try
  {
    converter.load(_pyScript);
    // _exec -> default name for OPENTURNS functions
    std::string errors = converter.getFunctionErrors("_exec");
    if(errors.empty())
    {
      if(_exportXml)
      {
        QSettings settings;
        QString currentDir = settings.value("currentDir").toString();
        if (currentDir.isEmpty())
          currentDir = QDir::homePath();
        QString fileName = QFileDialog::getSaveFileName(this,
                                    tr("Save to YACS schema..."),
                                    currentDir,
                                    QString("%1 (*.xml)" ).arg( tr("xml files")));
        if (!fileName.isEmpty())
        {
          if (!fileName.endsWith(".xml"))
            fileName += ".xml";
          QFile file(fileName);
          settings.setValue("currentDir", QFileInfo(fileName).absolutePath());
          converter.save(fileName.toStdString(), "_exec");
          _yacsFile = fileName;
          accept();
        }
      }
      else
        accept();
    }
    else
    {
      _errorMessages->show();
      _errorMessages->setText(errors.c_str());
    }
  }
  catch(Py2yacsException& e)
  {
    const char * error = e.what();
    _errorMessages->show();
    _errorMessages->setText(QString(error));
    return;
  }
}

QString Py2YacsDialog::getYacsFile()
{
  return _yacsFile;
}

std::string Py2YacsDialog::getScriptText()
{
  return _pyScript;
}

void Py2YacsDialog::setScriptText(const std::string& pyScript)
{
  PyEditor_Widget* pyEdit = dynamic_cast<PyEditor_Widget*>
                                             (_pyEditorWindow->centralWidget());
  if(pyEdit != nullptr)
    pyEdit->setText(QString(pyScript.c_str()));
}

void Py2YacsDialog::setExportXml(bool yes)
{
  _exportXml = yes;
}
