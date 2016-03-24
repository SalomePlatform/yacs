// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
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

#ifndef _GUIEXECUTOR_HXX_
#define _GUIEXECUTOR_HXX_

#include "define.hxx"

#include <QObject>
#include <string>
#include <list>


#include <yacsgui.hh>

namespace YACS
{
  namespace ENGINE
  {
    class Proc;
    class Node;
    class DataPort;
  }

  namespace HMI
  {
    class GuiObserver_i;
    class QtGuiContext;

    class GuiExecutor: public QObject
    {
    public:
      GuiExecutor(YACS::ENGINE::Proc* proc);
      virtual ~GuiExecutor();

      void closeContext();

      void startResumeDataflow(bool initialize = false);
      bool checkEndOfDataflow(bool display = true);
      void killDataflow();
      void suspendDataflow();
      void resumeDataflow();
      void stopDataflow();
      void resetDataflow();
  
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
      void addBreakpoint(std::string breakpoint);
      void removeBreakpoint(std::string breakpoint);
      void setNextStepList(std::list<std::string> nextStepList);

      void setInPortValue(YACS::ENGINE::DataPort* port, std::string value);

      void registerStatusObservers();
      bool isRunning() const { return _isRunning; };
      bool isStopOnError() const { return _isStopOnError; }

      void setEngineRef(YACS_ORB::YACS_Gen_ptr ref);
      YACS::ENGINE::Proc* getProc() { return _proc; };
      std::string getErrorDetails(YACS::ENGINE::Node* node);
      std::string getErrorReport(YACS::ENGINE::Node* node);
      std::string getContainerLog();
      std::string getContainerLog(YACS::ENGINE::Node* node);
      void shutdownProc();
      void setShutdownLevel(int level){_shutdownLevel=level;}
      int getShutdownLevel(){return _shutdownLevel;}
      
      YACS::ExecutorState updateSchema(std::string jobState);

      virtual bool event(QEvent *e);

    protected:
      void setBPList();

      YACS::ENGINE::Proc* _proc;
      QtGuiContext *_context;

      YACS_ORB::YACS_Gen_var _engineRef;
      YACS_ORB::ProcExec_var _procRef;
      YACS_ORB::Observer_var _observerRef;

      YACS::ExecutionMode _execMode;
      GuiObserver_i* _serv;

      bool _isRunning;
      bool _isSuspended;
      bool _isStopOnError;
      int _shutdownLevel;

      std::list<std::string> _breakpointList;
      std::string _loadStateFile;
    };
  }
}

#endif
