// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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

#include <YACSGui_Executor.h>
#include <YACSGui_XMLDriver.h>
#include <YACSGui_Observer.h>
#include <YACSGui_Module.h>
#include <SUIT_MessageBox.h>
#include <SalomeApp_Application.h>
#include <Executor.hxx>
#include "utilities.h"

#include <Qtx.h>

#include <qdir.h>

#include <iostream> //for debug only

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace YACS;
using namespace std;

//! Constructor
/*!
 */
YACSGui_Executor::YACSGui_Executor(YACSGui_Module* guiMod, Proc* theProc) :
  QThread(),
  _guiMod(guiMod),
  _proc(theProc)
{
  _localEngine = 0;
  _engineRef = YACS_ORB::YACS_Gen::_nil();
  _procRef = YACS_ORB::ProcExec::_nil();
  _observerRef = YACS_ORB::Observer::_nil();
  _serv = 0;
  _isRunning = false;
  _isSuspended = false;
  _isStopOnError = false;
  _execMode = YACS::CONTINUE;
  _loadStateFile = "";
}

//! Destructor
/*!
 */
YACSGui_Executor::~YACSGui_Executor()
{
}

//! local run in a QThread
/*! local run in a QThread Reimplemented from QThread class.
 */
void YACSGui_Executor::run()
{
  if (!_proc)
    return;
  int debug = 0; // for debug only
  _localEngine->RunW(_proc, debug);
  _isRunning = false;
}

//! Run dataflow.
/*! local run or remote (in a SALOME Container)
 */
void YACSGui_Executor::runDataflow(const bool isRemoteRun)
{
  DEBTRACE("YACSGui_Executor::runDataflow");
  _isRemoteRun = isRemoteRun;
  if (!isRemoteRun)  // --- wanted local run
    {
      if (! _localEngine)
        _localEngine = new Executor();
      if (_isRunning)
        if (! checkEndOfDataFlow()) return;
      _isRunning = true;
      _localEngine->setExecMode(YACS::CONTINUE); // --- control only on remote run
      start();
    }
  else              // --- wanted run in a SALOME Container
    {
      if (CORBA::is_nil(_engineRef))
      {
	DEBTRACE(">> Create YACS ORB engine!");
        _engineRef = YACSGui_Module::InitYACSGuiGen(_guiMod->getApp());
      }

      if (_isRunning)
        if (! checkEndOfDataFlow()) return;
      _isRunning = true;
      //Export the proc into temporary XML file
      QString aFileName = Qtx::tmpDir() + QDir::QDir::separator() + "tmp_" + _proc->getName() + "_" + getenv("USER");
      
      if (CORBA::is_nil(_procRef))
        {
          DEBTRACE("init _procRef");
          VisitorSaveSalomeSchema aWriter( _proc );
          aWriter.openFileSchema( aFileName );
          aWriter.visitProc( _proc );
          aWriter.closeFileSchema();
          _procRef = _engineRef->LoadProc(aFileName.latin1());
          registerStatusObservers();
          DEBTRACE("_procRef _init");
        }
      _procRef->setExecMode(getCurrentExecMode());
      _setBPList();
      if ( _loadStateFile.empty() )
      {
	DEBTRACE(">> Run!");
	cout<<">> this = "<<this<<endl<<endl;
	_procRef->Run();
      }
      else
      {
	DEBTRACE(">> Run from STATE!");
	try {
	_procRef->RunFromState(_loadStateFile.c_str());
	}
	catch (...) {
	  SUIT_MessageBox::error1(_guiMod->getApp()->desktop(), 
				  tr("ERROR"), 
				  tr("Runtime error: execution from the loaded state failed"), 
				  tr("BUT_OK"));
	  return;
	}
	cout<<">> this = "<<this<<endl<<endl;
      }
    }
}

//! test if a dataflow is running
/*!
 * return true if no running dataflow (TODO: check save state ? delete procexec servant ?)
 */
