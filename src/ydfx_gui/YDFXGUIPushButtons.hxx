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

#ifndef __YDFXGUIPUSHBUTTONS_HXX__
#define __YDFXGUIPUSHBUTTONS_HXX__

#include "ydfxwidgetsExport.hxx"

#include <QPushButton>
#include <QSettings>
#include <QDialog>
#include <QThread>
#include <QMutex>

class YACSEvalSession;
class YACSEvalYFXWrap;
class YDFXGUIAllPorts;

class YDFXGUIPushButton1 : public QPushButton
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUIPushButton1(QWidget *parent, YACSEvalYFXWrap *efx, YDFXGUIAllPorts *ports);
protected:
  YACSEvalYFXWrap *_efx;
  YDFXGUIAllPorts *_ports;
};

class YDFXGUIResourcePushButton : public YDFXGUIPushButton1
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUIResourcePushButton(QWidget *parent, YACSEvalYFXWrap *efx, YDFXGUIAllPorts *ports);
public slots:
  void resourceAssignmentRequested();
private:
  QSettings _settings;
};

class YDFXGUISeqInitButton : public YDFXGUIPushButton1
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUISeqInitButton(QWidget *parent, YACSEvalYFXWrap *efx, YDFXGUIAllPorts *ports);
public slots:
  void sequenceInitRequested();
signals:
  void sequenceWellDefined(bool);
private:
  QMap<QString,QString> _state;
};

class YDFXGUIMachineDialog : public QDialog
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUIMachineDialog(QWidget *wid);
  YDFXWIDGETS_EXPORT void setWidget(QWidget *wid) { _wid=wid; }
public slots:
  void interactivityStatusChanged(bool newStatus);
private:
  QWidget *_wid;
};

class YDFXGUIRunInfo : public QObject
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUIRunInfo(QObject *parent, int nbOfItems);
  YDFXWIDGETS_EXPORT std::vector<char> getItems() const;
  YDFXWIDGETS_EXPORT int getNbOfItems() const;
  YDFXWIDGETS_EXPORT void setNbOfItems(int nbOfItems);
  YDFXWIDGETS_EXPORT bool getComputationStatus() const;
public slots:
  YDFXWIDGETS_EXPORT void startComputation();
  YDFXWIDGETS_EXPORT void endComputation();
  YDFXWIDGETS_EXPORT void sampleOK(int);
  YDFXWIDGETS_EXPORT void sampleKO(int);
signals:
  void somethingChanged();
private:
  bool _computationInProgress;
  std::vector<char> _items;
  mutable QMutex _mut;
};

class YDFXGUIRunThread : public QThread
{
public:
  YDFXWIDGETS_EXPORT YDFXGUIRunThread(QObject *parent, YACSEvalYFXWrap *efx, YACSEvalSession *session, YDFXGUIRunInfo *info);
  YDFXWIDGETS_EXPORT ~YDFXGUIRunThread();
  YDFXWIDGETS_EXPORT bool getReturns(int& ret1) const;
  YDFXWIDGETS_EXPORT YACSEvalYFXWrap *getEFX() const { return _efx; }
  YDFXWIDGETS_EXPORT YACSEvalSession *getSess() const { return _session; }
public:
  YDFXWIDGETS_EXPORT void run();
private:
  YACSEvalYFXWrap *_efx;
  YACSEvalSession *_session;
  YDFXGUIRunInfo *_info;
  bool _ret0;
  int _ret1;
};

class YDFXGUIRunningButton : public QPushButton
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUIRunningButton(QWidget *parent);
  YDFXWIDGETS_EXPORT YACSEvalYFXWrap *getEFX();
  YDFXWIDGETS_EXPORT YDFXGUIRunInfo *getInfo();
  YDFXWIDGETS_EXPORT YACSEvalSession *getSess();
  YDFXWIDGETS_EXPORT YDFXGUIRunThread *getThread();
public slots:
  YDFXWIDGETS_EXPORT void runWizardSlot();
  YDFXWIDGETS_EXPORT void evaluationFinished();
private:
  void runEvaluation();
private:
  static bool ComputeStateRes(YACSEvalYFXWrap *efx, int& nbOfSamples);
  static bool ComputeStateSeq(YACSEvalYFXWrap *efx);
};

class YDFXGUIRunningPB : public QWidget
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUIRunningPB(QWidget *parent, YDFXGUIRunInfo *info);
  YDFXWIDGETS_EXPORT void startComputation();
  YDFXWIDGETS_EXPORT void paintEvent(QPaintEvent *event);
  YDFXWIDGETS_EXPORT QSize sizeHint() const;
  YDFXWIDGETS_EXPORT QSize minimumSizeHint() const;
private:
  YDFXGUIRunInfo *_info;
};

class YDFXGUIRunButton : public QWidget
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUIRunButton(QWidget *parent, YACSEvalSession *session, YACSEvalYFXWrap *efx);
  YDFXWIDGETS_EXPORT QSize sizeHint() const;
  YDFXWIDGETS_EXPORT QSize minimumSizeHint() const;
  YDFXWIDGETS_EXPORT YACSEvalYFXWrap *getEFX() { return _th->getEFX(); }
  YDFXWIDGETS_EXPORT YDFXGUIRunInfo *getInfo() { return _info; }
  YDFXWIDGETS_EXPORT YACSEvalSession *getSess() { return _th->getSess(); }
  YDFXWIDGETS_EXPORT YDFXGUIRunThread *getThread() { return _th; }
  YDFXWIDGETS_EXPORT YDFXGUIRunningButton *getPush() { return _push; }
public slots:
  YDFXWIDGETS_EXPORT void update();
  YDFXWIDGETS_EXPORT void setEnabled(bool);
  YDFXWIDGETS_EXPORT void setDisabled(bool);
private:
  YDFXGUIRunInfo *_info;
  YDFXGUIRunThread *_th;
  YDFXGUIRunningButton *_push;
  YDFXGUIRunningPB *_pb;
};

#endif
