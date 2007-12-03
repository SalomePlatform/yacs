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
  _engineRef = YACSGui_ORB::YACSGui_Gen::_nil();
  _procRef = YACSGui_ORB::ProcExec::_nil();
  _observerRef = YACSGui_ORB::Observer::_nil();
  _serv = 0;
  _isRunning = false;
  _isSuspended = false;
  _isStopOnError = false;
  _execMode = YACS::CONTINUE;
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
  MESSAGE("YACSGui_Executor::runDataflow");
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
        _engineRef = YACSGui_Module::InitYACSGuiGen(_guiMod->getApp());

      if (_isRunning)
        if (! checkEndOfDataFlow()) return;
      _isRunning = true;
      //Export the proc into temporary XML file
      QString aFileName = Qtx::tmpDir() + QDir::QDir::separator() + "tmp_" + _proc->getName();
      
      if (CORBA::is_nil(_procRef))
        {
          MESSAGE("init _procRef");
          VisitorSaveSchema aWriter( _proc );
          aWriter.openFileSchema( aFileName );
          aWriter.visitProc( _proc );
          aWriter.closeFileSchema();
          _procRef = _engineRef->LoadProc(aFileName.latin1());
          registerStatusObservers();
          MESSAGE("_procRef _init");
        }
      _procRef->setExecMode(getCurrentExecMode());
      _setBPList();
      _procRef->Run();
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
  MESSAGE("YACSGui_Executor::killDataflow");
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
  MESSAGE("YACSGui_Executor::suspendResumeDataflow");
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
        _procRef->setExecMode(YACSGui_ORB::STEPBYSTEP);
    }
  _isSuspended = !_isSuspended;
}

void YACSGui_Executor::suspendDataflow()
{
  MESSAGE("YACSGui_Executor::suspendDataflow");
  if (running())        // --- local run
    {
      _localEngine->setExecMode(YACS::STEPBYSTEP);
    }
  else if (_isRunning)  // --- remote run
    {
      _procRef->setExecMode(YACSGui_ORB::STEPBYSTEP);
    }
}

void YACSGui_Executor::resumeDataflow()
{
  MESSAGE("YACSGui_Executor::ResumeDataflow");
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
  MESSAGE("YACSGui_Executor::stopDataflow");
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
  MESSAGE("YACSGui_Executor::setStepByStepMode");
  _execMode = YACS::STEPBYSTEP;
  if (running())        // --- local run
    {
      printf("=> local\n");
      _localEngine->setExecMode(YACS::STEPBYSTEP);
    }
  else if (_isRunning)  // --- remote run
    {
      printf("=> remote\n");
      _procRef->setExecMode(YACSGui_ORB::STEPBYSTEP);
    }
}

void YACSGui_Executor::setContinueMode()
{
  MESSAGE("YACSGui_Executor::setContinueMode");
  _execMode = YACS::CONTINUE;
  if (running())        // --- local run
    {
      _localEngine->setExecMode(YACS::CONTINUE);
    }
  else if (_isRunning)  // --- remote run
    {
      _procRef->setExecMode(YACSGui_ORB::CONTINUE);
    }
}

void YACSGui_Executor::setBreakpointMode()
{
  MESSAGE("YACSGui_Executor::setBreakpointMode");
  _execMode = YACS::STOPBEFORENODES;
  if (running())        // --- local run
    {
      _localEngine->setExecMode(YACS::STOPBEFORENODES);
    }
  else if (_isRunning)  // --- remote run
    {
      _procRef->setExecMode(YACSGui_ORB::STOPBEFORENODES);
    }
}

void YACSGui_Executor::setStopOnError(bool aMode)
{
  MESSAGE("YACSGui_Executor::setStopOnError");
  if (running())        // --- local run
    {
      _localEngine->setStopOnError(aMode, "/tmp/dumpStateOnError.xml");
      _isStopOnError = aMode;
    }
  else if (_isRunning)  // --- remote run
    {
      _procRef->setStopOnError(aMode, "/tmp/dumpStateOnError.xml");
      _isStopOnError = aMode;
    }
}

void YACSGui_Executor::setNextStepList(std::list<std::string> nextStepList)
{
  MESSAGE("YACSGui_Executor::setNextStepList");
  if (running())        // --- local run
    {
      _localEngine->setStepsToExecute(nextStepList);
    }
  else if (_isRunning)  // --- remote run
    {
      YACSGui_ORB::stringArray listOfNextStep;
      listOfNextStep.length(nextStepList.size());
      int i=0;
      for (list<string>::iterator it = nextStepList.begin(); it != nextStepList.end(); ++it)
        listOfNextStep[i++] = (*it).c_str();
      _procRef->setStepsToExecute(listOfNextStep);
    }
}

void YACSGui_Executor::setBreakpointList(std::list<std::string> breakpointList)
{
  MESSAGE("YACSGui_Executor::setBreakpointList");
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
      YACSGui_ORB::stringArray listOfBreakPoints;
      listOfBreakPoints.length(_breakpointList.size());
      int i=0;
      for (list<string>::iterator it = _breakpointList.begin(); it != _breakpointList.end(); ++it)
        listOfBreakPoints[i++] = (*it).c_str();
      _procRef->setListOfBreakPoints(listOfBreakPoints);
    }
}

void YACSGui_Executor::registerStatusObservers()
{
  MESSAGE("YACSGui_Executor::registerStatusObservers");
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
  MESSAGE("---");
  _serv->SetRemoteProc(_procRef);
  _serv->setConversion();
  MESSAGE("---");
  std::set<Node*> aNodeSet = _proc->getAllRecursiveConstituents();
  for ( std::set<Node*>::iterator it = aNodeSet.begin(); it != aNodeSet.end(); it++ )
    {
      _procRef->addObserver(_observerRef, _serv->getEngineId((*it)->getNumId()) , "status");
    }
  _procRef->addObserver(_observerRef, _serv->getEngineId(_proc->getNumId()) , "executor"); 
}

YACSGui_ORB::executionMode YACSGui_Executor::getCurrentExecMode()
{
  switch (_execMode)
    {
    case YACS::CONTINUE: return YACSGui_ORB::CONTINUE;
    case YACS::STEPBYSTEP: return YACSGui_ORB::STEPBYSTEP;
    case YACS::STOPBEFORENODES: return YACSGui_ORB::STOPBEFORENODES;
    default: return YACSGui_ORB::CONTINUE;
    }
}
