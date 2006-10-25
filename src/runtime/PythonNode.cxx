
#include "PythonNode.hxx"
#include "RuntimeSALOME.hxx"

#include <iostream>
#include <sstream>

using namespace YACS::ENGINE;
using namespace std;

PythonNode::PythonNode(const string& name): ElementaryNode(name)
{
  _implementation = "Python";
  cerr << "PythonNode::PythonNode " << name << endl;
}

void PythonNode::set_script(const string& script)
{
  _script = script;
}

void PythonNode::execute()
{
  cerr << "PyNode::run" << endl;
  PyObject* context=PyDict_New();
  if( PyDict_SetItemString( context, "__builtins__", getSALOMERuntime()->getBuiltins() ))
    {
      stringstream msg;
      msg << "Impossible to set builtins" << __FILE__ << ":" << __LINE__;
      throw Exception(msg.str());
    }

  cerr << "---------------PyNode::inputs---------------" << endl;
  set<InputPort *>::iterator iter2;
  for(iter2 = _setOfInputPort.begin(); iter2 != _setOfInputPort.end(); iter2++)
    {
      InputPyPort *p=(InputPyPort *)*iter2;
      cerr << "port name: " << p->getName() << endl;
      cerr << "port kind: " << p->type()->kind() << endl;
      PyObject* ob=p->getPyObj();
      cerr << "ob refcnt: " << ob->ob_refcnt << endl;
      PyObject_Print(ob,stdout,Py_PRINT_RAW);
      cerr << endl;
      int ier=PyDict_SetItemString(context,p->getName().c_str(),ob);
      cerr << "ob refcnt: " << ob->ob_refcnt << endl;
    }
  
  cerr << "---------------End PyNode::inputs---------------" << endl;
  
  //calculation
  cerr << "PyNode::calculation " << _script << endl;
  PyObject *res=PyRun_String(_script.c_str(),Py_file_input,context,context);
  if(res == NULL)
    {
      PyErr_Print();
      return;
    }
  Py_DECREF(res);
  
  cerr << "-----------------PyNode::outputs-----------------" << endl;
  set<OutputPort *>::iterator iter;
  for(iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); iter++)
    {
      OutputPyPort *p=(OutputPyPort *)*iter;
      cerr << "port name: " << p->getName() << endl;
      cerr << "port kind: " << p->type()->kind() << endl;
      PyObject *ob=PyDict_GetItemString(context,p->getName().c_str());
      cerr << "ob refcnt: " << ob->ob_refcnt << endl;
      PyObject_Print(ob,stdout,Py_PRINT_RAW);
      cerr << endl;
      //Faut-il incrémenter ici ?
      Py_INCREF(ob);
      cerr << "ob refcnt: " << ob->ob_refcnt << endl;
      p->put(ob);
    }

  cerr << "-----------------End PyNode::outputs-----------------" << endl;
  Py_DECREF(context);
}
