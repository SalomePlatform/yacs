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

#ifndef __YDFXGUIMAIN_HXX__
#define __YDFXGUIMAIN_HXX__

#include "ydfxwidgetsExport.hxx"

#include <QPushButton>
#include <QTabWidget>
#include <QSettings>
#include <QThread>
#include <QMutex>

#include "Python.h"

class YACSEvalSession;
class YACSEvalYFXWrap;
class YDFXGUIAllPorts;
class YDFXGUIRunButton;

class YDFXGUIMainEFXWidget : public QWidget
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUIMainEFXWidget(YACSEvalSession *session, YACSEvalYFXWrap *efx, QWidget *parent);
  YDFXWIDGETS_EXPORT ~YDFXGUIMainEFXWidget();
private:
  YACSEvalSession *_session;
  YACSEvalYFXWrap *_efx;//owned
  YDFXGUIAllPorts *_ports;
  QPushButton *_seqInit;
  YDFXGUIRunButton *_run;
};

class AddTabWidget : public QWidget
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT AddTabWidget(QWidget *parent);
signals:
  void addNewTab();
};

class TabEFXViews : public QTabWidget
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT TabEFXViews(QWidget *parent, YACSEvalSession *session);
public slots:
  YDFXWIDGETS_EXPORT void newTabFromXMLRequested();
  YDFXWIDGETS_EXPORT void closeTabPlease(int tabId);
private:
  AddTabWidget *_addWidget;
  YACSEvalSession *_session;
};

class YDFXGUI : public QWidget
{
public:
  YDFXWIDGETS_EXPORT YDFXGUI(YACSEvalSession *session);
private:
  TabEFXViews *_tabWidget;
};

#endif
