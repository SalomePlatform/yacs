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
#include "RuntimeSALOME.hxx"
#include "PythonNode.hxx"
#include "PythonPorts.hxx"
#include "TypeCode.hxx"
#include "Container.hxx"
#include "SalomeContainer.hxx"

#include "PyStdout.hxx"
#include <iostream>
#include <sstream>
#include <fstream>

#ifdef WNT
#include <process.h>
#define getpid _getpid
#endif

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char PythonNode::IMPL_NAME[]="Python";
const char PythonNode::KIND[]="Python";

PythonNode::PythonNode(const PythonNode& other, ComposedNode *father):InlineNode(other,father)
{
  _implementation=IMPL_NAME;
  PyGILState_STATE gstate=PyGILState_Ensure();
  _context=PyDict_New();
  PyGILState_Release(gstate);
}

PythonNode::PythonNode(const std::string& name):InlineNode(name)
{
  _implementation=IMPL_NAME;
  PyGILState_STATE gstate = PyGILState_Ensure();
  _context=PyDict_New();
  PyGILState_Release(gstate);
}

PythonNode::~PythonNode()
{
  PyGILState_STATE gstate = PyGILState_Ensure();
  DEBTRACE( "_context refcnt: " << _context->ob_refcnt );
  Py_DECREF(_context);
  PyGILState_Release(gstate);
}

void PythonNode::checkBasicConsistency() const throw(YACS::Exception)
{
  DEBTRACE("checkBasicConsistency");
  InlineNode::checkBasicConsistency();

  PyGILState_STATE gstate = PyGILState_Ensure();
  PyObject* res;
  res=Py_CompileString(_script.c_str(),getName().c_str(),Py_file_input);
  if(res == NULL)
    {
      std::string error="";
      PyObject* new_stderr = newPyStdOut(error);
      PySys_SetObject((char*)"stderr", new_stderr);
      PyErr_Print();
      PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
      Py_DECREF(new_stderr);
      PyGILState_Release(gstate);
      throw Exception(error);
    }
  else
    Py_XDECREF(res);
  PyGILState_Release(gstate);
}

void PythonNode::load()
{
}

void PythonNode::execute()
{
  DEBTRACE( "++++++++++++++ PyNode::execute: " << getName() << " ++++++++++++++++++++" );
  PyGILState_STATE gstate = PyGILState_Ensure();
  if( PyDict_SetItemString( _context, "__builtins__", getSALOMERuntime()->getBuiltins() ))
    {
      stringstream msg;
      msg << "Impossible to set builtins" << __FILE__ << ":" << __LINE__;
      PyGILState_Release(gstate);
      _errorDetails=msg.str();
      throw Exception(msg.str());
    }

  DEBTRACE( "---------------PyNode::inputs---------------" );
  list<InputPort *>::iterator iter2;
  for(iter2 = _setOfInputPort.begin(); iter2 != _setOfInputPort.end(); iter2++)
    {
      InputPyPort *p=(InputPyPort *)*iter2;
      DEBTRACE( "port name: " << p->getName() );
      DEBTRACE( "port kind: " << p->edGetType()->kind() );
      PyObject* ob=p->getPyObj();
      DEBTRACE( "ob refcnt: " << ob->ob_refcnt );
#ifdef _DEVDEBUG_
      PyObject_Print(ob,stderr,Py_PRINT_RAW);
      cerr << endl;
#endif
      int ier=PyDict_SetItemString(_context,p->getName().c_str(),ob);
      DEBTRACE( "after PyDict_SetItemString:ob refcnt: " << ob->ob_refcnt );
    }
  
  DEBTRACE( "---------------End PyNode::inputs---------------" );
  
  //calculation
  DEBTRACE( "----------------PyNode::calculation---------------" );
  DEBTRACE(  _script );
  DEBTRACE( "_context refcnt: " << _context->ob_refcnt );

  std::ostringstream stream;
  stream << "/tmp/PythonNode_";
  stream << getpid();

  PyObject* code=Py_CompileString(_script.c_str(), stream.str().c_str(), Py_file_input);
  if(code == NULL)
    {
      _errorDetails="";
      PyObject* new_stderr = newPyStdOut(_errorDetails);
      PySys_SetObject((char*)"stderr", new_stderr);
      PyErr_Print();
      PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
      Py_DECREF(new_stderr);
      PyGILState_Release(gstate);
      throw Exception("Error during execution");
    }
  PyObject *res = PyEval_EvalCode((PyCodeObject *)code, _context, _context);
  Py_DECREF(code);
  DEBTRACE( "_context refcnt: " << _context->ob_refcnt );
  fflush(stdout);
  fflush(stderr);
  if(res == NULL)
    {
      _errorDetails="";
      PyObject* new_stderr = newPyStdOut(_errorDetails);
      PySys_SetObject((char*)"stderr", new_stderr);
      ofstream errorfile(stream.str().c_str());
      if (errorfile.is_open())
        {
          errorfile << _script;
          errorfile.close();
        }
      PyErr_Print();
      PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
      Py_DECREF(new_stderr);

      PyGILState_Release(gstate);
      throw Exception("Error during execution");
    }
  Py_DECREF(res);
  
  DEBTRACE( "-----------------PyNode::outputs-----------------" );
  list<OutputPort *>::iterator iter;
  try
    {
      for(iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); iter++)
        {
          OutputPyPort *p=(OutputPyPort *)*iter;
          DEBTRACE( "port name: " << p->getName() );
          DEBTRACE( "port kind: " << p->edGetType()->kind() );
          PyObject *ob=PyDict_GetItemString(_context,p->getName().c_str());
          if(ob==NULL){
             PyGILState_Release(gstate);
             std::string msg="Error during execution: there is no variable ";
             msg=msg+p->getName()+" in node context";
             _errorDetails=msg;
             throw Exception(msg);
          }
          DEBTRACE( "PyNode::outputs::ob refcnt: " << ob->ob_refcnt );
#ifdef _DEVDEBUG_
          PyObject_Print(ob,stderr,Py_PRINT_RAW);
          cerr << endl;
#endif
          p->put(ob);
        }
    }
  catch(ConversionException& ex)
    {
      PyGILState_Release(gstate);
      _errorDetails=ex.what();
      throw;
    }

  DEBTRACE( "-----------------End PyNode::outputs-----------------" );
  PyGILState_Release(gstate);
  DEBTRACE( "++++++++++++++ End PyNode::execute: " << getName() << " ++++++++++++++++++++" );
}

