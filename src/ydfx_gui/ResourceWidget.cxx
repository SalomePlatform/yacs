// Copyright (C) 2017-2020  CEA/DEN, EDF R&D
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

#include "ResourceWidget.hxx"

ResourceWidget::ResourceWidget(AbstractResourceModel* model, QWidget* parent)
: QScrollArea(parent),
  _model(model),
  _clusterBox(0),
  _localdirEdit(0)
{
  QWidget* mainWidget = new QWidget();
  QVBoxLayout *mainLayout = new QVBoxLayout();
  mainWidget->setLayout(mainLayout);
  QGroupBox *resourcesBox = new QGroupBox();
  resourcesBox->setTitle(tr("Launching parameters"));
  mainLayout->addWidget(resourcesBox);
  QGridLayout *resourcesLayout = new QGridLayout();
  resourcesBox->setLayout(resourcesLayout);
  
  QCheckBox *parallelizeStatusCheckBox = new QCheckBox(tr("Parallelize status"));
  parallelizeStatusCheckBox->setChecked(_model->getParallelizeStatus());
  connect(parallelizeStatusCheckBox, SIGNAL(toggled(bool)),
          this, SLOT(updateParallelizeStatus(bool)));
  resourcesLayout->addWidget(parallelizeStatusCheckBox, 0, 0);
  
  QLabel * resourcesLabel = new QLabel(tr("Computing resource:"));
  resourcesLayout->addWidget(resourcesLabel, 1, 0);
  
  QComboBox* resourcesComboBox = new QComboBox;
  std::vector<std::string> resources = _model->getFittingMachines();
  std::vector<std::string>::const_iterator it;
  for(it=resources.begin(); it!=resources.end(); it++)
    resourcesComboBox->addItem(QString(it->c_str()));
  resourcesLayout->addWidget(resourcesComboBox, 1, 1);
  
  _clusterBox = createClusterWidgets();
  mainLayout->addWidget(_clusterBox);

  resourcesComboBox->setCurrentIndex(
                resourcesComboBox->findText(model->getWantedMachine().c_str()));

  connect(resourcesComboBox, SIGNAL(currentIndexChanged( const QString &)),
          this, SLOT(updateWantedMachine( const QString &)));

  _clusterBox->setVisible(!model->isMachineInteractive(
                                                    model->getWantedMachine()));
  mainLayout->addStretch();
  setWidget(mainWidget);
  setWidgetResizable (true);
}

ResourceWidget::~ResourceWidget()
{
}

QWidget * ResourceWidget::createClusterWidgets()
{
  // Infos for clusters
  QGroupBox *clusterBox = new QGroupBox();
  QGridLayout *groupLayout = new QGridLayout;
  clusterBox->setLayout(groupLayout);
  clusterBox->setTitle(tr("Specific parameters for clusters"));
  QHBoxLayout *hLayout;
  int pos = 0;

  QLabel *nbprocLabel = new QLabel();
  QSpinBox *nbprocEdit = new QSpinBox();
  nbprocLabel->setText(tr("Number of processes"));
  nbprocEdit->setRange(1, 10000);
  groupLayout->addWidget(nbprocLabel, pos, 0);
  groupLayout->addWidget(nbprocEdit, pos++, 1);
  connect(nbprocEdit, SIGNAL(valueChanged(int)), this, SLOT(updateNbprocs(int)));
  nbprocEdit->setValue(_model->getNbprocs());

  QLabel *remotedirLabel = new QLabel();
  _remotedirEdit = new QLineEdit();
  remotedirLabel->setText(tr("Remote working directory"));
  groupLayout->addWidget(remotedirLabel, pos, 0);
  groupLayout->addWidget(_remotedirEdit, pos++, 1);
  connect(_remotedirEdit, SIGNAL(textEdited(const QString &)),
          this, SLOT(updateRemoteDir(const QString &)));
  _remotedirEdit->setText(_model->getRemoteDir().c_str());

  QLabel *localdirLabel = new QLabel();
  _localdirEdit = new QLineEdit();
  QPushButton* localdirButton = new QPushButton("...");
  
  localdirLabel->setText(tr("Local result directory"));
  groupLayout->addWidget(localdirLabel, pos, 0);
  groupLayout->addWidget(_localdirEdit, pos, 1);
  groupLayout->addWidget(localdirButton, pos++, 2);
  connect(_localdirEdit, SIGNAL(textEdited(const QString &)),
          this, SLOT(updateLocalDir(const QString &)));
  connect(localdirButton,SIGNAL(clicked()),this, SLOT(onChoseLocaldir()));
  _localdirEdit->setText(_model->getLocalDir().c_str());
  
  QLabel *wckeyLabel = new QLabel();
  QLineEdit *wckeyEdit = new QLineEdit();
  wckeyLabel->setText(tr("Working Characterization Key"));
  groupLayout->addWidget(wckeyLabel, pos, 0);
  groupLayout->addWidget(wckeyEdit, pos++, 1);
  connect(wckeyEdit, SIGNAL(textEdited(const QString &)),
          this, SLOT(updateWckey(const QString &)));
  wckeyEdit->setText(_model->getWckey().c_str());
  
  hLayout = new QHBoxLayout();
  QLabel *timeLabel = new QLabel();
  QLabel *hoursLabel = new QLabel();
  QLabel *minutesLabel = new QLabel();
  QSpinBox *hoursEdit = new QSpinBox();
  QSpinBox *minutesEdit = new QSpinBox();
  hoursEdit->setRange(0, 99);
  minutesEdit->setRange(0,59);
  timeLabel->setText(tr("Time limit (0:0 for default values):"));
  hoursLabel->setText(tr("hours"));
  minutesLabel->setText(tr("minutes"));
  hLayout->addWidget(hoursEdit);
  hLayout->addWidget(hoursLabel);
  hLayout->addWidget(minutesEdit);
  hLayout->addWidget(minutesLabel);
  groupLayout->addWidget(timeLabel, pos, 0);
  groupLayout->addLayout(hLayout, pos++, 1);
  
  hoursEdit->setValue(_model->getMaxDurationHours());
  minutesEdit->setValue(_model->getMaxDurationMinutes());
  connect(hoursEdit, SIGNAL(valueChanged(int)),
          this, SLOT(updateMaxHours(int)));
  connect(minutesEdit, SIGNAL(valueChanged(int)),
          this, SLOT(updateMaxMinutes(int)));
  
  QGroupBox * inputFilesBox = new QGroupBox;
  inputFilesBox->setTitle(tr("Input files"));
  QHBoxLayout *inputFilesLayout = new QHBoxLayout;
  inputFilesBox->setLayout(inputFilesLayout);
  _inputFilesList = new QListWidget;
  _inputFilesList->setSelectionMode(QAbstractItemView::MultiSelection);
  std::list<std::string>::const_iterator it;
  for(it=_model->getInFiles().begin(); it!= _model->getInFiles().end(); it++)
  {
    _inputFilesList->addItem(it->c_str());
  }
  QVBoxLayout *inputButtonsLayout = new QVBoxLayout;
  QPushButton *addInputFilesButton = new QPushButton(tr("+"));
  _removeInputFilesButton = new QPushButton(tr("-"));
  inputButtonsLayout->addWidget(addInputFilesButton);
  inputButtonsLayout->addWidget(_removeInputFilesButton);
  inputFilesLayout->addWidget(_inputFilesList);
  inputFilesLayout->addLayout(inputButtonsLayout);
  groupLayout->addWidget(inputFilesBox, pos++, 0, 1, -1);
  _removeInputFilesButton->setEnabled(false);
  connect(_inputFilesList, SIGNAL(itemSelectionChanged()),
          this, SLOT(inputSelectionChanged()));
  connect(addInputFilesButton, SIGNAL(clicked()),
          this, SLOT(addInputFiles()));
  connect(_removeInputFilesButton, SIGNAL(clicked()),
          this, SLOT(removeInputFiles()));
  return clusterBox;
}

