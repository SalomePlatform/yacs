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

#include <iostream>
#include <typeinfo>

#include "AlternateThreadPT.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::BASES;

AlternateThreadPT::AlternateThreadPT()
  : _threadStatus(UNEXISTING)
{
  YASSERT(pthread_cond_init(&_pingPongCond, NULL) == 0)
  YASSERT(pthread_mutex_init(&_pingPongMutex, NULL) == 0)
}

AlternateThreadPT::~AlternateThreadPT()
{
  try {
    terminateSlaveThread();
    YASSERT(pthread_mutex_destroy(&_pingPongMutex) == 0)
    YASSERT(pthread_cond_destroy(&_pingPongCond) == 0)
  } catch (const exception & e) {
    cerr << "Exception happened in AlternateThreadPT destructor: " << e.what() << endl;
  } catch (...) {
    cerr << "Unknown exception happened in AlternateThreadPT destructor." << endl;
  }
}

void AlternateThreadPT::start()
{
  // This method must not be called if a slave thread is running
  YASSERT(_threadStatus == UNEXISTING)

  YASSERT(pthread_mutex_lock(&_pingPongMutex) == 0)
  DEBTRACE("Starting thread")
  YASSERT(pthread_create(&_threadId, 0, runThread, this) == 0)
  DEBTRACE("Master waiting for slave")
  YASSERT(pthread_cond_wait(&_pingPongCond, &_pingPongMutex) == 0)
  DEBTRACE("Master running again")
}

void AlternateThreadPT::terminateSlaveThread()
{
  // This method must not be called by the slave thread
  YASSERT(_threadStatus == UNEXISTING || !pthread_equal(pthread_self(), _threadId))

  switch (_threadStatus) {
    case UNEXISTING:
      return;
    case READY_TO_JOIN:
      break;
    case NORMAL_CYCLE:
    {
      // First try to signal the slave thread to end properly
      DEBTRACE("Master is trying to terminate slave by signaling error")
      _threadStatus = TERMINATION_REQUESTED;
      signalSlaveAndWait();

      if (_threadStatus != READY_TO_JOIN) {
        // Try to cancel the thread
        cerr << "Warning: Slave thread in AlternateThread did not end properly. "
                "Thread will be canceled." << endl;
        YASSERT(pthread_cancel(_threadId) == 0)
        YASSERT(pthread_cond_wait(&_pingPongCond, &_pingPongMutex) == 0)

        if (_threadStatus != READY_TO_JOIN) {
          // If cancel failed, we can do nothing more, throw an exception
          YASSERT(false);
        }
      }
      break;
    }
    default:
      YASSERT(false)
  }

  // Finally join the thread
  YASSERT(pthread_mutex_unlock(&_pingPongMutex) == 0)
  YASSERT(pthread_join(_threadId, NULL) == 0)
  _threadStatus = UNEXISTING;
  DEBTRACE("AlternateThread terminated")
}

void AlternateThreadPT::signalSlaveAndWait()
{
  YASSERT(!pthread_equal(pthread_self(), _threadId))
  DEBTRACE("Master signaling slave and waiting");
  signalAndWait();
  DEBTRACE("Master running again");
}

void AlternateThreadPT::signalMasterAndWait()
{
  YASSERT(pthread_equal(pthread_self(), _threadId))
  DEBTRACE("Slave signaling master and waiting");
  signalAndWait();
  DEBTRACE("Slave running again");
}

void AlternateThreadPT::signalAndWait()
{
  YASSERT(_threadStatus == NORMAL_CYCLE || _threadStatus == TERMINATION_REQUESTED)
  YASSERT(pthread_cond_signal(&_pingPongCond) == 0)
  YASSERT(pthread_cond_wait(&_pingPongCond, &_pingPongMutex) == 0)
}

bool AlternateThreadPT::isTerminationRequested() const
{
  return (_threadStatus == TERMINATION_REQUESTED);
}

AlternateThreadPT::ThreadStatus AlternateThreadPT::getThreadStatus() const
{
  return _threadStatus;
}

void * AlternateThreadPT::runThread(void * instance)
{
  try {
    AlternateThreadPT * instanceCst = (AlternateThreadPT *)instance;
    YASSERT(pthread_mutex_lock(&instanceCst->_pingPongMutex) == 0)
    DEBTRACE("Slave thread is now running")
    instanceCst->_threadStatus = NORMAL_CYCLE;
    pthread_cleanup_push(threadCleanupFct, instance);
    instanceCst->run();
    pthread_cleanup_pop(1);
  } catch (const exception & e) {
    cerr << "Unrecoverable error: an exception was caught in AlternateThread "
            "(exceptions should normally be caught before getting here). "
            "Exception type is: " << typeid(e).name() << ", message is: " <<
            e.what() << endl;
    threadCleanupFct(instance);
  }
  // We can't catch (...) here because it causes problems with thread cancellation, at least
  // with gcc 4.1.2 and older. With newer versions it should be possible to do something like
  // catch (abi::__forced_unwind e) { throw; }. See
  // http://gcc.gnu.org/bugzilla/show_bug.cgi?id=28145 for details.
  // The problem is that no exception should get out of the thread scope without being caught
  // (otherwise the program aborts). So for now the user has to take care of catching all
  // exceptions, but he cannot use catch (...).
  return NULL;
}

void AlternateThreadPT::threadCleanupFct(void * instance)
{
  // Beware of not throwing exceptions in this method
  DEBTRACE("Cleaning up slave thread")
  AlternateThreadPT * instanceCst = (AlternateThreadPT *)instance;
  instanceCst->_threadStatus = READY_TO_JOIN;
  pthread_cond_signal(&instanceCst->_pingPongCond);
  pthread_mutex_unlock(&instanceCst->_pingPongMutex);
}