Node *PythonNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new PythonNode(*this,father);
}

//! Create a new node of same type with a given name
PythonNode* PythonNode::cloneNode(const std::string& name)
{
  PythonNode* n=new PythonNode(name);
  n->setScript(_script);
  list<InputPort *>::iterator iter;
  for(iter = _setOfInputPort.begin(); iter != _setOfInputPort.end(); iter++)
    {
      InputPyPort *p=(InputPyPort *)*iter;
      DEBTRACE( "port name: " << p->getName() );
      DEBTRACE( "port kind: " << p->edGetType()->kind() );
      n->edAddInputPort(p->getName(),p->edGetType());
    }
  list<OutputPort *>::iterator iter2;
  for(iter2 = _setOfOutputPort.begin(); iter2 != _setOfOutputPort.end(); iter2++)
    {
      OutputPyPort *p=(OutputPyPort *)*iter2;
      DEBTRACE( "port name: " << p->getName() );
      DEBTRACE( "port kind: " << p->edGetType()->kind() );
      n->edAddOutputPort(p->getName(),p->edGetType());
    }
  return n;
}

PyFuncNode::PyFuncNode(const PyFuncNode& other, ComposedNode *father):InlineFuncNode(other,father),_pyfunc(0)
{
  _implementation = PythonNode::IMPL_NAME;
  PyGILState_STATE gstate = PyGILState_Ensure();
  _context=PyDict_New();
  DEBTRACE( "_context refcnt: " << _context->ob_refcnt );
  if( PyDict_SetItemString( _context, "__builtins__", getSALOMERuntime()->getBuiltins() ))
    {
      stringstream msg;
      msg << "Not possible to set builtins" << __FILE__ << ":" << __LINE__;
      _errorDetails=msg.str();
      PyGILState_Release(gstate);
      throw Exception(msg.str());
    }
  PyGILState_Release(gstate);
}

PyFuncNode::PyFuncNode(const std::string& name): InlineFuncNode(name),_pyfunc(0)
{

  _implementation = PythonNode::IMPL_NAME;
  DEBTRACE( "PyFuncNode::PyFuncNode " << name );
  PyGILState_STATE gstate = PyGILState_Ensure();
  _context=PyDict_New();
  DEBTRACE( "_context refcnt: " << _context->ob_refcnt );
  if( PyDict_SetItemString( _context, "__builtins__", getSALOMERuntime()->getBuiltins() ))
    {
      stringstream msg;
      msg << "Not possible to set builtins" << __FILE__ << ":" << __LINE__;
      _errorDetails=msg.str();
      PyGILState_Release(gstate);
      throw Exception(msg.str());
    }
  PyGILState_Release(gstate);
}

