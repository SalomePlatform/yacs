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

#include <Python.h>
#include "SALOME_LifeCycleCORBA.hxx"
#include "SALOME_NamingService.hxx"

#include "GuiExecutor.hxx"
#include "GuiObserver_i.hxx"
#include "QtGuiContext.hxx"

#include "RuntimeSALOME.hxx"

#include "Proc.hxx"
#include "Node.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "Executor.hxx"


//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;
using namespace YACS::ENGINE;

GuiExecutor::GuiExecutor(YACS::ENGINE::Proc* proc)
{
  DEBTRACE("GuiExecutor::GuiExecutor");

  _proc = proc;
  _context = QtGuiContext::getQtCurrent();

  _engineRef = YACS_ORB::YACS_Gen::_nil();
  _procRef = YACS_ORB::ProcExec::_nil();
  _observerRef = YACS_ORB::Observer::_nil();

  _execMode = YACS::CONTINUE;
  _serv = 0;

  _isRunning = false;
  _isSuspended = false;
  _isStopOnError = false;
  _shutdownLevel=1;

  _loadStateFile = "";
  _breakpointList.clear();
}


GuiExecutor::~GuiExecutor()
{
  DEBTRACE("GuiExecutor::~GuiExecutor");
}

void GuiExecutor::closeContext()
{
  DEBTRACE("GuiExecutor::closeContext");
  _context = 0;
}

void GuiExecutor::startResumeDataflow(bool initialize)
{
  DEBTRACE("GuiExecutor::runDataflow " << initialize);
  if (!_context)
    {
      DEBTRACE("context closed");
      return;
    }
  if (CORBA::is_nil(_engineRef))
    {
      DEBTRACE("Create YACS ORB engine!");
      YACS::ENGINE::RuntimeSALOME* runTime = YACS::ENGINE::getSALOMERuntime();
      CORBA::ORB_ptr orb = runTime->getOrb();
      SALOME_NamingService namingService(orb);
      SALOME_LifeCycleCORBA lcc(&namingService);
      ostringstream containerName;
      containerName << "localhost/YACSContainer" << QtGuiContext::getQtCurrent()->getStudyId();
      Engines::EngineComponent_var comp = lcc.FindOrLoad_Component(containerName.str().c_str(), "YACS" );
      _engineRef =YACS_ORB::YACS_Gen::_narrow(comp);
      YASSERT(!CORBA::is_nil(_engineRef));
    }

  checkEndOfDataflow(); // --- to allow change of the _isRunning state

  if (_isRunning)
    {
      if (_isSuspended)
        {
          _procRef->setExecMode(getCurrentExecMode());
          _procRef->resumeCurrentBreakPoint();
          _isSuspended = false;
        }
      return;
    }

  _isRunning = true;

  if (CORBA::is_nil(_procRef))
    {
      DEBTRACE("init _procRef");
      _procRef = _engineRef->LoadProc(_context->getFileName().toUtf8());
      registerStatusObservers();
      DEBTRACE("_procRef init");
    }

  YASSERT(!CORBA::is_nil(_procRef));

  if (initialize)
    _procRef->setExecMode(YACS_ORB::STEPBYSTEP);
  else
    _procRef->setExecMode(getCurrentExecMode());
  setBPList();

  if (_loadStateFile.empty())
    {
      DEBTRACE("Run from scratch!");
      _procRef->Run();
    }
  else
    {
      DEBTRACE("Run from STATE!");
      try
        {
          _procRef->RunFromState(_loadStateFile.c_str());
        }
      catch (...)
        {
          DEBTRACE("Runtime error: execution from the loaded state failed")
          return;
        }
    }
}

bool GuiExecutor::checkEndOfDataflow(bool display)
{
  DEBTRACE("GuiExecutor::checkEndOfDataFlow");
  if (_isRunning)
    {
      if (CORBA::is_nil(_procRef))
        {
          DEBTRACE("Runtime error: connection lost on a running scheme");
          _isRunning = false;
          return false;              
        }
      if (_procRef->isNotFinished())
        {
          DEBTRACE("Remote Execution Already running...");
          return false;
        }
      else
        {
          _isRunning = false;
          // --- TODO: cleaning on server ...
        }
    }
  return true;
}

