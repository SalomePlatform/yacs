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

#include "RuntimeSALOME.hxx"
#include "SalomePythonComponent.hxx"
#include "SalomePythonNode.hxx"
#include "PythonNode.hxx"
#include "PythonPorts.hxx"
#include "CORBANode.hxx"
#include "TypeCode.hxx"

#include <iostream>
#include <sstream>

using namespace YACS::ENGINE;
using namespace std;

const char SalomePythonNode::PLACEMENT_VAR_NAME_IN_INTERP[]="__container__from__YACS__";

SalomePythonNode::SalomePythonNode(const SalomePythonNode& other, ComposedNode *father):ServiceInlineNode(other,father),_pyfunc(0),_context(0)
{
  //Not a bug : just because port point of view this is like PythonNode.
  _implementation = PythonNode::IMPL_NAME;
  PyGILState_STATE gstate = PyGILState_Ensure();
  _context=PyDict_New();
  if( PyDict_SetItemString( _context, "__builtins__", getSALOMERuntime()->getBuiltins() ))
    {
      stringstream msg;
      msg << "Not possible to set builtins" << __FILE__ << ":" << __LINE__;
      PyGILState_Release(gstate);
      throw Exception(msg.str());
    }
  PyGILState_Release(gstate);
}

SalomePythonNode::SalomePythonNode(const std::string& name): ServiceInlineNode(name),_pyfunc(0)
{

  //Not a bug : just because port point of view this is like PythonNode.
  _implementation = PythonNode::IMPL_NAME;
  PyGILState_STATE gstate = PyGILState_Ensure();
  _context=PyDict_New();
  if( PyDict_SetItemString( _context, "__builtins__", getSALOMERuntime()->getBuiltins() ))
    {
      stringstream msg;
      msg << "Not possible to set builtins" << __FILE__ << ":" << __LINE__;
      PyGILState_Release(gstate);
      throw Exception(msg.str());
    }
  PyGILState_Release(gstate);
}

void SalomePythonNode::load()
{
  ServiceInlineNode::load();
  cerr << "---------------SalomePythonNode::load function---------------" << endl;
  list<OutputPort *>::iterator iter;
  string value2Export=((SalomePythonComponent*)_component)->getStringValueToExportInInterp(this);
  PyObject* ob=PyString_FromString(value2Export.c_str());
  PyDict_SetItemString(_context,PLACEMENT_VAR_NAME_IN_INTERP,ob);
  for(iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); iter++)
    {
      OutputPyPort *p=(OutputPyPort *)*iter;
      cerr << "port name: " << p->getName() << endl;
      cerr << "port kind: " << p->edGetType()->kind() << endl;
    }
  cerr <<  _script << endl;
  PyGILState_STATE gstate = PyGILState_Ensure();
  PyObject *res=PyRun_String(_script.c_str(),Py_file_input,_context,_context);
  if(res == NULL)
    {
      PyErr_Print();
      PyGILState_Release(gstate);
      throw Exception("Error during execution");
      return;
    }
  Py_DECREF(res);
  _pyfunc=PyDict_GetItemString(_context,_method.c_str());
  if(_pyfunc == NULL)
    {
      PyErr_Print();
      PyGILState_Release(gstate);
      throw Exception("Error during execution");
    }
  cerr << "---------------End SalomePythonNode::load function---------------" << endl;
  PyGILState_Release(gstate);
}

void SalomePythonNode::execute()
{
  cerr << "++++++++++++++ SalomePythonNode::execute: " << getName() << " ++++++++++++++++++++" << endl;
  int pos=0;
  PyObject* ob;
  if(!_pyfunc)throw Exception("SalomePythonNode badly loaded");
  PyGILState_STATE gstate = PyGILState_Ensure();
  
  cerr << "---------------SalomePythonNode::inputs---------------" << endl;
  PyObject* args = PyTuple_New(getNumberOfInputPorts()) ;
  list<InputPort *>::iterator iter2;
  for(iter2 = _setOfInputPort.begin(); iter2 != _setOfInputPort.end(); iter2++)
    {
      InputPyPort *p=(InputPyPort *)*iter2;
      cerr << "port name: " << p->getName() << endl;
      cerr << "port kind: " << p->edGetType()->kind() << endl;
      ob=p->getPyObj();
      PyObject_Print(ob,stderr,Py_PRINT_RAW);
      cerr << endl;
      cerr << "ob refcnt: " << ob->ob_refcnt << endl;
      Py_INCREF(ob);
      PyTuple_SetItem(args,pos,ob);
      cerr << "ob refcnt: " << ob->ob_refcnt << endl;
      pos++;
    }
  cerr << "---------------End SalomePythonNode::inputs---------------" << endl;

  cerr << "----------------SalomePythonNode::calculation---------------" << endl;
  PyObject_Print(_pyfunc,stderr,Py_PRINT_RAW);
  cerr << endl;
  PyObject_Print(args,stderr,Py_PRINT_RAW);
  cerr << endl;
  PyObject* result = PyObject_CallObject( _pyfunc , args ) ;
  Py_DECREF(args);
  if(result == NULL)
    {
      PyErr_Print();
      PyGILState_Release(gstate);
      throw Exception("Error during execution");
    }
  cerr << "----------------End SalomePythonNode::calculation---------------" << endl;

  cerr << "-----------------SalomePythonNode::outputs-----------------" << endl;
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
  PyObject_Print(result,stderr,Py_PRINT_RAW);
  cerr << endl;
  list<OutputPort *>::iterator iter;
  try
    {
      for(iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); iter++)
        {
          OutputPyPort *p=(OutputPyPort *)*iter;
          cerr << "port name: " << p->getName() << endl;
          cerr << "port kind: " << p->edGetType()->kind() << endl;
          cerr << "port pos : " << pos << endl;
          if(PyTuple_Check(result))ob=PyTuple_GetItem(result,pos) ;
          else ob=result;
          cerr << "ob refcnt: " << ob->ob_refcnt << endl;
          PyObject_Print(ob,stderr,Py_PRINT_RAW);
          cerr << endl;
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
  cerr << "-----------------End SalomePythonNode::outputs-----------------" << endl;

  Py_DECREF(result);
  PyGILState_Release(gstate);
  cerr << "++++++++++++++ End SalomePythonNode::execute: " << getName() << " ++++++++++++++++++++" << endl;
}

std::string SalomePythonNode::getKind() const
{
  static const char LOC_KIND[]="";
  return LOC_KIND;
}

Node *SalomePythonNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new SalomePythonNode(*this,father);
}

ServiceNode *SalomePythonNode::createNode(const std::string &name)
{
  ServiceNode* node=new SalomePythonNode(name);
  node->setComponent(_component);
  return node;
}

//! Create a new node of same type with a given name
SalomePythonNode* SalomePythonNode::cloneNode(const std::string& name)
{
  SalomePythonNode* n=new SalomePythonNode(name);
  n->setScript(_script);
  n->setMethod(_method);
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