PyFuncNode::~PyFuncNode()
{
  DEBTRACE( getName() );
  PyGILState_STATE gstate = PyGILState_Ensure();
  DEBTRACE( "_context refcnt: " << _context->ob_refcnt );
  if(_pyfunc)DEBTRACE( "_pyfunc refcnt: " << _pyfunc->ob_refcnt );
  Py_DECREF(_context);
  PyGILState_Release(gstate);
  if(!CORBA::is_nil(_pynode))
    {
      _pynode->Destroy();
    }
}

void PyFuncNode::checkBasicConsistency() const throw(YACS::Exception)
{
  DEBTRACE("checkBasicConsistency");
  InlineFuncNode::checkBasicConsistency();

  PyGILState_STATE gstate = PyGILState_Ensure();
  PyObject* res;
  res=Py_CompileString(_script.c_str(),getName().c_str(),Py_file_input);
  if(res == NULL)
    {
      std::string error="";
      PyObject* new_stderr = newPyStdOut(error);
      PySys_SetObject((char*)"stderr", new_stderr);
      PyErr_Print();
      PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
      Py_DECREF(new_stderr);
      PyGILState_Release(gstate);
      throw Exception(error);
    }
  else
    Py_XDECREF(res);
  PyGILState_Release(gstate);
}

void PyFuncNode::load()
{
  if(_mode=="remote")
    loadRemote();
  else
    loadLocal();
}

void PyFuncNode::loadRemote()
{
  DEBTRACE( "---------------PyfuncNode::loadRemote function---------------" );
  if(_container)
    {
      if(!_container->isAlreadyStarted(0))
        {
          try
            {
              _container->start(0);
            }
          catch(Exception& e)
            {
              _errorDetails=e.what();
              throw e;
            }
        }
    }
  else
    {
      std::string what("PyFuncNode::loadRemote : a load operation requested on \"");
      what+=_name; what+="\" with no container specified.";
      _errorDetails=what;
      throw Exception(what);
    }

  Engines::Container_var objContainer=((SalomeContainer*)_container)->getContainerPtr(0);
  try
    {
      _pynode = objContainer->createPyNode(getName().c_str(),getScript().c_str());
    }
  catch( const SALOME::SALOME_Exception& ex )
    {
      std::string msg="Exception on remote python node creation ";
      msg += '\n';
      msg += ex.details.text.in();
      _errorDetails=msg;
      throw Exception(msg);
    }

  PyGILState_STATE gstate = PyGILState_Ensure();
  const char picklizeScript[]="import cPickle\n"
                              "def pickleForDistPyth2009(*args,**kws):\n"
                              "  return cPickle.dumps((args,kws),-1)\n"
                              "\n"
                              "def unPickleForDistPyth2009(st):\n"
                              "  args=cPickle.loads(st)\n"
                              "  return args\n";
  PyObject *res=PyRun_String(picklizeScript,Py_file_input,_context,_context);
  if(res == NULL)
    {
      _errorDetails="";
      PyObject* new_stderr = newPyStdOut(_errorDetails);
      PySys_SetObject((char*)"stderr", new_stderr);
      PyErr_Print();
      PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
      Py_DECREF(new_stderr);

      PyGILState_Release(gstate);
      throw Exception("Error during load");
    }
  Py_DECREF(res);

  _pyfuncSer=PyDict_GetItemString(_context,"pickleForDistPyth2009");
  _pyfuncUnser=PyDict_GetItemString(_context,"unPickleForDistPyth2009");
  if(_pyfuncSer == NULL)
    {
      _errorDetails="";
      PyObject* new_stderr = newPyStdOut(_errorDetails);
      PySys_SetObject((char*)"stderr", new_stderr);
      PyErr_Print();
      PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
      Py_DECREF(new_stderr);

      PyGILState_Release(gstate);
      throw Exception("Error during load");
    }
  if(_pyfuncUnser == NULL)
    {
      _errorDetails="";
      PyObject* new_stderr = newPyStdOut(_errorDetails);
      PySys_SetObject((char*)"stderr", new_stderr);
      PyErr_Print();
      PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
      Py_DECREF(new_stderr);

      PyGILState_Release(gstate);
      throw Exception("Error during load");
    }
  DEBTRACE( "---------------End PyfuncNode::loadRemote function---------------" );
  PyGILState_Release(gstate);


}

