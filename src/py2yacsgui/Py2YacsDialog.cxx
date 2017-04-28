// Copyright (C) 2016-2017  CEA/DEN, EDF R&D
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

#include "Py2YacsDialog.hxx"

Py2YacsDialog::Py2YacsDialog( QWidget* parent)
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
  exportLayout->addWidget(_exportButton);

  QHBoxLayout *validationLayout = new QHBoxLayout;
  _okButton = new QPushButton(tr("O&k"));
  QPushButton * cancelButton = new QPushButton(tr("&Cancel"));
  validationLayout->addWidget(_okButton);
  validationLayout->addWidget(cancelButton);
  
  QGroupBox *editWidget = new QGroupBox(tr("Python script:"));
  QVBoxLayout *editLayout = new QVBoxLayout;
  editLayout->addLayout(fileLayout);
  editLayout->addWidget(_pyEditor);
  editLayout->addWidget(applyButton);
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
  mainLayout->addLayout(exportLayout);
  mainLayout->addLayout(validationLayout);
  setLayout(mainLayout);
  setWindowTitle(tr("Python to YACS schema editor"));
  
  invalidModel();
  _saveButton->setEnabled(false);
  connect(_pyEditor, SIGNAL(textChanged()), this, SLOT(invalidModel()));
  connect(applyButton,SIGNAL(clicked()),this, SLOT(onApply()));
  
  connect(&model, SIGNAL(scriptChanged(const QString&)),
          _pyEditor, SLOT(setText(const QString&)));
  connect(&model, SIGNAL(errorChanged(const QString&)),
          errorMessages, SLOT(setText(const QString&)));
  connect(&model, SIGNAL(functionsChanged(std::list<std::string>)),
          this, SLOT(onFunctionNamesChange(std::list<std::string>)));
  connect(_functionChosen,SIGNAL(currentIndexChanged(const QString &)),
          &model, SLOT(setFunctionName(const QString&)));
  connect(loadButton,SIGNAL(clicked()),this, SLOT(onLoad()));
  connect(_saveButton,SIGNAL(clicked()),this, SLOT(onSave()));
  connect(saveAsButton,SIGNAL(clicked()),this, SLOT(onSaveAs()));
  connect(_exportButton,SIGNAL(clicked()),this, SLOT(onExport()));
  connect(cancelButton,SIGNAL(clicked()),this, SLOT(reject()));
  connect(_okButton,SIGNAL(clicked()),this, SLOT(accept()));
}

void Py2YacsDialog::onFunctionNamesChange(std::list<std::string> validFunctionNames)
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

void Py2YacsDialog::onLoad()
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
    
    model.loadFile(fileName.toStdString());
    _saveButton->setEnabled(model.savePossible());
    checkModel();
  }
}

void Py2YacsDialog::onExport()
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
    
    model.exportToXml(fileName.toStdString());
  }
}

void Py2YacsDialog::onApply()
{
  model.setScript(_pyEditor->toPlainText().toStdString());
  checkModel();
}

void Py2YacsDialog::invalidModel()
{
  _okButton->setEnabled(false);
  _exportButton->setEnabled(false);
  _functionChosen->setEnabled(false);
}

void Py2YacsDialog::checkModel()
{
  bool modelState = model.schemaAvailable();
  _okButton->setEnabled(modelState);
  _exportButton->setEnabled(modelState);
  _functionChosen->setEnabled(modelState);
}

void Py2YacsDialog::onSave()
{
  model.setScript(_pyEditor->toPlainText().toStdString());
  model.save();
  checkModel();
}

void Py2YacsDialog::onSaveAs()
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
    
    model.setScript(_pyEditor->toPlainText().toStdString());
    model.saveAs(fileName.toStdString());
    _saveButton->setEnabled(model.savePossible());
    checkModel();
  }
}

YACS::ENGINE::Proc* Py2YacsDialog::getYacsSchema()
{
  return model.getProc();
}