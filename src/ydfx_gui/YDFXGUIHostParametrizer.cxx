// Copyright (C) 2016  CEA/DEN, EDF R&D
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
// Author : Anthony Geay (EDF R&D)

#include "YDFXGUIHostParametrizer.hxx"

#include "YDFXGUIWrap.hxx"

#include "YACSEvalResource.hxx"

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSettings>
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>

#include <iostream>

const char YDFXGUIHostParametrizer::MACHINE[]="machine/machine";

const char YDFXGUIBatchInfo::NBPROCS[]="machine/nbprocs";

const char YDFXGUIBatchInfo::REMOTEDIR[]="machine/remotedir";

const char YDFXGUIBatchInfo::LOCALDIR[]="machine/localdir";

const char YDFXGUIBatchInfo::WCKEY[]="machine/wckey";

const char YDFXGUIBatchInfo::MAXDUR[]="machine/maxdur";

YDFXGUIHostParametrizer::YDFXGUIHostParametrizer(QWidget *parent, YACSEvalYFXWrap *efx):QWidget(parent),_hostSelector(0),_clusterAdvInfo(0),_buttonBox(0),_efx(efx),_wasInteractive(true)
{
  _efx->lockPortsForEvaluation();
  YACSEvalListOfResources *res(_efx->giveResources());
  std::vector<std::string> machines(res->getAllFittingMachines());
  /////////////
  QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Maximum);
  sizePolicy1.setHorizontalStretch(0);
  sizePolicy1.setVerticalStretch(0);
  sizePolicy1.setHeightForWidth(sizePolicy().hasHeightForWidth());
  QVBoxLayout *verticalLayout_3(new QVBoxLayout(this));
  QHBoxLayout *horizontalLayout_2(new QHBoxLayout);
  QLabel *label(new QLabel(this));
  QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Maximum);
  sizePolicy2.setHorizontalStretch(0);
  sizePolicy2.setVerticalStretch(0);
  sizePolicy2.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
  label->setText("Select machine");
  horizontalLayout_2->addWidget(label);
  _hostSelector=new QComboBox(this);
  int i(0);
  foreach(std::string mach,machines)
    _hostSelector->insertItem(i++,mach.c_str());
  connect(_hostSelector,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(changeMachine(const QString &)));
  sizePolicy2.setHeightForWidth(_hostSelector->sizePolicy().hasHeightForWidth());
  horizontalLayout_2->addWidget(_hostSelector);
  verticalLayout_3->addLayout(horizontalLayout_2,1);
  //
  _clusterAdvInfo=new YDFXGUIBatchInfo(this,_efx);
  verticalLayout_3->addWidget(_clusterAdvInfo,1);
  connect(_clusterAdvInfo,SIGNAL(statusOfEntryChanged(bool)),this,SLOT(clusterAdvParamStatusChanged(bool)));
  //
  QHBoxLayout *horizontalLayout_3(new QHBoxLayout);
  _buttonBox=new QDialogButtonBox(this);
  _buttonBox->setOrientation(Qt::Horizontal);
  _buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
  horizontalLayout_3->addWidget(_buttonBox,0);
  verticalLayout_3->addLayout(horizontalLayout_3,0);
  //
  _hostSelector->setCurrentIndex(0);
  {
    YACSEvalListOfResources *res(_efx->giveResources());
    _wasInteractive=res->isMachineInteractive(machines[0]);
  }
  this->changeMachine(QString(machines[0].c_str()));
}

void YDFXGUIHostParametrizer::loadFromSettings(const QSettings& settings)
{
  if(!settings.contains(MACHINE))
    return ;
  QVariant v(settings.value(MACHINE));
  QString vs(v.toString());
  if(vs.isEmpty())
    return ;
  int id(_hostSelector->findText(vs));
  if(id==-1)
    return ;
  _hostSelector->setCurrentIndex(id);
  YACSEvalListOfResources *res(_efx->giveResources());
  if(!res->isMachineInteractive(vs.toStdString()))
    _clusterAdvInfo->loadFromSettings(settings);
}