bool YACSGui_Executor::checkEndOfDataFlow(bool display)
{
  if (_isRunning)
    {
      if(running()) // --- local run not finished
        {
          if (display)
            SUIT_MessageBox::error1(_guiMod->getApp()->desktop(), 
                                    tr("ERROR"), 
                                    tr("Local Execution Already running..."), 
                                    tr("BUT_OK"));
          return false;
        }
      if (CORBA::is_nil(_procRef))
        {
          if (display)
            SUIT_MessageBox::error1(_guiMod->getApp()->desktop(), 
                                    tr("ERROR"), 
                                    tr("Runtime error: connection lost on a running scheme"), 
                                    tr("BUT_OK"));
          _isRunning = false;
          return false;              
        }
      if (_procRef->isNotFinished())
        {
          if (display)
            SUIT_MessageBox::error1(_guiMod->getApp()->desktop(), 
                                    tr("ERROR"), 
                                    tr("Remote Execution Already running..."), 
                                    tr("BUT_OK"));
          return false;
        }
      else
        {
          _isRunning = false;
          // --- TODO: delete procExec on server ...
        }
    }
  return true;
}

//! Kill dataflow.
/*!
 */
void YACSGui_Executor::killDataflow()
{
  DEBTRACE("YACSGui_Executor::killDataflow");
  //terminate(); // not safe!
  if (running())        // --- local run
    {
      _localEngine->stopExecution();
    }
  else if (_isRunning)  // --- remote run
    {
      _procRef->stopExecution();
    }
}


//! Suspend/Resume dataflow.
/*!
 */
void YACSGui_Executor::suspendResumeDataflow()
{
  DEBTRACE("YACSGui_Executor::suspendResumeDataflow");
  if (running())        // --- local run
    {
      if (_isSuspended)
        {
          _localEngine->setExecMode(_execMode);
          _localEngine->resumeCurrentBreakPoint();
        }
      else
        _localEngine->setExecMode(YACS::STEPBYSTEP);
    }
  else if (_isRunning)  // --- remote run
    {
      if (_isSuspended)
        {
          _procRef->setExecMode(getCurrentExecMode());
          _procRef->resumeCurrentBreakPoint();
        }
      else
        _procRef->setExecMode(YACS_ORB::STEPBYSTEP);
    }
  _isSuspended = !_isSuspended;
}

void YACSGui_Executor::suspendDataflow()
{
  DEBTRACE("YACSGui_Executor::suspendDataflow");
  if (running())        // --- local run
    {
      _localEngine->setExecMode(YACS::STEPBYSTEP);
    }
  else if (_isRunning)  // --- remote run
    {
      _procRef->setExecMode(YACS_ORB::STEPBYSTEP);
    }
}

void YACSGui_Executor::resumeDataflow()
{
  DEBTRACE("YACSGui_Executor::ResumeDataflow");
  if (running())        // --- local run
    {
      _localEngine->setExecMode(_execMode);
      _localEngine->resumeCurrentBreakPoint();
    }
  else if (_isRunning)  // --- remote run
    {
      _procRef->setExecMode(getCurrentExecMode());
      _procRef->resumeCurrentBreakPoint();
    }
}

//! Stop dataflow.
/*!
 */
void YACSGui_Executor::stopDataflow()
{
  DEBTRACE("YACSGui_Executor::stopDataflow");
  if (running())        // --- local run
    {
      _localEngine->stopExecution();
    }
  else if (_isRunning)  // --- remote run
    {
      _procRef->stopExecution();
    }

}

void YACSGui_Executor::setStepByStepMode()
{
  DEBTRACE("YACSGui_Executor::setStepByStepMode");
  _execMode = YACS::STEPBYSTEP;
  if (running())        // --- local run
    _localEngine->setExecMode(YACS::STEPBYSTEP);
  else if (_isRunning)  // --- remote run
    _procRef->setExecMode(YACS_ORB::STEPBYSTEP);
}

