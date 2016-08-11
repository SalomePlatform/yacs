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

#include "YDFXGUIPushButtons.hxx"

#include "YDFXGUIWrap.hxx"
#include "YDFXGUISeqInit.hxx"
#include "YDFXGUIPyThreadSaver.hxx"
#include "YDFXGUIHostParametrizer.hxx"

#include "YACSEvalSession.hxx"
#include "YACSEvalObserver.hxx"
#include "YACSEvalResource.hxx"
#include "YACSEvalExecParams.hxx"

#include "Exception.hxx"

#include <QPainter>
#include <QVBoxLayout>
#include <QApplication>
#include <QStackedLayout>
#include <QDialogButtonBox>

/////////////

YDFXGUIPushButton1::YDFXGUIPushButton1(QWidget *parent, YACSEvalYFXWrap *efx, YDFXGUIAllPorts *ports):QPushButton(parent),_efx(efx),_ports(ports)
{
  setEnabled(false);
}

/////////////

YDFXGUIResourcePushButton::YDFXGUIResourcePushButton(QWidget *parent, YACSEvalYFXWrap *efx, YDFXGUIAllPorts *ports):YDFXGUIPushButton1(parent,efx,ports)
{
  setText("Assign Resources");
  connect(this,SIGNAL(clicked(bool)),this,SLOT(resourceAssignmentRequested()));
  connect(efx,SIGNAL(runSignal(bool)),this,SLOT(setHidden(bool)));
}

void YDFXGUIResourcePushButton::resourceAssignmentRequested()
{
  QDialog dial(this);
  QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(dial.sizePolicy().hasHeightForWidth());
  dial.setSizePolicy(sizePolicy);
  QVBoxLayout *vbox(new QVBoxLayout(&dial));
  YDFXGUIHostParametrizer *param(new YDFXGUIHostParametrizer(&dial,_efx));
  QObject::connect(param->getDialogButtonBox(),SIGNAL(accepted()),&dial,SLOT(accept()));
  QObject::connect(param->getDialogButtonBox(),SIGNAL(rejected()),&dial,SLOT(reject()));
  vbox->addWidget(param);
  param->loadFromSettings(_settings);
  if(dial.exec())
    {
      param->applyToEFX();
      param->learnSettings(_settings);
    }
}

/////////////

YDFXGUISeqInitButton::YDFXGUISeqInitButton(QWidget *parent, YACSEvalYFXWrap *efx, YDFXGUIAllPorts *ports):YDFXGUIPushButton1(parent,efx,ports)
{
  setText("Init sequences");
  connect(this,SIGNAL(clicked(bool)),this,SLOT(sequenceInitRequested()));
  connect(efx,SIGNAL(runSignal(bool)),this,SLOT(setHidden(bool)));
}

void YDFXGUISeqInitButton::sequenceInitRequested()
{
  QDialog dial(this);
  YDFXGUISeqInit *zeWidget(new YDFXGUISeqInit(&dial,_efx));
  zeWidget->loadState(_state);
  QVBoxLayout *mainLayout(new QVBoxLayout(&dial));
  mainLayout->addWidget(zeWidget);
  QObject::connect(zeWidget,SIGNAL(assignButtonClicked()),&dial,SLOT(accept()));
  if(dial.exec())
    {
      _state=zeWidget->saveState();
      emit sequenceWellDefined(true);
    }
}

/////////////

YDFXGUIRunInfo::YDFXGUIRunInfo(QObject *parent, int nbOfItems):QObject(parent),_computationInProgress(false),_items(nbOfItems,0)
{
}

void YDFXGUIRunInfo::startComputation()
{
  _mut.lock();
  std::fill(_items.begin(),_items.end(),0);
  _computationInProgress=true;
  _mut.unlock();
  emit somethingChanged();
}

void YDFXGUIRunInfo::endComputation()
{
  _mut.lock();
  _computationInProgress=false;
  _mut.unlock();
  emit somethingChanged();
}

void YDFXGUIRunInfo::sampleOK(int pos)
{
  _mut.lock();
  _items[pos]=1;
  _mut.unlock();
  emit somethingChanged();
}