void YDFXGUIHostParametrizer::learnSettings(QSettings& settings) const
{
  settings.setValue(MACHINE,_hostSelector->currentText());
  YACSEvalListOfResources *res(_efx->giveResources());
  std::string selectedRes(_hostSelector->currentText().toStdString());
  if(!res->isMachineInteractive(selectedRes))//res->isInteractive() does not work here if efx has no Resource (no containers !)
    _clusterAdvInfo->learnSettings(settings);
}

void YDFXGUIHostParametrizer::applyToEFX()
{
  YACSEvalListOfResources *res(_efx->giveResources());
  std::string selectedRes(_hostSelector->currentText().toStdString());
  res->setWantedMachine(selectedRes);
  if(!res->isMachineInteractive(selectedRes))//res->isInteractive() does not work here if efx has no Resource (no containers !)
    {
      YACSEvalParamsForCluster& ps(res->getAddParamsForCluster());
      _clusterAdvInfo->applyToParams(ps);
    }
  emit readyForRunSignal(true);
}

QString YDFXGUIHostParametrizer::getNameOfHost()
{
  return _hostSelector->currentText();
}

void YDFXGUIHostParametrizer::changeMachine(const QString& newMachineSelected)
{
  YACSEvalListOfResources *res(_efx->giveResources());
  bool isInterac(res->isMachineInteractive(newMachineSelected.toStdString()));
  _clusterAdvInfo->setVisible(!isInterac);
  QPushButton *ok(_buttonBox->button(QDialogButtonBox::Ok));
  ok->setDisabled(!isInterac && !_clusterAdvInfo->isOK());
  if(_wasInteractive!=isInterac)
    {
      emit interactivityChanged(isInterac);
      _wasInteractive=isInterac;
    }
}

void YDFXGUIHostParametrizer::clusterAdvParamStatusChanged(bool newStatus)
{
  QPushButton *ok(_buttonBox->button(QDialogButtonBox::Ok));
  ok->setDisabled(!newStatus);
}

QValidator::State YDFXGUIDurationValidator::validate(QString &input, int &) const
{
  QString input2(input.simplified());
  input2.replace(' ',QString());
  QStringList sp(input2.split(QChar(':')));
  if(sp.size()>3)
    return QValidator::Invalid;
  QRegExp rx("\\d{1,2}");
  if(sp.size()==1)
    {
      if(input2.isEmpty())
        return QValidator::Intermediate;
      if(!rx.exactMatch(input2))
        return QValidator::Invalid;
      return QValidator::Intermediate;
    }
  if(sp.size()==2 && sp[1].isEmpty())
    {
      if(sp[0].isEmpty())
        return QValidator::Invalid;
      return QValidator::Intermediate;
    }
  if(sp.size()==2)
    sp.insert(0,QString("00"));
  if(sp[2].isEmpty())
    return QValidator::Intermediate;
  int hms(0);
  foreach(QString elt,sp)
    {
      if(!rx.exactMatch(elt))
        return QValidator::Invalid;
      bool isOK(false);
      uint val(elt.toUInt(&isOK));
      if(!isOK)
        return QValidator::Invalid;
      if(hms>1)
        if(val>=60)
          return QValidator::Invalid;
      hms++;
    }
  return QValidator::Acceptable;
}
 
