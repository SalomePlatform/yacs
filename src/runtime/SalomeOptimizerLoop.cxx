//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "SalomeOptimizerLoop.hxx"
#include <Python.h>
#include "TypeCode.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

struct GILState{
  PyGILState_STATE gstate;
  GILState(){gstate=PyGILState_Ensure();}
  ~GILState(){PyGILState_Release(gstate);}
};

SalomeOptimizerAlgStandardized::SalomeOptimizerAlgStandardized(Pool *pool, OptimizerAlgBase *alg)
                               : OptimizerAlgStandardized(pool,alg)
{
}

SalomeOptimizerAlgStandardized::~SalomeOptimizerAlgStandardized()
{
}

void SalomeOptimizerAlgStandardized::parseFileToInit(const std::string& fileName)
{
  GILState gs;
  OptimizerAlgStandardized::parseFileToInit(fileName);
}

void SalomeOptimizerAlgStandardized::start()
{
  switch(_algBehind->getType())
    {
    case EVENT_ORIENTED:
      {
        GILState gs;
        ((OptimizerAlgSync *) _algBehind)->start();
        break;
      }
    case NOT_EVENT_ORIENTED:
      {
        void **stackForNewTh= new void* [2];
        stackForNewTh[0]=(void *) ((OptimizerAlgASync *)(_algBehind));//In case of virtual inheritance
        stackForNewTh[1]=(void *) &_condition;
        _threadInCaseOfNotEvent=new ::YACS::BASES::Thread(threadFctForAsync,stackForNewTh);
        _condition.waitForAWait();
        break;
      }
    default:
      throw Exception("Unrecognized type of algorithm. Only 2 types are available : EVENT_ORIENTED or NOT_EVENT_ORIENTED.");
    }
}

void SalomeOptimizerAlgStandardized::takeDecision()
{
  switch(_algBehind->getType())
    {
    case EVENT_ORIENTED:
      {
        GILState gs;
        ((OptimizerAlgSync *) _algBehind)->takeDecision();
        break;
      }
    case NOT_EVENT_ORIENTED:
      {
        _condition.notifyOneSync();
        break;
      }
    default:
      throw Exception("Unrecognized type of algorithm. Only 2 types are available : EVENT_ORIENTED or NOT_EVENT_ORIENTED.");
    }
}

void SalomeOptimizerAlgStandardized::finish()
{
  GILState gs;
  OptimizerAlgStandardized::finish();
}

void SalomeOptimizerAlgStandardized::initialize(const Any *input) throw (YACS::Exception)
{
  GILState gs;
  OptimizerAlgStandardized::initialize(input);
}

TypeCode *SalomeOptimizerAlgStandardized::getTCForIn() const
{
  GILState gs;
  return OptimizerAlgStandardized::getTCForIn();
}

TypeCode *SalomeOptimizerAlgStandardized::getTCForOut() const
{
  GILState gs;
  return OptimizerAlgStandardized::getTCForOut();
}

void *SalomeOptimizerAlgStandardized::threadFctForAsync(void* ownStack)
{
  GILState gs;
  void **ownStackCst=(void **)ownStack;
  OptimizerAlgASync *alg=(OptimizerAlgASync *)ownStackCst[0];
  ::YACS::BASES::DrivenCondition *cond=(::YACS::BASES::DrivenCondition *)ownStackCst[1];
  delete [] ownStackCst;
  alg->startToTakeDecision(cond);
  return 0;
}

/*! \class YACS::ENGINE::SalomeOptimizerLoop
 *  \brief class to build optimization loops
 *
 * \ingroup Nodes
 */

SalomeOptimizerLoop::SalomeOptimizerLoop(const std::string& name, const std::string& algLibWthOutExt,
                                         const std::string& symbolNameToOptimizerAlgBaseInstanceFactory, bool algInitOnFile,bool initAlgo):
                     OptimizerLoop(name,algLibWthOutExt,symbolNameToOptimizerAlgBaseInstanceFactory,algInitOnFile,false),
                     _pyalg(0),_cppalg(0)
{
  if(initAlgo)
    setAlgorithm(algLibWthOutExt,symbolNameToOptimizerAlgBaseInstanceFactory);
}

SalomeOptimizerLoop::SalomeOptimizerLoop(const OptimizerLoop& other, ComposedNode *father, bool editionOnly): 
                     OptimizerLoop(other,father,editionOnly),_pyalg(0),_cppalg(0)
{
}