void PyFuncNode::loadLocal()
{
  DEBTRACE( "---------------PyFuncNode::load function " << getName() << " ---------------" );
  DEBTRACE(  _script );

#ifdef _DEVDEBUG_
  list<OutputPort *>::iterator iter;
  for(iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); iter++)
    {
      OutputPyPort *p=(OutputPyPort *)*iter;
      DEBTRACE( "port name: " << p->getName() );
      DEBTRACE( "port kind: " << p->edGetType()->kind() );
    }
#endif

  PyGILState_STATE gstate = PyGILState_Ensure();
  DEBTRACE( "_context refcnt: " << _context->ob_refcnt );

  std::ostringstream stream;
  stream << "/tmp/PythonNode_";
  stream << getpid();

  PyObject* code=Py_CompileString(_script.c_str(), stream.str().c_str(), Py_file_input);
  if(code == NULL)
    {
      _errorDetails="";
      PyObject* new_stderr = newPyStdOut(_errorDetails);
      PySys_SetObject((char*)"stderr", new_stderr);
      PyErr_Print();
      PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
      Py_DECREF(new_stderr);
      PyGILState_Release(gstate);
      throw Exception("Error during execution");
    }
  PyObject *res = PyEval_EvalCode((PyCodeObject *)code, _context, _context);
  Py_DECREF(code);

  DEBTRACE( "_context refcnt: " << _context->ob_refcnt );
  if(res == NULL)
    {
      _errorDetails="";
      PyObject* new_stderr = newPyStdOut(_errorDetails);
      PySys_SetObject((char*)"stderr", new_stderr);
      ofstream errorfile(stream.str().c_str());
      if (errorfile.is_open())
        {
          errorfile << _script;
          errorfile.close();
        }
      PyErr_Print();
      PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
      Py_DECREF(new_stderr);

      PyGILState_Release(gstate);
      throw Exception("Error during execution");
      return;
    }
  Py_DECREF(res);
  _pyfunc=PyDict_GetItemString(_context,_fname.c_str());
  DEBTRACE( "_pyfunc refcnt: " << _pyfunc->ob_refcnt );
  if(_pyfunc == NULL)
    {
      _errorDetails="";
      PyObject* new_stderr = newPyStdOut(_errorDetails);
      PySys_SetObject((char*)"stderr", new_stderr);
      PyErr_Print();
      PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
      Py_DECREF(new_stderr);

      PyGILState_Release(gstate);
      throw Exception("Error during execution");
    }
  DEBTRACE( "---------------End PyFuncNode::load function---------------" );
  PyGILState_Release(gstate);
}

void PyFuncNode::execute()
{
  if(_mode=="remote")
    executeRemote();
  else
    executeLocal();
}

