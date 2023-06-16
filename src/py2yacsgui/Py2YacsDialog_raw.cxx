// Copyright (C) 2016-2023  CEA/DEN, EDF R&D
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
#include <QtWidgets>
#include <QSettings>
#ifdef HAS_PYEDITOR
#include <PyEditor_Editor.h>
#endif

#include "Py2YacsDialog_raw.hxx"

Py2YacsDialog_raw::Py2YacsDialog_raw( QWidget* parent)
: QDialog(parent)
{
  QHBoxLayout *fileLayout = new QHBoxLayout;
  QPushButton *loadButton = new QPushButton(tr("&Load"));
  _saveButton = new QPushButton(tr("&Save"));
  QPushButton *saveAsButton = new QPushButton(tr("Save &as ..."));
  fileLayout->addWidget(loadButton);
  fileLayout->addWidget(_saveButton);
  fileLayout->addWidget(saveAsButton);

#ifdef HAS_PYEDITOR
  _pyEditor = new PyEditor_Editor(this);
#else
  _pyEditor = new QTextEdit(this);
#endif

  QPushButton * applyButton = new QPushButton(tr("A&pply"));
  QTextEdit *errorMessages = new QTextEdit(this);
  errorMessages->setReadOnly(true);

  QHBoxLayout *exportLayout = new QHBoxLayout;
  _functionChosen = new QComboBox(this);
  _exportButton = new QPushButton(tr("E&xport to YACS schema..."));
  exportLayout->addWidget(new QLabel(tr("Function to run:")));
  exportLayout->addWidget(_functionChosen);

  QHBoxLayout *validationLayout = new QHBoxLayout;
  _okButton = new QPushButton(tr("Save YACS schema and &quit"));
  QPushButton * cancelButton = new QPushButton(tr("&Cancel"));
  validationLayout->addWidget(_okButton);
  validationLayout->addWidget(cancelButton);

  QGroupBox *editWidget = new QGroupBox(tr("Python script:"));
  QVBoxLayout *editLayout = new QVBoxLayout;
  editLayout->addLayout(fileLayout);
  editLayout->addWidget(_pyEditor);
  editLayout->addWidget(applyButton);
  editLayout->addLayout(exportLayout);
  editWidget->setLayout(editLayout);

  QGroupBox *messageWidget = new QGroupBox(tr("Messages:"));
  QVBoxLayout *messageLayout = new QVBoxLayout;
  messageLayout->addWidget(errorMessages);
  messageWidget->setLayout(messageLayout);

  QSplitter * splitterW = new QSplitter(Qt::Vertical);
  splitterW->addWidget(editWidget);
  splitterW->addWidget(messageWidget);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(splitterW);
  mainLayout->addLayout(validationLayout);
  setLayout(mainLayout);
  setWindowTitle(tr("Python to YACS schema editor"));

  invalidModel();
  _saveButton->setEnabled(false);
  connect(_pyEditor, SIGNAL(textChanged()), this, SLOT(invalidModel()));
  connect(applyButton,SIGNAL(clicked()),this, SLOT(onApply()));

  connect(&_model, SIGNAL(scriptChanged(const QString&)),
          _pyEditor, SLOT(setText(const QString&)));
  connect(&_model, SIGNAL(errorChanged(const QString&)),
          errorMessages, SLOT(setText(const QString&)));
  connect(&_model, SIGNAL(functionsChanged(std::list<std::string>)),
          this, SLOT(onFunctionNamesChange(std::list<std::string>)));
  connect(_functionChosen,SIGNAL(currentIndexChanged(const QString &)),
          &_model, SLOT(setFunctionName(const QString&)));
  connect(loadButton,SIGNAL(clicked()),this, SLOT(onLoad()));
  connect(_saveButton,SIGNAL(clicked()),this, SLOT(onSave()));
  connect(saveAsButton,SIGNAL(clicked()),this, SLOT(onSaveAs()));
  connect(_exportButton,SIGNAL(clicked()),this, SLOT(onExport()));
  connect(cancelButton,SIGNAL(clicked()),this, SLOT(reject()));
  connect(_okButton,SIGNAL(clicked()),this, SLOT(onExport()));
}

void Py2YacsDialog_raw::onFunctionNamesChange(std::list<std::string> validFunctionNames)
{
  int new_index = 0;
  int count = 0;
  QString lastChoice = _functionChosen->currentText();
  _functionChosen->clear();
  std::list<std::string>::const_iterator it;
  for(it=validFunctionNames.begin(); it!=validFunctionNames.end(); it++)
  {
    _functionChosen->addItem(it->c_str());
    if(lastChoice == it->c_str())
      new_index = count;
    count++;
  }
  _functionChosen->setCurrentIndex(new_index);
}

void Py2YacsDialog_raw::onLoad()
{
  QSettings settings;
  QString currentDir = settings.value("currentDir").toString();
  if (currentDir.isEmpty())
    currentDir = QDir::homePath();
  QString fileName = QFileDialog::getOpenFileName(this, tr("Python script to import..."),
                     currentDir,
                     tr("Python script (*.py);;"));

  if (!fileName.isEmpty())
  {
    QFile file(fileName);
    settings.setValue("currentDir", QFileInfo(fileName).absolutePath());
    
    _model.loadFile(fileName.toStdString());
    _saveButton->setEnabled(_model.savePossible());
    checkModel();
  }
}

void Py2YacsDialog_raw::onExport()
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
    if(_model.exportToXml(fileName.toStdString()))
    {
      _yacsFile = fileName;
      accept();
    }
  }
}

void Py2YacsDialog_raw::onApply()
{
  _model.setScript(_pyEditor->toPlainText().toStdString());
  checkModel();
}

void Py2YacsDialog_raw::invalidModel()
{
  _okButton->setEnabled(false);
  _exportButton->setEnabled(false);
  _functionChosen->setEnabled(false);
}

void Py2YacsDialog_raw::checkModel()
{
  bool modelState = _model.schemaAvailable();
  _okButton->setEnabled(modelState);
  _exportButton->setEnabled(modelState);
  _functionChosen->setEnabled(modelState);
}

void Py2YacsDialog_raw::onSave()
{
  _model.setScript(_pyEditor->toPlainText().toStdString());
  _model.save();
  checkModel();
}

void Py2YacsDialog_raw::onSaveAs()
{
  QSettings settings;
  QString currentDir = settings.value("currentDir").toString();
  if (currentDir.isEmpty())
    currentDir = QDir::homePath();
  QString fileName = QFileDialog::getSaveFileName(this,
                                  tr("Save to python file..."),
                                  currentDir,
                                  QString("%1 (*.py)" ).arg( tr("python files")));
  if (!fileName.isEmpty())
  {
    if (!fileName.endsWith(".py"))
      fileName += ".py";
    QFile file(fileName);
    settings.setValue("currentDir", QFileInfo(fileName).absolutePath());
    _model.setScript(_pyEditor->toPlainText().toStdString());
    _model.saveAs(fileName.toStdString());
    _saveButton->setEnabled(_model.savePossible());
    checkModel();
  }
}

YACS::ENGINE::Proc* Py2YacsDialog_raw::getYacsSchema()
{
  return _model.getProc();
}

QString Py2YacsDialog_raw::getYacsFile()
{
  return _yacsFile;
}