void GuiExecutor::killDataflow()
{
  DEBTRACE("GuiExecutor::killDataflow");
  if (_isRunning)
    _procRef->stopExecution();
}

void GuiExecutor::suspendDataflow()
{
  DEBTRACE("GuiExecutor::suspendDataflow");
  if (_isRunning)
    _procRef->setExecMode(YACS_ORB::STEPBYSTEP);
}

void GuiExecutor::resumeDataflow()
{
  DEBTRACE("GuiExecutor::resumeDataflow");
  if (_isRunning)
    {
      _procRef->setExecMode(getCurrentExecMode());
      _procRef->resumeCurrentBreakPoint();
    }
}

void GuiExecutor::stopDataflow()
{
  DEBTRACE("GuiExecutor::stopDataflow");
  if (_isRunning)
    _procRef->stopExecution();
}

void GuiExecutor::resetDataflow()
{
  DEBTRACE("GuiExecutor::resetDataflow");
  //update _isRunning
  checkEndOfDataflow();
  if (_isRunning)
    _procRef->stopExecution();
  checkEndOfDataflow();

  if (!_isRunning)
    {
      _isRunning = true;
      _procRef->setExecMode(YACS_ORB::STEPBYSTEP);
      //full reset: set all nodes in error to READY state and start execution
      _procRef->RestartFromState("");
    }
}

  
void GuiExecutor::setStepByStepMode()
{
  DEBTRACE("GuiExecutor::setStepByStepMode");
  _execMode = YACS::STEPBYSTEP;
  if (_isRunning)
    _procRef->setExecMode(YACS_ORB::STEPBYSTEP);
}

void GuiExecutor::setContinueMode()
{
  DEBTRACE("GuiExecutor::setContinueMode");
  _execMode = YACS::CONTINUE;
  if (_isRunning)
    _procRef->setExecMode(YACS_ORB::CONTINUE);
}

void GuiExecutor::setBreakpointMode()
{
  DEBTRACE("GuiExecutor::setBreakpointMode");
  _execMode = YACS::STOPBEFORENODES;
  if (_isRunning)
    _procRef->setExecMode(YACS_ORB::STOPBEFORENODES);
}

void GuiExecutor::setStopOnError(bool aMode)
{
  DEBTRACE("GuiExecutor::setStopOnError " << aMode);
  if (_isRunning)
    {
#ifdef WIN32
      _procRef->setStopOnError(aMode, (getenv("TEMP") + string("\\dumpStateOnError_") + getenv("USER") + string(".xml")).c_str());
#else
      _procRef->setStopOnError(aMode, (string("/tmp/dumpStateOnError_") + getenv("USER") + string(".xml")).c_str());
#endif
      _isStopOnError = true;
    }
}

void GuiExecutor::unsetStopOnError()
{
  DEBTRACE("GuiExecutor::unsetStopOnError");
  if (_isRunning)
    {
      _procRef->unsetStopOnError();
      _isStopOnError = false;
    }
}


void GuiExecutor::saveState(const std::string& xmlFile)
{
  DEBTRACE("GuiExecutor::saveState " << xmlFile);
  bool StartFinish = (getExecutorState() == YACS::NOTYETINITIALIZED ||
                      getExecutorState() == YACS::FINISHED);
  if ( _isRunning ||
       !(CORBA::is_nil(_procRef)) && StartFinish ) {
    if ( !_procRef->saveState(xmlFile.c_str()) ) {
      string what = "Impossible to open file for writing: " + xmlFile;
      throw Exception(what);
    }
  }
}

void GuiExecutor::setLoadStateFile(std::string xmlFile)
{
  DEBTRACE("GuiExecutor::setLoadStateFile " << xmlFile);
  _loadStateFile = xmlFile;
}

  
YACS_ORB::executionMode GuiExecutor::getCurrentExecMode()
{
  DEBTRACE("GuiExecutor::getCurrentExecMode");
  switch (_execMode)
    {
    case YACS::CONTINUE: return YACS_ORB::CONTINUE;
    case YACS::STEPBYSTEP: return YACS_ORB::STEPBYSTEP;
    case YACS::STOPBEFORENODES: return YACS_ORB::STOPBEFORENODES;
    default: return YACS_ORB::CONTINUE;
    }
}