void YDFXGUIRunInfo::sampleKO(int pos)
{
  _mut.lock();
  _items[pos]=2;
  _mut.unlock();
  emit somethingChanged();
}

std::vector<char> YDFXGUIRunInfo::getItems() const
{
  std::vector<char> ret;
  _mut.lock();
  ret=_items;
  _mut.unlock();
  return ret;
}

int YDFXGUIRunInfo::getNbOfItems() const
{
  _mut.lock();
  int ret(_items.size());
  _mut.unlock();
  return ret;
}

void YDFXGUIRunInfo::setNbOfItems(int nbOfItems)
{
  _items.resize(nbOfItems);
  std::fill(_items.begin(),_items.end(),0);
}

bool YDFXGUIRunInfo::getComputationStatus() const
{
  _mut.lock();
  bool ret(_computationInProgress);
  _mut.unlock();
  return ret;
}

/////////////

class MyObserver : public YACSEvalObserver
{
public:
  MyObserver(YDFXGUIRunInfo *info):_info(info) { }
  void startComputation(YACSEvalYFX *sender)
  {
    //std::cerr << " Start ! " << _info->getNbOfItems() << std::endl;
  }
  void notifySampleOK(YACSEvalYFX *sender, int sampleId)
  {
    _info->sampleOK(sampleId);
    //std::cerr << "sample OK = " << sampleId << std::endl;
  }
  void notifySampleKO(YACSEvalYFX *sender, int sampleId)
  {
    _info->sampleKO(sampleId);
    //std::cerr << "sample KO = " << sampleId << std::endl;
  }
private:
  YDFXGUIRunInfo *_info;
};

/////////////

YDFXGUIRunThread::YDFXGUIRunThread(QObject *parent, YACSEvalYFXWrap *efx, YACSEvalSession *session, YDFXGUIRunInfo *info):QThread(parent),_efx(efx),_session(session),_info(info),_ret0(false),_ret1(-1)
{
  _session->setForcedPyThreadSavedStatus(true);
}

void YDFXGUIRunThread::run()
{
  YDFXGUIPyThreadSaver::SaveContext(this);
  _efx->getParams()->setStopASAPAfterErrorStatus(false);
  MyObserver *obs(new MyObserver(_info));
  _efx->registerObserver(obs);
  obs->decrRef();
  _ret0=_efx->run(_session,_ret1);
}

YDFXGUIRunThread::~YDFXGUIRunThread()
{
}

bool YDFXGUIRunThread::getReturns(int& ret1) const
{
  ret1=_ret1;
  return _ret0;
}

/////////////

YDFXGUIRunningButton::YDFXGUIRunningButton(QWidget *parent):QPushButton(parent)
{
  setText("Run !");
  int dummy;
  YACSEvalYFXWrap *efx(getEFX());
  setEnabled(ComputeStateRes(efx,dummy) && ComputeStateSeq(efx));
  connect(this,SIGNAL(clicked()),this,SLOT(runWizardSlot()));
}

YACSEvalYFXWrap *YDFXGUIRunningButton::getEFX()
{
  YDFXGUIRunButton *parentc(qobject_cast<YDFXGUIRunButton *>(parent()));
  if(!parentc)
    return 0;
  return parentc->getEFX();
}

YDFXGUIRunInfo *YDFXGUIRunningButton::getInfo()
{
  YDFXGUIRunButton *parentc(qobject_cast<YDFXGUIRunButton *>(parent()));
  if(!parentc)
    return 0;
  return parentc->getInfo();
}

YACSEvalSession *YDFXGUIRunningButton::getSess()
{
  YDFXGUIRunButton *parentc(qobject_cast<YDFXGUIRunButton *>(parent()));
  if(!parentc)
    return 0;
  return parentc->getSess();
}

YDFXGUIRunThread *YDFXGUIRunningButton::getThread()
{
  YDFXGUIRunButton *parentc(qobject_cast<YDFXGUIRunButton *>(parent()));
  if(!parentc)
    return 0;
  return parentc->getThread();
}