void YACSGui_Executor::setContinueMode()
{
  DEBTRACE("YACSGui_Executor::setContinueMode");
  _execMode = YACS::CONTINUE;
  if (running())        // --- local run
    {
      _localEngine->setExecMode(YACS::CONTINUE);
    }
  else if (_isRunning)  // --- remote run
    {
      _procRef->setExecMode(YACS_ORB::CONTINUE);
    }
}

void YACSGui_Executor::setBreakpointMode()
{
  DEBTRACE("YACSGui_Executor::setBreakpointMode");
  _execMode = YACS::STOPBEFORENODES;
  if (running())        // --- local run
    {
      _localEngine->setExecMode(YACS::STOPBEFORENODES);
    }
  else if (_isRunning)  // --- remote run
    {
      _procRef->setExecMode(YACS_ORB::STOPBEFORENODES);
    }
}

void YACSGui_Executor::setStopOnError(bool aMode)
{
  DEBTRACE("YACSGui_Executor::setStopOnError");
  if (running())        // --- local run
    {
      _localEngine->setStopOnError(aMode,
				   string("/tmp/dumpStateOnError_") + getenv("USER") + string(".xml"));
      _isStopOnError = true;
    }
  else if (_isRunning)  // --- remote run
    {
      _procRef->setStopOnError(aMode,
			       (string("/tmp/dumpStateOnError_") + getenv("USER") + string(".xml")).c_str());
      _isStopOnError = true;
    }
}

void YACSGui_Executor::unsetStopOnError()
{
  DEBTRACE("YACSGui_Executor::unsetStopOnError");
  if (running())        // --- local run
    {
      _localEngine->unsetStopOnError();
      _isStopOnError = false;
    }
  else if (_isRunning)  // --- remote run
    {
      _procRef->unsetStopOnError();
      _isStopOnError = false;
    }
}

void YACSGui_Executor::saveState(const std::string& xmlFile)
{
  DEBTRACE("YACSGui_Executor::saveState");
  bool StartFinish = (getExecutorState() == YACS::NOTYETINITIALIZED || getExecutorState() == YACS::FINISHED );

  if ( running()
       ||
       _localEngine && (CORBA::is_nil(_procRef)) && StartFinish )        // --- local run
    _localEngine->saveState(xmlFile);
  else if ( _isRunning
	    ||
	    !_localEngine && !(CORBA::is_nil(_procRef)) && StartFinish )  // --- remote run
    _procRef->saveState(xmlFile.c_str());
}

void YACSGui_Executor::setLoadStateFile(std::string xmlFile)
{
  _loadStateFile = xmlFile;
}

void YACSGui_Executor::setNextStepList(std::list<std::string> nextStepList)
{
  DEBTRACE("YACSGui_Executor::setNextStepList");
  if (running())        // --- local run
    {
      _localEngine->setStepsToExecute(nextStepList);
    }
  else if (_isRunning)  // --- remote run
    {
      YACS_ORB::stringArray listOfNextStep;
      listOfNextStep.length(nextStepList.size());
      int i=0;
      for (list<string>::iterator it = nextStepList.begin(); it != nextStepList.end(); ++it)
        listOfNextStep[i++] = (*it).c_str();
      _procRef->setStepsToExecute(listOfNextStep);
    }
}

void YACSGui_Executor::setBreakpointList(std::list<std::string> breakpointList)
{
  DEBTRACE("YACSGui_Executor::setBreakpointList");
  _breakpointList.clear();
  _breakpointList = breakpointList;
  _setBPList();
}

//! list must be sent, even empty (only way to remove all breakpoints)
void YACSGui_Executor::_setBPList()
{
  if (running())        // --- local run
    {
      _localEngine->setListOfBreakPoints(_breakpointList);
    }
  else if (_isRunning)  // --- remote run
    {
      YACS_ORB::stringArray listOfBreakPoints;
      listOfBreakPoints.length(_breakpointList.size());
      int i=0;
      for (list<string>::iterator it = _breakpointList.begin(); it != _breakpointList.end(); ++it)
        listOfBreakPoints[i++] = (*it).c_str();
      _procRef->setListOfBreakPoints(listOfBreakPoints);
    }
}

