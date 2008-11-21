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

      void startResumeDataflow();
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

      void registerStatusObservers();
      bool isRunning() const { return _isRunning; };
      bool isStopOnError() const { return _isStopOnError; }

      void setEngineRef(YACS_ORB::YACS_Gen_ptr ref);
      YACS::ENGINE::Proc* getProc() { return _proc; };
      std::string getErrorDetails(YACS::ENGINE::Node* node);
      std::string getErrorReport(YACS::ENGINE::Node* node);
      std::string getContainerLog();
      std::string getContainerLog(YACS::ENGINE::Node* node);

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

      std::list<std::string> _breakpointList;
      std::string _loadStateFile;
    };
  }
}

#endif
