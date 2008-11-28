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
#ifndef YACSGui_Executor_HeaderFile
#define YACSGui_Executor_HeaderFile

#include <Proc.hxx>
#include <Executor.hxx>

#include <YACSGui_Graph.h>

#include <qobject.h>
#include <qthread.h>
#include <yacsgui.hh>

#include <list>
#include <string>

class YACSGui_Module;
class Observer_i;

class YACSGui_Executor : public QObject, public QThread
{
  Q_OBJECT

 public:
  YACSGui_Executor(YACSGui_Module* guiMod, YACS::ENGINE::Proc* theProc);
  ~YACSGui_Executor();
  
  void runDataflow(const bool isRemoteRun = false);
  bool checkEndOfDataFlow(bool display = true);
  void killDataflow();
  void suspendResumeDataflow();
  void suspendDataflow();
  void resumeDataflow();
  void stopDataflow();
  
  void setStepByStepMode();
  void setContinueMode();
  void setBreakpointMode();
  void setStopOnError(bool aMode);
  void unsetStopOnError();
  void saveState(const std::string& xmlFile);

  void setLoadStateFile(std::string xmlFile);
  
  YACS_ORB::executionMode getCurrentExecMode();
  int getExecutorState();
  
  void setBreakpointList(std::list<std::string> breakpointList);
  void setNextStepList(std::list<std::string> nextStepList);

  void setGraph(YACSGui_Graph* theGraph) { _graph = theGraph; }
  YACSGui_Graph* getGraph() const { return _graph; }
  void registerStatusObservers();
  bool isRunning() const { return _isRunning; };
  bool isStopOnError() const { return _isStopOnError; }

  void setEngineRef(YACS_ORB::YACS_Gen_ptr theRef);

  //YACS::ENGINE::Executor* getLocalEngine() { return _localEngine; };
  YACS::ENGINE::Proc* getProc() { return _proc; };
  std::string getErrorDetails(YACS::ENGINE::Node* node);
  std::string getErrorReport(YACS::ENGINE::Node* node);
  std::string getContainerLog();
  std::string getContainerLog(YACS::ENGINE::Node* node);
 protected:
  virtual void run();
  void _setBPList();

 private:
  YACS::ENGINE::Executor* _localEngine;
  YACS::ENGINE::Proc* _proc;
  YACS_ORB::YACS_Gen_var _engineRef;
  YACS_ORB::ProcExec_var _procRef;
  YACS_ORB::Observer_var _observerRef;
  YACS::ExecutionMode _execMode;
  Observer_i* _serv;
  YACSGui_Graph* _graph;
  YACSGui_Module* _guiMod;
  bool _isRemoteRun;
  bool _isRunning;
  bool _isSuspended;
  bool _isStopOnError;
  std::list<std::string> _breakpointList;
  std::string _loadStateFile;
};

#endif