void YACSGui_Executor::registerStatusObservers()
{
  DEBTRACE("YACSGui_Executor::registerStatusObservers");
  if (CORBA::is_nil(_procRef))
    {
      SUIT_MessageBox::error1(_guiMod->getApp()->desktop(), 
                              tr("ERROR"), 
                              tr("Runtime error (yacsgui): Lost connection on YACS executor"), 
                              tr("BUT_OK"));
      return;
    }
  if (CORBA::is_nil(_observerRef))
    {
      _serv = new Observer_i(_proc, _guiMod, this);
      _observerRef = _serv->_this();
      _serv->SetImpl(_graph->getStatusObserver());
    }
  DEBTRACE("---");
  _serv->SetRemoteProc(_procRef);
  _serv->setConversion();
  DEBTRACE("---");
  std::list<Node*> aNodeSet = _proc->getAllRecursiveConstituents();
  for ( std::list<Node*>::iterator it = aNodeSet.begin(); it != aNodeSet.end(); it++ )
    {
      _procRef->addObserver(_observerRef, _serv->getEngineId((*it)->getNumId()) , "status");
    }
  _procRef->addObserver(_observerRef, _serv->getEngineId(_proc->getNumId()) , "executor"); 
}

YACS_ORB::executionMode YACSGui_Executor::getCurrentExecMode()
{
  switch (_execMode)
    {
    case YACS::CONTINUE: return YACS_ORB::CONTINUE;
    case YACS::STEPBYSTEP: return YACS_ORB::STEPBYSTEP;
    case YACS::STOPBEFORENODES: return YACS_ORB::STOPBEFORENODES;
    default: return YACS_ORB::CONTINUE;
    }
}

int YACSGui_Executor::getExecutorState()
{
  if ( running()
       ||
       _localEngine && (CORBA::is_nil(_procRef)) )        // --- local run
    return _localEngine->getExecutorState();
  else if ( _isRunning
	    ||
	    !_localEngine && !(CORBA::is_nil(_procRef)) )  // --- remote run
    return _procRef->getExecutorState();
  else if ( !_localEngine && (CORBA::is_nil(_procRef)) )
    return YACS::NOTYETINITIALIZED;
  else if ( _localEngine && !(CORBA::is_nil(_procRef)) )
    return YACS::FINISHED;
}

void YACSGui_Executor::setEngineRef(YACS_ORB::YACS_Gen_ptr theRef)
{
  _engineRef = theRef;
}

std::string YACSGui_Executor::getErrorDetails(YACS::ENGINE::Node* node)
{
  if (_serv)
  {
    //get the node engine id
    int engineId=_serv->getEngineId(node->getNumId());
    return _procRef->getErrorDetails(engineId);
  }
  return "---";
}

std::string YACSGui_Executor::getErrorReport(YACS::ENGINE::Node* node)
{
  if (_serv)
  {
    //get the node engine id
    int engineId=_serv->getEngineId(node->getNumId());
    return _procRef->getErrorReport(engineId);
  }
  return "---";
}

std::string YACSGui_Executor::getContainerLog()
{
  DEBTRACE("YACSGui_Executor::getContainerLog");
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

std::string YACSGui_Executor::getContainerLog(YACS::ENGINE::Node* node)
{
  std::string msg;
  if (_serv)
    {
      //get the node engine id
      int engineId=_serv->getEngineId(node->getNumId());
      CORBA::String_var logname = _procRef->getContainerLog(engineId);
      msg=logname;
      std::string::size_type pos = msg.find(":");
      msg=msg.substr(pos+1);
    }
  return msg;
}
