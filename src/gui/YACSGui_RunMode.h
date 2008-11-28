//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef _MYRUNMODE_HXX_
#define _MYRUNMODE_HXX_

#include "runmode.h"
#include "ComposedNode.hxx"
#include "Proc.hxx"
#include "define.hxx"

#include <qlistview.h>
#include <list>
#include <string>
#include <set>

class YACSGui_ComposedNodeViewItem;

class YACSGui_Executor;

class YACSGui_RunMode : public runMode
{
  Q_OBJECT

public:
  YACSGui_RunMode(YACSGui_Executor* guiExec,
                  QWidget* parent = 0,
                  const char* name = 0,
                  bool modal = FALSE,
                  WFlags fl = 0 );
  ~YACSGui_RunMode();

 public slots:
  virtual void onResume();
  virtual void onPause();
  virtual void onStop();
  virtual void onModeContinue();
  virtual void onModeStepByStep();
  virtual void onModeBreakpoints();
  virtual void onDismiss();
  virtual void onStopOnError();
  virtual void onSaveState();
  virtual void onAllNextToRun();
  virtual void onRemoveAllNextToRun();
  virtual void onBreakpointClicked(QListViewItem *item);
  virtual void onNextStepClicked(QListViewItem *item);

  virtual void onNotifyStatus(int status);
  virtual void onNotifyNodeStatus(int nodeId, int status);
  virtual void onNotifyNextSteps(std::list<std::string> nextSteps);

 protected:
  void resetTreeNode();
  void addTreeNode(YACSGui_ComposedNodeViewItem *parent,
                   YACS::ENGINE::ComposedNode* father);

  YACSGui_Executor* _guiExec;
  YACS::ENGINE::Proc* _proc;
  bool _saveState;
  std::map<int, QListViewItem*> _mapListViewItem;
  std::map<QCheckListItem*, std::string> _mapNextSteps;
  std::set<int> _breakpointSet;
};

#endif