int GuiExecutor::getExecutorState()
{
  DEBTRACE("GuiExecutor::getExecutorState");
  if (_isRunning || !CORBA::is_nil(_procRef))
    return _procRef->getExecutorState();
  else if (CORBA::is_nil(_procRef))
    return YACS::NOTYETINITIALIZED;
  else
    return YACS::FINISHED;
}

  
void GuiExecutor::setBreakpointList(std::list<std::string> breakpointList)
{
  DEBTRACE("GuiExecutor::setBreakpointList");
  if (!_context)
    {
      DEBTRACE("context closed");
      return;
    }
  _breakpointList.clear();
  _breakpointList = breakpointList;
  setBPList();
  if ((_execMode == YACS::CONTINUE) && ! _breakpointList.empty())
    {
      _context->getGMain()->_breakpointsModeAct->setChecked(true);
      setBreakpointMode();
    }
}

void GuiExecutor::addBreakpoint(std::string breakpoint)
{
  DEBTRACE("addBreakpoint " << breakpoint);
  if (!_context)
    {
      DEBTRACE("context closed");
      return;
    }
  _breakpointList.push_back(breakpoint);
  setBPList();
  if ((_execMode == YACS::CONTINUE) && ! _breakpointList.empty())
    {
      _context->getGMain()->_breakpointsModeAct->setChecked(true);
      setBreakpointMode();
    }
}

void GuiExecutor::removeBreakpoint(std::string breakpoint)
{
  DEBTRACE("removeBreakpoint " << breakpoint);
  _breakpointList.remove(breakpoint);
  setBPList();
}

void GuiExecutor::setNextStepList(std::list<std::string> nextStepList)
{
  DEBTRACE("GuiExecutor::setNextStepList");
  if (_isRunning)
    {
      YACS_ORB::stringArray listOfNextStep;
      listOfNextStep.length(nextStepList.size());
      int i=0;
      for (list<string>::iterator it = nextStepList.begin(); it != nextStepList.end(); ++it)
        listOfNextStep[i++] = (*it).c_str();
      _procRef->setStepsToExecute(listOfNextStep);
    }
}

void GuiExecutor::registerStatusObservers()
{
  DEBTRACE("GuiExecutor::registerStatusObservers");
  if (CORBA::is_nil(_procRef))
    {
      DEBTRACE("Runtime error (yacsgui): Lost connection on YACS executor");
      return;
    }
  if (CORBA::is_nil(_observerRef))
    {
      _serv = new GuiObserver_i(_proc);
      _serv->SetImpl(this);
      _observerRef = _serv->_this();
    }
  DEBTRACE("---");
  _serv->SetRemoteProc(_procRef);
  _serv->setConversion();
  DEBTRACE("---");
  std::list<Node*> aNodeSet = _proc->getAllRecursiveConstituents();
  for ( std::list<Node*>::iterator it = aNodeSet.begin(); it != aNodeSet.end(); it++ )
    {
      _procRef->addObserver(_observerRef, _serv->getEngineId((*it)->getNumId()) , "status");
      _procRef->addObserver(_observerRef, _serv->getEngineId((*it)->getNumId()) , "progress");
    }
  _procRef->addObserver(_observerRef, _serv->getEngineId(_proc->getNumId()) , "executor"); 
}



void GuiExecutor::setEngineRef(YACS_ORB::YACS_Gen_ptr ref)
{
  DEBTRACE("GuiExecutor::setEngineRef");
  _engineRef = ref;
}

std::string GuiExecutor::getErrorDetails(YACS::ENGINE::Node* node)
{
  DEBTRACE("GuiExecutor::getErrorDetails");
  if (_serv)
  {
    int engineId=_serv->getEngineId(node->getNumId());
    return _procRef->getErrorDetails(engineId);
  }
  return "---";
}