void PyFuncNode::executeRemote()
{
  DEBTRACE( "++++++++++++++ PyFuncNode::executeRemote: " << getName() << " ++++++++++++++++++++" );
  if(!_pyfuncSer)
    throw Exception("DistributedPythonNode badly loaded");
  PyGILState_STATE gstate = PyGILState_Ensure();

  //===========================================================================
  // Get inputs in input ports, build a Python tuple and pickle it
  //===========================================================================
  PyObject* ob;
  PyObject* args = PyTuple_New(getNumberOfInputPorts());
  std::list<InputPort *>::iterator iter2;
  int pos=0;
  for(iter2 = _setOfInputPort.begin(); iter2 != _setOfInputPort.end(); ++iter2)
    {
      InputPyPort *p=(InputPyPort *)*iter2;
      ob=p->getPyObj();
      Py_INCREF(ob);
      PyTuple_SetItem(args,pos,ob);
      pos++;
    }
#ifdef _DEVDEBUG_
  PyObject_Print(args,stderr,Py_PRINT_RAW);
  std::cerr << endl;
#endif
  PyObject *serializationInput=PyObject_CallObject(_pyfuncSer,args);
  //The pickled string may contain NULL characters so use PyString_AsStringAndSize
  char* serializationInputC;
  Py_ssize_t len;
  if (PyString_AsStringAndSize(serializationInput, &serializationInputC, &len))
    {
      PyGILState_Release(gstate);
      throw Exception("DistributedPythonNode problem in python pickle");
    }

  Engines::pickledArgs_var serializationInputCorba=new Engines::pickledArgs;
  serializationInputCorba->length(len);
  for(int i=0; i < len ; i++)
    serializationInputCorba[i]=serializationInputC[i];

  //===========================================================================
  // Execute in remote Python node
  //===========================================================================
  DEBTRACE( "-----------------starting remote python invocation-----------------" );
  Engines::pickledArgs_var resultCorba;
  try
    {
      resultCorba=_pynode->execute(getFname().c_str(),serializationInputCorba);
    }
  catch( const SALOME::SALOME_Exception& ex )
    {
      std::string msg="Exception on remote python invocation";
      msg += '\n';
      msg += ex.details.text.in();
      _errorDetails=msg;
      PyGILState_Release(gstate);
      throw Exception(msg);
    }
  DEBTRACE( "-----------------end of remote python invocation-----------------" );
  //===========================================================================
  // Get results, unpickle and put them in output ports
  //===========================================================================
  char *resultCorbaC=new char[resultCorba->length()+1];
  resultCorbaC[resultCorba->length()]='\0';
  for(int i=0;i<resultCorba->length();i++)
    resultCorbaC[i]=resultCorba[i];

  PyObject* resultPython=PyString_FromStringAndSize(resultCorbaC,resultCorba->length());
  delete [] resultCorbaC;
  args = PyTuple_New(1);
  PyTuple_SetItem(args,0,resultPython);
  PyObject *finalResult=PyObject_CallObject(_pyfuncUnser,args);
  Py_DECREF(args);

  DEBTRACE( "-----------------PythonNode::outputs-----------------" );
  int nres=1;
  if(finalResult == Py_None)
    nres=0;
  else if(PyTuple_Check(finalResult))
    nres=PyTuple_Size(finalResult);

  if(getNumberOfOutputPorts() != nres)
    {
      std::string msg="Number of output arguments : Mismatch between definition and execution";
      Py_DECREF(finalResult);
      PyGILState_Release(gstate);
      _errorDetails=msg;
      throw Exception(msg);
    }

  pos=0;
  std::list<OutputPort *>::iterator iter;
  try
    {
      for(iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); ++iter)
        {
          OutputPyPort *p=(OutputPyPort *)*iter;
          DEBTRACE( "port name: " << p->getName() );
          DEBTRACE( "port kind: " << p->edGetType()->kind() );
          DEBTRACE( "port pos : " << pos );
          if(PyTuple_Check(finalResult))
            ob=PyTuple_GetItem(finalResult,pos) ;
          else 
            ob=finalResult;
          DEBTRACE( "ob refcnt: " << ob->ob_refcnt );
          p->put(ob);
          pos++;
        }
      Py_DECREF(finalResult);
    }
  catch(ConversionException& ex)
    {
      Py_DECREF(finalResult);
      PyGILState_Release(gstate);
      _errorDetails=ex.what();
      throw;
    }

  PyGILState_Release(gstate);

  DEBTRACE( "++++++++++++++ ENDOF PyFuncNode::executeRemote: " << getName() << " ++++++++++++++++++++" );
}