/////////////

YDFXGUIMachineDialog::YDFXGUIMachineDialog(QWidget *parent):QDialog(parent)
{
}

void YDFXGUIMachineDialog::interactivityStatusChanged(bool)
{
  QSize sz1(_wid->minimumSizeHint()),sz2(_wid->sizeHint());
  QMargins marg(layout()->contentsMargins());
  int delta(marg.top()+marg.bottom());
  setMinimumHeight(sz1.height()+delta);
  setMaximumHeight(sz2.height()+delta);
}

/////////////

void YDFXGUIRunningButton::runWizardSlot()
{
  YDFXGUIMachineDialog dial(this);
  QGridLayout *mainLayout(new QGridLayout(&dial));
  YDFXGUIHostParametrizer *zeWidget(new YDFXGUIHostParametrizer(&dial,getEFX()));
  dial.setWidget(zeWidget);
  dial.interactivityStatusChanged(true);
  connect(zeWidget,SIGNAL(interactivityChanged(bool)),&dial,SLOT(interactivityStatusChanged(bool)));
  mainLayout->addWidget(zeWidget);
  connect(zeWidget->getDialogButtonBox(),SIGNAL(accepted()),&dial,SLOT(accept()));
  connect(zeWidget->getDialogButtonBox(),SIGNAL(rejected()),&dial,SLOT(reject()));
  if(dial.exec())
    {
      getEFX()->lockPortsForEvaluation();
      zeWidget->applyToEFX();
      getInfo()->setNbOfItems(getEFX()->getNbOfItems());
      runEvaluation();
    }
}

void YDFXGUIRunningButton::runEvaluation()
{
  if(!getSess()->isLaunched())
    getSess()->launch();
  if(!PyEval_ThreadsInitialized())
    PyEval_InitThreads();
  connect(getThread(),SIGNAL(finished()),this,SLOT(evaluationFinished()));
  setEnabled(false);
  getThread()->start();
}

void YDFXGUIRunningButton::evaluationFinished()
{
  YACSEvalYFXWrap *efx(getEFX());
  int dummy;
  setEnabled(ComputeStateRes(efx,dummy) && ComputeStateSeq(efx));
}

bool YDFXGUIRunningButton::ComputeStateRes(YACSEvalYFXWrap *efx, int& nbOfSamples)
{
  return efx->computeSequencesStatus(nbOfSamples);
}

bool YDFXGUIRunningButton::ComputeStateSeq(YACSEvalYFXWrap *efx)
{
  bool isOKForDef(true);
  try
    {
      efx->giveResources()->checkOKForRun();
    }
  catch(YACS::Exception& e)
    {
      isOKForDef=false;
    }
  return isOKForDef;
}

/////////////////////

YDFXGUIRunningPB::YDFXGUIRunningPB(QWidget *parent, YDFXGUIRunInfo *info):QWidget(parent),_info(info)
{
}