std::string GuiExecutor::getErrorReport(YACS::ENGINE::Node* node)
{
  DEBTRACE("GuiExecutor::getErrorReport");
  if (_serv)
  {
    int engineId=_serv->getEngineId(node->getNumId());
    return _procRef->getErrorReport(engineId);
  }
  return "---";
}

std::string GuiExecutor::getContainerLog()
{
  DEBTRACE("GuiExecutor::getContainerLog");
  std::string msg="";
  if (!CORBA::is_nil(_engineRef))
    {
      Engines::Container_var cont= _engineRef->GetContainerRef();
      CORBA::String_var logname = cont->logfilename();
      DEBTRACE(logname);
      msg=logname;
      std::string::size_type pos = msg.find(":");
      msg=msg.substr(pos+1);
    }
  return msg;
}

std::string GuiExecutor::getContainerLog(YACS::ENGINE::Node* node)
{
  DEBTRACE("GuiExecutor::getContainerLog(YACS::ENGINE::Node* node)");
  std::string msg;
  if (_serv)
    {
      int engineId=_serv->getEngineId(node->getNumId());
      CORBA::String_var logname = _procRef->getContainerLog(engineId);
      msg=logname;
      std::string::size_type pos = msg.find(":");
      msg=msg.substr(pos+1);
    }
  return msg;
}

void GuiExecutor::shutdownProc()
{
  DEBTRACE("GuiExecutor::shutdownProc " << _shutdownLevel << "," << _isRunning);
  checkEndOfDataflow();
  if (!_isRunning)
    _procRef->shutdownProc(_shutdownLevel);
}

void GuiExecutor::setInPortValue(YACS::ENGINE::DataPort* port, std::string value)
{
  DEBTRACE("GuiExecutor::setInPortValue");

  YACS::ENGINE::Node* node = port->getNode();
  YACS::ENGINE::ComposedNode* rootNode = node->getRootNode();

  std::string nodeName;
  if(rootNode == node)
    nodeName = node->getName();
  else
    nodeName = rootNode->getChildName(node);

  std::string msg = _procRef->setInPortValue(nodeName.c_str(), port->getName().c_str(), value.c_str());
}

bool GuiExecutor::event(QEvent *e)
{
  DEBTRACE("GuiExecutor::event");
  YACSEvent *yev = dynamic_cast<YACSEvent*>(e);
  if (!yev) return false;
  int numid = yev->getYACSEvent().first;
  string event = yev->getYACSEvent().second;
  DEBTRACE("<" << numid << "," << event << ">");
  if (!_context)
    {
      DEBTRACE("context closed");
      return true;
    }
  if (event == "executor") // --- Executor notification: state
    {
      int execState = _procRef->getExecutorState();
      list<string> nextSteps;
      if ((execState == YACS::WAITINGTASKS) || (execState == YACS::PAUSED))
        {
          YACS_ORB::stringArray_var nstp = _procRef->getTasksToLoad();
          for (CORBA::ULong i=0; i<nstp->length(); i++)
            nextSteps.push_back(nstp[i].in());
          if (execState == YACS::PAUSED)
            _isSuspended = true;
        }
      SubjectProc *sproc = _context->getSubjectProc();
      sproc->update(YACS::HMI::UPDATEPROGRESS, execState, sproc);
    }
  else // --- Node notification
    {
      if (! _serv->_engineToGuiMap.count(numid))
        return true;
      int state = _procRef->getNodeState(numid);
      int iGui = _serv->_engineToGuiMap[numid];
      YASSERT(_context->_mapOfExecSubjectNode.count(iGui));
      SubjectNode *snode = _context->_mapOfExecSubjectNode[iGui];
      DEBTRACE("node " << snode->getName() << " state=" << state);

      if (event == "progress") { // --- Update progress bar
        std::string progress = _procRef->getNodeProgress(numid);
        snode->setProgress( progress );
        snode->update(YACS::HMI::PROGRESS, state, snode);
      }
      else { // --- Update node ports
        YACS::ENGINE::Node *node = snode->getNode();
        list<InputPort*> inports = node->getLocalInputPorts();
        list<InputPort*>::iterator iti = inports.begin();
        for ( ; iti != inports.end(); ++iti)
          {
            string val = _procRef->getInPortValue(numid, (*iti)->getName().c_str());
            DEBTRACE("node " << snode->getName() << " inport " << (*iti)->getName() 
                     << " value " << val);
            YASSERT(_context->_mapOfSubjectDataPort.count(*iti));
            SubjectDataPort* port = _context->_mapOfSubjectDataPort[*iti];
            port->setExecValue(val);
            port->update(YACS::HMI::UPDATEPROGRESS, 0, port);
          }
        list<OutputPort*> outports = node->getLocalOutputPorts();
        list<OutputPort*>::iterator ito = outports.begin();
        for ( ; ito != outports.end(); ++ito)
          {
            string val = _procRef->getOutPortValue(numid, (*ito)->getName().c_str());
            DEBTRACE("node " << snode->getName() << " outport " << (*ito)->getName() 
                     << " value " << val);
            YASSERT(_context->_mapOfSubjectDataPort.count(*ito));
            SubjectDataPort* port = _context->_mapOfSubjectDataPort[*ito];
            port->setExecValue(val);
            port->update(YACS::HMI::UPDATEPROGRESS, 0, port);
          }
        snode->update(YACS::HMI::UPDATEPROGRESS, state, snode);
      }
   }

  return true;
}