void PyFuncNode::executeLocal()
{
  DEBTRACE( "++++++++++++++ PyFuncNode::execute: " << getName() << " ++++++++++++++++++++" );

  int pos=0;
  PyObject* ob;
  if(!_pyfunc)throw Exception("PyFuncNode badly loaded");
  PyGILState_STATE gstate = PyGILState_Ensure();

  DEBTRACE( "---------------PyFuncNode::inputs---------------" );
  PyObject* args = PyTuple_New(getNumberOfInputPorts()) ;
  list<InputPort *>::iterator iter2;
  for(iter2 = _setOfInputPort.begin(); iter2 != _setOfInputPort.end(); iter2++)
    {
      InputPyPort *p=(InputPyPort *)*iter2;
      DEBTRACE( "port name: " << p->getName() );
      DEBTRACE( "port kind: " << p->edGetType()->kind() );
      ob=p->getPyObj();
#ifdef _DEVDEBUG_
      PyObject_Print(ob,stderr,Py_PRINT_RAW);
      cerr << endl;
#endif
      DEBTRACE( "ob refcnt: " << ob->ob_refcnt );
      Py_INCREF(ob);
      PyTuple_SetItem(args,pos,ob);
      DEBTRACE( "ob refcnt: " << ob->ob_refcnt );
      pos++;
    }
  DEBTRACE( "---------------End PyFuncNode::inputs---------------" );

  DEBTRACE( "----------------PyFuncNode::calculation---------------" );
#ifdef _DEVDEBUG_
  PyObject_Print(_pyfunc,stderr,Py_PRINT_RAW);
  cerr << endl;
  PyObject_Print(args,stderr,Py_PRINT_RAW);
  cerr << endl;
#endif
  DEBTRACE( "_pyfunc refcnt: " << _pyfunc->ob_refcnt );
  PyObject* result = PyObject_CallObject( _pyfunc , args ) ;
  DEBTRACE( "_pyfunc refcnt: " << _pyfunc->ob_refcnt );
  Py_DECREF(args);
  fflush(stdout);
  fflush(stderr);
  if(result == NULL)
    {
      _errorDetails="";
      PyObject* new_stderr = newPyStdOut(_errorDetails);
      PySys_SetObject((char*)"stderr", new_stderr);
      std::ostringstream stream;
      stream << "/tmp/PythonNode_";
      stream << getpid();
      ofstream errorfile(stream.str().c_str());
      if (errorfile.is_open())
        {
          errorfile << _script;
          errorfile.close();
        }
      PyErr_Print();
      PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
      Py_DECREF(new_stderr);

      PyGILState_Release(gstate);
      throw Exception("Error during execution");
    }
  DEBTRACE( "----------------End PyFuncNode::calculation---------------" );

  DEBTRACE( "-----------------PyFuncNode::outputs-----------------" );
  int nres=1;
  if(result == Py_None)
    nres=0;
  else if(PyTuple_Check(result))
    nres=PyTuple_Size(result);

  if(getNumberOfOutputPorts() != nres)
    {
      std::string msg="Number of output arguments : Mismatch between definition and execution";
      Py_DECREF(result);
      PyGILState_Release(gstate);
      _errorDetails=msg;
      throw Exception(msg);
    }

  pos=0;
#ifdef _DEVDEBUG_
  PyObject_Print(result,stderr,Py_PRINT_RAW);
  cerr << endl;
#endif
  list<OutputPort *>::iterator iter;
  try
    {
      for(iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); iter++)
        {
          OutputPyPort *p=(OutputPyPort *)*iter;
          DEBTRACE( "port name: " << p->getName() );
          DEBTRACE( "port kind: " << p->edGetType()->kind() );
          DEBTRACE( "port pos : " << pos );
          if(PyTuple_Check(result))ob=PyTuple_GetItem(result,pos) ;
          else ob=result;
          DEBTRACE( "ob refcnt: " << ob->ob_refcnt );
#ifdef _DEVDEBUG_
          PyObject_Print(ob,stderr,Py_PRINT_RAW);
          cerr << endl;
#endif
          p->put(ob);
          pos++;
        }
    }
  catch(ConversionException& ex)
    {
      Py_DECREF(result);
      PyGILState_Release(gstate);
      _errorDetails=ex.what();
      throw;
    }
  DEBTRACE( "-----------------End PyFuncNode::outputs-----------------" );

  Py_DECREF(result);
  PyGILState_Release(gstate);
  DEBTRACE( "++++++++++++++ End PyFuncNode::execute: " << getName() << " ++++++++++++++++++++" );
}

Node *PyFuncNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new PyFuncNode(*this,father);
}

//! Create a new node of same type with a given name
PyFuncNode* PyFuncNode::cloneNode(const std::string& name)
{
  PyFuncNode* n=new PyFuncNode(name);
  n->setScript(_script);
  n->setFname(_fname);
  list<InputPort *>::iterator iter;
  for(iter = _setOfInputPort.begin(); iter != _setOfInputPort.end(); iter++)
    {
      InputPyPort *p=(InputPyPort *)*iter;
      n->edAddInputPort(p->getName(),p->edGetType());
    }
  list<OutputPort *>::iterator iter2;
  for(iter2 = _setOfOutputPort.begin(); iter2 != _setOfOutputPort.end(); iter2++)
    {
      OutputPyPort *p=(OutputPyPort *)*iter2;
      n->edAddOutputPort(p->getName(),p->edGetType());
    }
  return n;
}

std::string PyFuncNode::getContainerLog()
{
  if(_mode=="local")return "";

  std::string msg;
  try
    {
      Engines::Container_var objContainer=((SalomeContainer*)_container)->getContainerPtr(0);
      CORBA::String_var logname = objContainer->logfilename();
      DEBTRACE(logname);
      msg=logname;
      std::string::size_type pos = msg.find(":");
      msg=msg.substr(pos+1);
    }
  catch(...)
    {
      msg = "Container no longer reachable";
    }
  return msg;
}