YDFXGUIBatchInfo::YDFXGUIBatchInfo(QWidget *parent, YACSEvalYFXWrap *efx):QWidget(parent),_efx(efx),_nbProcs(0),_remoteDir(0),_localDir(0),_WCKey(0),_maxDuration(0),_wasOK(false)
{
  QVBoxLayout *verticalLayout(new QVBoxLayout(this));
  QGroupBox *gb(new QGroupBox(this));
  gb->setTitle("Specific info for cluster");
  verticalLayout->addWidget(gb);
  QVBoxLayout *verticalLayout_1(new QVBoxLayout(gb));
  QFrame *frame(new QFrame(gb));
  verticalLayout_1->addWidget(frame);
  frame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  frame->setFrameShadow(QFrame::Raised);
  QVBoxLayout *verticalLayout_2(new QVBoxLayout(frame));
  QSpacerItem *verticalSpacer(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
  verticalLayout_2->addItem(verticalSpacer);
  QHBoxLayout *horizontalLayout_3(0);
  //
  horizontalLayout_3=new QHBoxLayout;
  QLabel *label_5(new QLabel(frame));
  label_5->setText("Number of processes");
  horizontalLayout_3->addWidget(label_5);
  _nbProcs=new QLineEdit(frame);
  QValidator *nbProcVal(new QIntValidator(1,10000,_nbProcs));
  _nbProcs->setValidator(nbProcVal);
  horizontalLayout_3->addWidget(_nbProcs);
  verticalLayout_2->addLayout(horizontalLayout_3);
  verticalSpacer=new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
  verticalLayout_2->addItem(verticalSpacer);
  connect(_nbProcs,SIGNAL(textChanged(const QString&)),this,SLOT(somethingChanged()));
  //
  horizontalLayout_3=new QHBoxLayout;
  QLabel *label_2(new QLabel(frame));
  label_2->setText("Remote dir");
  horizontalLayout_3->addWidget(label_2);
  _remoteDir=new QLineEdit(frame);
  horizontalLayout_3->addWidget(_remoteDir);
  verticalLayout_2->addLayout(horizontalLayout_3);
  connect(_remoteDir,SIGNAL(textChanged(const QString&)),this,SLOT(somethingChanged()));
  //
  verticalSpacer=new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
  verticalLayout_2->addItem(verticalSpacer);
  //
  horizontalLayout_3=new QHBoxLayout;
  QLabel *label_3(new QLabel(frame));
  label_3->setText("Local dir");
  horizontalLayout_3->addWidget(label_3);
  _localDir=new QLineEdit(frame);
  horizontalLayout_3->addWidget(_localDir);
  connect(_localDir,SIGNAL(textChanged(const QString&)),this,SLOT(somethingChanged()));
  verticalLayout_2->addLayout(horizontalLayout_3);
  verticalSpacer=new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
  verticalLayout_2->addItem(verticalSpacer);
  //
  horizontalLayout_3=new QHBoxLayout;
  QLabel *label_4(new QLabel(frame));
  label_4->setText("Working Caracterization Key");
  horizontalLayout_3->addWidget(label_4);
  _WCKey=new QLineEdit(frame);
  connect(_WCKey,SIGNAL(textChanged(const QString&)),this,SLOT(somethingChanged()));
  horizontalLayout_3->addWidget(_WCKey);
  verticalLayout_2->addLayout(horizontalLayout_3);
  verticalSpacer=new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
  verticalLayout_2->addItem(verticalSpacer);
  //
  horizontalLayout_3=new QHBoxLayout;
  QLabel *label_6(new QLabel(frame));
  label_6->setText("Max duration");
  horizontalLayout_3->addWidget(label_6);
  _maxDuration=new QLineEdit(frame);
  QValidator *maxDurVal(new YDFXGUIDurationValidator(_maxDuration));
  _maxDuration->setText("05:00");
  _maxDuration->setValidator(maxDurVal);
  connect(_maxDuration,SIGNAL(textChanged(const QString&)),this,SLOT(somethingChanged()));
  horizontalLayout_3->addWidget(_maxDuration);
  verticalLayout_2->addLayout(horizontalLayout_3);
  verticalSpacer=new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
  verticalLayout_2->addItem(verticalSpacer);
}

void YDFXGUIBatchInfo::showEvent(QShowEvent *event)
{
  YDFXGUIHostParametrizer *parentc(qobject_cast<YDFXGUIHostParametrizer *>(parent()));
  if(parentc)
    {
      QPushButton *ok(parentc->getDialogButtonBox()->button(QDialogButtonBox::Ok));
      ok->setEnabled(isOK());
    }
  QWidget::showEvent(event);
}

bool YDFXGUIBatchInfo::isOK() const
{
  bool ret1(!_nbProcs->text().isEmpty() && !_remoteDir->text().isEmpty() && !_localDir->text().isEmpty() && !_WCKey->text().isEmpty());
  const QValidator *val(_maxDuration->validator());
  int dummy;
  QString st(_maxDuration->text());
  QValidator::State sta(val->validate(st,dummy));
  return ret1 && sta==QValidator::Acceptable;
}

void YDFXGUIBatchInfo::loadFromSettings(const QSettings& settings)
{
  static const int NB=5;
  const char *tab1[NB]={NBPROCS,REMOTEDIR,LOCALDIR,WCKEY,MAXDUR};
  QLineEdit *tab2[NB]={_nbProcs,_remoteDir,_localDir,_WCKey,_maxDuration};
  QString tab3[NB];
  YACSEvalParamsForCluster* cp(0);
  YACSEvalListOfResources *lr(_efx->giveResources());
  if(lr)
    {
      std::vector<std::string> machines(lr->getAllChosenMachines());
      if(machines.size()==1)
        {
          if(machines[0]==getNameOfHost().toStdString())
            cp=&lr->getAddParamsForCluster();
        }
    }
  if(cp)
    {
      tab3[0]=QString("%1").arg(cp->getNbProcs());
      tab3[1]=cp->getRemoteWorkingDir().c_str();
      tab3[2]=cp->getLocalWorkingDir().c_str();
      tab3[3]=cp->getWCKey().c_str();
      tab3[4]=cp->getMaxDuration().c_str();
    }
  for(int i=0;i<NB;i++)
    {
      const char *entry(tab1[i]);
      QLineEdit *le(tab2[i]);
      if(!tab3[1].isEmpty())
        {
          le->setText(tab3[i]);
          continue;
        }
      //
      if(!settings.contains(entry))
        continue ;
      QVariant v(settings.value(entry));
      QString vs(v.toString());
      if(vs.isEmpty())
        return ;
      le->setText(vs);
    }
  bool newOK(isOK());
  _wasOK=newOK;
  emit statusOfEntryChanged(newOK);
}

void YDFXGUIBatchInfo::learnSettings(QSettings& settings) const
{
  settings.setValue(NBPROCS,_nbProcs->text());
  settings.setValue(REMOTEDIR,_remoteDir->text());
  settings.setValue(LOCALDIR,_localDir->text());
  settings.setValue(WCKEY,_WCKey->text());
  settings.setValue(MAXDUR,_maxDuration->text());
}

void YDFXGUIBatchInfo::applyToParams(YACSEvalParamsForCluster& ps) const
{
  QString nbProcs(_nbProcs->text());
  ps.setNbProcs(nbProcs.toUInt());
  ps.setRemoteWorkingDir(_remoteDir->text().toStdString());
  ps.setLocalWorkingDir(_localDir->text().toStdString());
  ps.setWCKey(_WCKey->text().toStdString());
  ps.setMaxDuration(NormalizeDuration(_maxDuration->text()).toStdString());
}

QString YDFXGUIBatchInfo::getNameOfHost()
{
  YDFXGUIHostParametrizer *parentc(qobject_cast<YDFXGUIHostParametrizer *>(parent()));
  if(parentc)
    return parentc->getNameOfHost();
  else
    return QString();
}

void YDFXGUIBatchInfo::somethingChanged()
{
  emit statusOfEntryChanged(isOK());
}

QString YDFXGUIBatchInfo::NormalizeDuration(const QString& txt)
{
  QString input2(txt.simplified());
  input2.replace(' ',QString());
  QStringList sp(txt.split(QChar(':')));
  QStringList ret;
  foreach(QString elt,sp)
    {
      bool isOK(false);
      uint val(elt.toUInt(&isOK));
      if(!isOK)
        {
          ret << "00";
          continue;
        }
      ret << QString("%1").arg(val,2,10,QChar('0'));
    }
  return ret.join(QChar(':'));
}

