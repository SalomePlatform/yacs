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

#include "SalomeOptimizerLoop.hxx"

// rnv: avoid compilation warning on Linux : "_POSIX_C_SOURCE" and "_XOPEN_SOURCE" are redefined
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif
#include <Python.h>
#include "TypeCode.hxx"
#include "PyStdout.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;


/*! \class YACS::ENGINE::SalomeOptimizerLoop
 *  \brief class to build optimization loops
 *
 * \ingroup Nodes
 */

SalomeOptimizerLoop::SalomeOptimizerLoop(const std::string& name, const std::string& algLibWthOutExt,
                                         const std::string& symbolNameToOptimizerAlgBaseInstanceFactory,
                                         bool algInitOnFile,bool initAlgo, Proc * procForTypes):
                     OptimizerLoop(name,algLibWthOutExt,symbolNameToOptimizerAlgBaseInstanceFactory,algInitOnFile,false)
{
  if(initAlgo)
    {
      //try
      //  {
          setAlgorithm(algLibWthOutExt, symbolNameToOptimizerAlgBaseInstanceFactory, true, procForTypes);
      //  }
      //catch(YACS::Exception& e)
      //  {
          //ignore it
      //  }
    }
}

SalomeOptimizerLoop::SalomeOptimizerLoop(const SalomeOptimizerLoop& other, ComposedNode *father, bool editionOnly): 
                     OptimizerLoop(other,father,editionOnly)
{
}

SalomeOptimizerLoop::~SalomeOptimizerLoop()
{
}

Node *SalomeOptimizerLoop::simpleClone(ComposedNode *father, bool editionOnly) const
{
  SalomeOptimizerLoop* sol=new SalomeOptimizerLoop(*this,father,editionOnly);
  // TODO: Remove this const_cast (find a better design to get the type codes from the original node)
  Proc * procForTypes = sol->getProc();
  if (procForTypes == NULL) {
    const Proc * origProc = getProc();
    procForTypes = const_cast<Proc *>(origProc);
  }
  sol->setAlgorithm(_alglib, _symbol, false, procForTypes);
  return sol;
}

//! Load the algorithm object from a Python module or a dynamic library
/*!
 *  Member _alglib is the library name (shared library WITHOUT extension .so or python
 *  module WITH extension .py). Member _symbol is a symbol name in the library to use as
 *  an algorithm factory.
 */
void SalomeOptimizerLoop::loadAlgorithm()
{
  YASSERT(_alg == NULL)

  if(_alglib.size() > 3 && _alglib.substr(_alglib.size()-3,3)==".py")
    {
      //if alglib extension is .py try to import the corresponding python module
      PyGILState_STATE gstate=PyGILState_Ensure();

      PyObject* mainmod = PyImport_AddModule("__main__");
      PyObject* globals = PyModule_GetDict(mainmod);

      std::string pyscript;
      pyscript="import sys\n"
               "import SALOMERuntime\n"
               "filename='";
      pyscript=pyscript+_alglib+"'\nentry='"+_symbol+"'\n";
      pyscript=pyscript+"import os\n"
                        "import pilot\n"
                        "rep,mod=os.path.split(os.path.splitext(filename)[0])\n"
                        "if rep != '':\n"
                        "  sys.path.insert(0,rep)\n"
                        "algomodule=__import__(mod)\n"
                        "if rep != '':\n"
                        "  del sys.path[0]\n"
                        "algoclass= getattr(algomodule,entry)\n"
                        "algo= algoclass()\n"
                        "swigalgo= algo.this\n"
                        "\n";

      PyObject* res=PyRun_String(pyscript.c_str(), Py_file_input, globals, globals );

      if(res == NULL)
        {
          //error during import
          _errorDetails="";
          PyObject* new_stderr = newPyStdOut(_errorDetails);
          PySys_SetObject((char*)"stderr", new_stderr);
          PyErr_Print();
          PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
          Py_DECREF(new_stderr);
          modified();
          PyGILState_Release(gstate);
          throw YACS::Exception(_errorDetails);
        }
      else
        {
          Py_DECREF(res);

          typedef struct {
              PyObject_HEAD
              void *ptr;
              void *ty;
              int own;
              PyObject *next;
          } SwigPyObject;

          SwigPyObject* pyalgo = (SwigPyObject*)PyDict_GetItemString(globals, "swigalgo");
          _alg=(OptimizerAlgBase*)pyalgo->ptr;
          _alg->setPool(&_myPool);
          _alg->incrRef();
        }
      PyGILState_Release(gstate);
    }
  else
    {
      //else try to load a dynamic library
      OptimizerLoop::loadAlgorithm();
    }
}
