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
#ifndef __ALTERNATETHREADPT_HXX__
#define __ALTERNATETHREADPT_HXX__

#include <pthread.h>

#include "YACSBasesExport.hxx"

namespace YACS
{
  namespace BASES
  {
    //! This class provides a mechanism to run two threads alternately.
    /*!
     * Alternate threads can be necessary when two pieces of code must run alternately
     * and communicate but are difficult or impossible to synchronize explicitly
     * (e.g. asynchronous algorithms in optimizer loop). This class guarantees that the
     * two threads NEVER run concurrently, so no lock mechanism is necessary when sharing
     * data between them.
     *
     * The two threads are called "master thread" and "slave thread". The master thread is
     * the one that calls the method start() and that will continue to run after the
     * destruction of this object. The slave thread is created when the method start() is
     * called. It will run the code in the method run() and will be destroyed when the
     * master thread calls terminateSlaveThread() or at the destruction of this object.
     *
     * When the master thread calls start(), it will block and the slave thread will begin
     * to execute the code in the method run(). The two threads can then alternate by
     * calling the method signalMasterAndWait() (in the slave thread) and signalSlaveAndWait()
     * (in the master thread). Finally, the master thread must call terminateSlaveThread()
     * to terminate the slave thread. There is no explicit mechanism for the slave thread to
     * request its own termination, but this can be done with an external flag (see the pool
     * object in OptimizerLoop for instance).
     *
     * This class is purely virtual. Subclasses must implement the run() method that will
     * be executed in the slave thread.
     */
    class YACSBASES_EXPORT AlternateThreadPT
    {
    public:
      enum ThreadStatus {UNEXISTING, NORMAL_CYCLE, TERMINATION_REQUESTED, READY_TO_JOIN};

      AlternateThreadPT();
      virtual ~AlternateThreadPT();

      //! Create and launch the slave thread.
      /*!
       * This method must not be called by the slave thread or
       * if a slave thread is already running.
       */
      void start();

      //! Block the master thread and release the slave thread.
      virtual void signalSlaveAndWait();

      //! Terminate the slave thread.
      void terminateSlaveThread();

      //! Block the slave thread and release the master thread.
      virtual void signalMasterAndWait();

      //! Return true if the master requested the slave thread termination.
      bool isTerminationRequested() const;

      //! Return the thread status.
      ThreadStatus getThreadStatus() const;

    protected:
      //! This method must be implemented in subclasses and will be run in the slave thread.
      /*!
       * The slave thread must call signalMasterAndWait() when necessary to give the control
       * back to the master thread. When returning from this method, the slave thread MUST
       * check for an eventual termination request (with the method isTerminationRequested()).
       * If the termination is requested, the slave thread must perform any necessary cleanup
       * and finish as soon as possible.
       */
      virtual void run()=0;

    private:
      void signalAndWait();

      static void * runThread(void * instance);
      static void threadCleanupFct(void * instance);

      pthread_t _threadId;
      ThreadStatus _threadStatus;
      pthread_cond_t _pingPongCond;
      pthread_mutex_t _pingPongMutex;

    };
  }
}

#endif
