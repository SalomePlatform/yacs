
#include "RuntimeSALOME.hxx"
#include "PythonNode.hxx"
#include "PythonPorts.hxx"

#include <iostream>
#include <sstream>

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
  PyObject *res=PyRun_String(_script.c_str(),Py_file_input,_context,_context);
  DEBTRACE( "_context refcnt: " << _context->ob_refcnt );
  if(res == NULL)
    {
      PyErr_Print();
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
  catch(ConversionException)
    {
      PyGILState_Release(gstate);
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
}

void PyFuncNode::load()
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
  PyObject *res=PyRun_String(_script.c_str(),Py_file_input,_context,_context);
  DEBTRACE( "_context refcnt: " << _context->ob_refcnt );
  if(res == NULL)
    {
      PyErr_Print();
      PyGILState_Release(gstate);
      throw Exception("Error during execution");
      return;
    }
  Py_DECREF(res);
  _pyfunc=PyDict_GetItemString(_context,_fname.c_str());
  DEBTRACE( "_pyfunc refcnt: " << _pyfunc->ob_refcnt );
  if(_pyfunc == NULL)
    {
      PyErr_Print();
      PyGILState_Release(gstate);
      throw Exception("Error during execution");
    }
  DEBTRACE( "---------------End PyFuncNode::load function---------------" );
  PyGILState_Release(gstate);
}

void PyFuncNode::execute()
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
  if(result == NULL)
    {
      PyErr_Print();
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
      Py_DECREF(result);
      PyGILState_Release(gstate);
      throw Exception("Number of output arguments : Mismatch between definition and execution");
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
  catch(ConversionException)
    {
      Py_DECREF(result);
      PyGILState_Release(gstate);
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