void ResourceWidget::updateParallelizeStatus(bool v)
{
  _model->setParallelizeStatus(v);
}

void ResourceWidget::updateWantedMachine( const QString& v)
{
  _model->setWantedMachine(v.toStdString());
  bool isRemote = !_model->isMachineInteractive(v.toStdString());
  _clusterBox->setVisible(isRemote);
  if(isRemote)
  {
    std::string defaultValue = _model->getDefaultRemoteDir(v.toStdString());
    _remotedirEdit->setText(defaultValue.c_str());
    _model->setRemoteDir(defaultValue);
  }
}

void ResourceWidget::updateNbprocs(int v)
{
  _model->setNbprocs(v);
}

void ResourceWidget::updateRemoteDir(const QString &v)
{
  _model->setRemoteDir(v.toStdString());
}

void ResourceWidget::updateLocalDir(const QString &v)
{
  _model->setLocalDir(v.toStdString());
}

void ResourceWidget::updateWckey(const QString &v)
{
  _model->setWckey(v.toStdString());
}

void ResourceWidget::updateMaxHours(int v)
{
  _model->setMaxDurationHours(v);
}

void ResourceWidget::updateMaxMinutes(int v)
{
  _model->setMaxDurationMinutes(v);
}

void ResourceWidget::onChoseLocaldir()
{
  QString dir;
  dir = QFileDialog::getExistingDirectory(this,
                                          tr("Choose local result directory"),
                                          "",
                                          QFileDialog::ShowDirsOnly
                                          | QFileDialog::DontResolveSymlinks);

  if (dir != "")
  {
    _model->setLocalDir(dir.toStdString());
    _localdirEdit->setText(dir);
  }
}

void ResourceWidget::inputSelectionChanged()
{
  bool canRemove = (_inputFilesList->selectedItems().size() > 0);
  _removeInputFilesButton->setEnabled(canRemove);
}

void ResourceWidget::addInputFiles()
{
  // We need to be able to select both files and folders.
  // Standard QFileDialog cannot do it.
  QFileDialog w(this, tr("Select input files"),
              "", tr("All files and folders(*)")) ;
  w.setOption(QFileDialog::DontUseNativeDialog,true);
  QListView *l = w.findChild<QListView*>("listView");
  if (l)
  {
    l->setSelectionMode(QAbstractItemView::MultiSelection);
  }
  QTreeView *t = w.findChild<QTreeView*>();
  if (t)
  {
    t->setSelectionMode(QAbstractItemView::MultiSelection);
  }
  if(w.exec())
  {
    QStringList fileNames = w.selectedFiles();
    for (int i = 0; i < fileNames.size(); ++i) 
    {
      QString fileName = fileNames.at(i);
      if (_inputFilesList->findItems(fileName,Qt::MatchFixedString).size() == 0)
      {
        _model->getInFiles().push_back(fileName.toStdString());
        _inputFilesList->addItem(fileName);
      }
    }
  }
}

void ResourceWidget::removeInputFiles()
{
  QList<QListWidgetItem *> list = _inputFilesList->selectedItems();
  for (int i = 0; i < list.size(); ++i)
  {
    int row = _inputFilesList->row( list.at(i) );
    QListWidgetItem * item = _inputFilesList->takeItem(row);
    std::string itemName = item->text().toStdString();
    _model->getInFiles().remove(itemName);
    delete item;
  }
}
