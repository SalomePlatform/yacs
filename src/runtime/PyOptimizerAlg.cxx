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

// rnv: avoid compilation warning on Linux : "_POSIX_C_SOURCE" and "_XOPEN_SOURCE" are redefined
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif
#include <Python.h>

#include "PyOptimizerAlg.hxx"

using namespace std;
using namespace YACS::BASES;
using namespace YACS::ENGINE;

struct GILState{
  PyGILState_STATE gstate;
  GILState(){gstate=PyGILState_Ensure();}
  ~GILState(){PyGILState_Release(gstate);}
};


PyOptimizerAlgBase::PyOptimizerAlgBase(Pool *pool)
  : OptimizerAlgBase(pool)
{
}

PyOptimizerAlgBase::~PyOptimizerAlgBase()
{
}

void PyOptimizerAlgBase::startProxy()
{
  GILState gs;
  OptimizerAlgBase::startProxy();
}

void PyOptimizerAlgBase::takeDecisionProxy()
{
  GILState gs;
  OptimizerAlgBase::takeDecisionProxy();
}

void PyOptimizerAlgBase::finishProxy()
{
  GILState gs;
  OptimizerAlgBase::finishProxy();
}

void PyOptimizerAlgBase::initializeProxy(const Any *input) throw (YACS::Exception)
{
  GILState gs;
  OptimizerAlgBase::initializeProxy(input);
}

TypeCode * PyOptimizerAlgBase::getTCForInProxy() const
{
  GILState gs;
  return OptimizerAlgBase::getTCForInProxy();
}

TypeCode * PyOptimizerAlgBase::getTCForOutProxy() const
{
  GILState gs;
  return OptimizerAlgBase::getTCForOutProxy();
}

TypeCode * PyOptimizerAlgBase::getTCForAlgoInitProxy() const
{
  GILState gs;
  return OptimizerAlgBase::getTCForAlgoInitProxy();
}

TypeCode * PyOptimizerAlgBase::getTCForAlgoResultProxy() const
{
  GILState gs;
  return OptimizerAlgBase::getTCForAlgoResultProxy();
}

Any * PyOptimizerAlgBase::getAlgoResultProxy()
{
  GILState gs;
  return OptimizerAlgBase::getAlgoResultProxy();
}


PyOptimizerAlgASync::PyOptimizerAlgASync(Pool *pool)
  : PyOptimizerAlgBase(pool)
{
}

PyOptimizerAlgASync::~PyOptimizerAlgASync()
{
  if (getThreadStatus() == AlternateThread::UNEXISTING) return;

  Py_BEGIN_ALLOW_THREADS;
  try {
    // Force the thread termination. We must allow Python threads for that because the
    // thread cleanup includes some Python cleanup.
    terminateSlaveThread();
  } catch (const exception & e) {
    cerr << "Exception happened in PyOptimizerAlgASync destructor: " << e.what() << endl;
  } catch (...) {
    cerr << "Unknown exception happened in PyOptimizerAlgASync destructor." << endl;
  }
  Py_END_ALLOW_THREADS;
}

void PyOptimizerAlgASync::startProxy()
{
  start();
}

void PyOptimizerAlgASync::takeDecisionProxy()
{
  takeDecision();
}

void PyOptimizerAlgASync::finishProxy()
{
  terminateSlaveThread();
  PyOptimizerAlgBase::finishProxy();
}

void PyOptimizerAlgASync::start()
{
  AlternateThread::start();
}

void PyOptimizerAlgASync::takeDecision()
{
  signalSlaveAndWait();
}

void PyOptimizerAlgASync::signalMasterAndWait()
{
  Py_BEGIN_ALLOW_THREADS;
  AlternateThread::signalMasterAndWait();
  Py_END_ALLOW_THREADS;
}

void PyOptimizerAlgASync::run()
{
  GILState gs;
  startToTakeDecision();
}
