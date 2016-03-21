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
#include "ExecutorSwig.hxx"
#include "Scheduler.hxx"

#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

void ExecutorSwig::RunPy(Scheduler *graph,int debug, bool isPyThread, bool fromscratch)
{
  DEBTRACE("ExecutorSwig::RunPy(Scheduler *graph,int debug, bool isPyThread, bool fromscratch) "
           << debug << " " << isPyThread << " " << fromscratch);
  PyThreadState *_save;
  if (isPyThread) _save = PyEval_SaveThread(); // allow Python threads when embedded in a Python thread
  try
    {
      RunB(graph, debug, fromscratch);
    }
  catch (YACS::Exception& e)
    {
      DEBTRACE("YACS exception caught: ");
      DEBTRACE(e.what());
    }
  catch (const std::ios_base::failure&)
    {
      DEBTRACE("io failure");
    }
  catch(...)
    {
      DEBTRACE("Caught unknown exception.");
    }
  if (isPyThread) PyEval_RestoreThread(_save); // restore thread state and lock at the end of Python thread
}

void ExecutorSwig::waitPause()
{
  DEBTRACE("ExecutorSwig::waitPause()");
  PyThreadState *_save;
  _save = PyEval_SaveThread(); // allow Python threads when embedded in a Python thread
  Executor::waitPause();
  PyEval_RestoreThread(_save); // restore thread state and lock at the end of Python thread
}

