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