void YDFXGUIRunningPB::paintEvent(QPaintEvent *event)
{
  const int SZ_OF_PEN_RECT=2;
  QPainter painter(this);
  QRect refRect(rect());
  QRect refRect2(refRect);//event->rect());
  painter.setPen(QPen(Qt::black,SZ_OF_PEN_RECT,Qt::SolidLine));//,Qt::RoundCap));
  refRect2.translate(SZ_OF_PEN_RECT,SZ_OF_PEN_RECT);
  refRect2.setWidth(refRect2.width()-2*SZ_OF_PEN_RECT);;
  refRect2.setHeight(refRect2.height()-2*SZ_OF_PEN_RECT);
  painter.drawRect(refRect2);
  //
  std::vector<char> items(_info->getItems());
  int nbOfItems(items.size());
  //
  float xFact(float(refRect.width()-3.5*SZ_OF_PEN_RECT)/float(nbOfItems));
  //
  QPalette pal(QApplication::palette("QPushButton"));
  QColor color(pal.color(QPalette::Window)),color2;
  painter.setPen(QPen(color,0));
  for(int ii=0;ii<nbOfItems;)
    {
      for(;ii<nbOfItems && items[ii]==0;ii++);
      if(ii==nbOfItems)
        continue;
      int ref(items[ii]),start(ii);
      for(;ii<nbOfItems && items[ii]==ref;ii++);
      int endd(ii);
      if(ref==1)
        color2=Qt::green;
      else
        color2=Qt::red;
      QBrush brush(color2);
      painter.setBrush(brush);
      painter.setPen(QPen(color2,0));
      float xs(float(start)*xFact+1.5f*SZ_OF_PEN_RECT);
      float xe(float(endd)*xFact+1.5f*SZ_OF_PEN_RECT);
      painter.drawRect(QRectF(xs,1.5f*SZ_OF_PEN_RECT,xe-xs,refRect.height()-3.5f*SZ_OF_PEN_RECT));
    }
  int nbOfEltsDone(nbOfItems-std::count(items.begin(),items.end(),0));
  QString txt(QString("%1/%2 (%3%)").arg(nbOfEltsDone).arg(nbOfItems).arg(float(nbOfEltsDone)/float(nbOfItems)*100.f,0,'f',0));
  QFont ft(font());
  ft.setBold(true);
  QFontMetrics fm(ft);
  QSize refRect3(fm.boundingRect(txt).size());
  painter.setFont(ft);
  painter.setPen(QPen(Qt::black));
  painter.drawText(QPoint((refRect.width()-refRect3.width())/2,refRect.height()/2+refRect3.height()/2-fm.descent()),txt);
}

QSize YDFXGUIRunningPB::sizeHint() const
{
  int width(3*_info->getNbOfItems());
  width=std::max(width,50);
  width=std::min(width,150);
  return QSize(width,15);
}

QSize YDFXGUIRunningPB::minimumSizeHint() const
{
  int width(3*_info->getNbOfItems());
  width=std::max(width,50);
  width=std::min(width,120);
  return sizeHint();
}

/////////////

YDFXGUIRunButton::YDFXGUIRunButton(QWidget *parent, YACSEvalSession *session, YACSEvalYFXWrap *efx):QWidget(parent),_info(new YDFXGUIRunInfo(this,0)),_th(new YDFXGUIRunThread(this,efx,session,_info)),_push(new YDFXGUIRunningButton(this)),_pb(new YDFXGUIRunningPB(this,_info))
{
  QVBoxLayout *mainLayout(new QVBoxLayout(this));
  mainLayout->addWidget(_pb);
  mainLayout->addWidget(_push);
  _pb->hide();
  _push->setEnabled(false);
  //
  connect(getThread(),SIGNAL(started()),_info,SLOT(startComputation()));
  connect(getThread(),SIGNAL(finished()),_info,SLOT(endComputation()));
  //
  connect(_info,SIGNAL(somethingChanged()),this,SLOT(update()));
  connect(_info,SIGNAL(somethingChanged()),_pb,SLOT(update()));
}

QSize YDFXGUIRunButton::sizeHint() const
{
  QSize sz,sz2(QWidget::sizeHint());
  if(_info->getComputationStatus())
    sz=_pb->sizeHint();
  else
    sz=_push->sizeHint();
  sz.rwidth()+=sz2.rwidth();
  sz.rheight()+=sz2.rheight();
  return sz;
}

QSize YDFXGUIRunButton::minimumSizeHint() const
{
  QSize sz,sz2(QWidget::minimumSizeHint());
  if(_info->getComputationStatus())
    sz=_pb->minimumSizeHint();
  else
    sz=_push->minimumSizeHint();
  sz.rwidth()+=sz2.rwidth();
  sz.rheight()+=sz2.rheight();
  return sz;
}

void YDFXGUIRunButton::update()
{
  bool stat(_info->getComputationStatus());
  _pb->setVisible(stat);
  _push->setHidden(stat);
  QWidget::update();
}

void YDFXGUIRunButton::setEnabled(bool status)
{
  _push->setEnabled(status);
  QWidget::setEnabled(status);
}

void YDFXGUIRunButton::setDisabled(bool status)
{
  _push->setDisabled(status);
  QWidget::setEnabled(status);
}