SalomeOptimizerLoop::~SalomeOptimizerLoop()
{
}

Node *SalomeOptimizerLoop::simpleClone(ComposedNode *father, bool editionOnly) const
{
  SalomeOptimizerLoop* sol=new SalomeOptimizerLoop(*this,father,editionOnly);
  sol->setAlgorithm(_loader.getLibNameWithoutExt(),_symbol,false);
  return sol;
}

//! Create and set the algorithm object given the library and factory names (symbol in library)
/*!
 *   throw an exception if the node is connected
 *   \param alglib the library name (shared library WITHOUT extension .so or python module WITH extension .py)
 *   \param symbol a symbol name in the library to use as an algorithm factory
 *   \param checkLinks if true check if the ports are connected  (default true)
 *   \exception Exception if ports are connected and checkLinks is true
 */
void SalomeOptimizerLoop::setAlgorithm(const std::string& alglib, const std::string& symbol, bool checkLinks)
{
  if(checkLinks)
    {
      if(_splittedPort.edGetNumberOfOutLinks() != 0)
        throw Exception("The OptimizerLoop node must be disconnected before setting the algorithm");
      if(_retPortForOutPool.edGetNumberOfLinks() != 0)
        throw Exception("The OptimizerLoop node must be disconnected before setting the algorithm");
    }

  _symbol=symbol;

  OptimizerAlgBaseFactory algFactory=0;

  OptimizerAlgBase* algo=0;

  if(alglib.size() > 3 && alglib.substr(alglib.size()-3,3)==".py")
    {
      //if alglib extension is .py try to import the corresponding python module
      PyGILState_STATE gstate=PyGILState_Ensure();

      PyObject* mainmod = PyImport_AddModule("__main__");
      PyObject* globals = PyModule_GetDict(mainmod);

      std::string pyscript;
      pyscript="import sys\n"
               "import SALOMERuntime\n"
               "filename='";
      pyscript=pyscript+alglib+"'\nentry='"+symbol+"'\n";
      pyscript=pyscript+"import os\n"
                        "import pilot\n"
                        "rep,mod=os.path.split(os.path.splitext(filename)[0])\n"
                        "if rep != '':\n"
                        "  sys.path.insert(0,rep)\n"
                        "algomodule=__import__(mod)\n"
                        "if rep != '':\n"
                        "  del sys.path[0]\n"
                        "algoclass= getattr(algomodule,entry)\n"
                        "algo= algoclass(None)\n"
                        "swigalgo= algo.this\n"
                        "\n";

      PyObject* res=PyRun_String(pyscript.c_str(), Py_file_input, globals, globals );

      if(res == NULL)
        {
          PyErr_Print();
          PyGILState_Release(gstate);
          throw Exception("Problem when creating the algorithm : see traceback");
        }

      Py_DECREF(res);

      typedef struct {
          PyObject_HEAD
          void *ptr;
          void *ty;
          int own;
          PyObject *next;
      } SwigPyObject;

      SwigPyObject* pyalgo = (SwigPyObject*)PyDict_GetItemString(globals, "swigalgo");
      algo=(OptimizerAlgBase*)pyalgo->ptr;
      algo->setPool(&_myPool);
      algo->incrRef();

      if(_cppalg)
        {
          _cppalg->decrRef();
          _cppalg=0;
        }

      if(_pyalg==0)
        _pyalg=new SalomeOptimizerAlgStandardized(&_myPool,0);
      _alg=_pyalg;

      _alg->setAlgPointer(algo);

      if(!algo)
        return;

      PyGILState_Release(gstate);
    }
  else
    {
      //else try to load a dynamic library
      _loader=YACS::BASES::DynLibLoader(alglib);

      if(_pyalg)
        {
          _pyalg->decrRef();
          _pyalg=0;
        }

      if(_cppalg==0)
        _cppalg=new OptimizerAlgStandardized(&_myPool,0);
      _alg=_cppalg;

      if(alglib!="" && _symbol!="")
        algFactory=(OptimizerAlgBaseFactory)_loader.getHandleOnSymbolWithName(_symbol);

      if(algFactory)
        algo=algFactory(&_myPool);

      _alg->setAlgPointer(algo);

      if(!algo)
        return;
    }

  _splittedPort.edSetType(_alg->getTCForIn());
  _retPortForOutPool.edSetType(_alg->getTCForOut());
}