void GuiExecutor::setBPList()
{
  DEBTRACE("GuiExecutor::setBPList");
  if (_isRunning)
  {
    YACS_ORB::stringArray listOfBreakPoints;
    listOfBreakPoints.length(_breakpointList.size());
    int i=0;
    for (list<string>::iterator it = _breakpointList.begin(); it != _breakpointList.end(); ++it)
      listOfBreakPoints[i++] = (*it).c_str();
    _procRef->setListOfBreakPoints(listOfBreakPoints);
  }
}

YACS::ExecutorState GuiExecutor::updateSchema(string jobState)
{
  YACS::ExecutorState execState = YACS::NOTYETINITIALIZED;

  int numid;
  int state;
  std::list<Node*> aNodeSet = _proc->getAllRecursiveConstituents();
  for ( std::list<Node*>::iterator it = aNodeSet.begin(); it != aNodeSet.end(); it++ ){

    numid = (*it)->getNumId();

    state = _proc->getNodeState(numid);
    SubjectNode *snode = _context->_mapOfExecSubjectNode[numid];

    YACS::ENGINE::Node *node = snode->getNode();
    list<InputPort*> inports = node->getLocalInputPorts();
    list<InputPort*>::iterator iti = inports.begin();
    for ( ; iti != inports.end(); ++iti)
      {
        string val = _proc->getInPortValue(numid, (*iti)->getName().c_str());
        YASSERT(_context->_mapOfSubjectDataPort.count(*iti));
        SubjectDataPort* port = _context->_mapOfSubjectDataPort[*iti];
        port->setExecValue(val);
        port->update(YACS::HMI::UPDATEPROGRESS, 0, port);
      }
    list<OutputPort*> outports = node->getLocalOutputPorts();
    list<OutputPort*>::iterator ito = outports.begin();
    for ( ; ito != outports.end(); ++ito)
      {
        string val = _proc->getOutPortValue(numid, (*ito)->getName().c_str());
        YASSERT(_context->_mapOfSubjectDataPort.count(*ito));
        SubjectDataPort* port = _context->_mapOfSubjectDataPort[*ito];
        port->setExecValue(val);
        port->update(YACS::HMI::UPDATEPROGRESS, 0, port);
      }
    snode->update(YACS::HMI::UPDATEPROGRESS, state, snode);
  }
  state = _proc->getRootNode()->getEffectiveState();
  switch(state){
  case YACS::LOADED:
  case YACS::ACTIVATED:
    if(jobState!="RUNNING")
      execState = YACS::FINISHED;
    else
      execState = YACS::RUNNING;
    break;
  case YACS::FAILED:
  case YACS::DONE:
    execState = YACS::FINISHED;
    break;
  case YACS::SUSPENDED:
    execState = YACS::PAUSED;
    break;
  }
  SubjectProc *sproc = _context->getSubjectProc();
  sproc->update(YACS::HMI::UPDATEPROGRESS, execState, sproc);

  return execState;
}
