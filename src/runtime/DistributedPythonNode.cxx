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

#include "DistributedPythonNode.hxx"
#include "RuntimeSALOME.hxx"
#include "SalomeContainer.hxx"
#include "PythonNode.hxx"
#include "SalomeHPContainer.hxx"
#include "SalomeContainerTmpForHP.hxx"
#include "AutoGIL.hxx"

#include "PythonPorts.hxx"
#include "YacsTrace.hxx"
#include "PyStdout.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char DistributedPythonNode::KIND[]="DistPython";
const char DistributedPythonNode::IMPL_NAME[]="Python";
const char DistributedPythonNode::SALOME_CONTAINER_METHOD_IDL[]="createPyNode";

Node *DistributedPythonNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new DistributedPythonNode(*this,father);
}

DistributedPythonNode::DistributedPythonNode(const std::string& name):ServerNode(name),_context(0),_pyfuncSer(0),_pyfuncUnser(0)
{
  initMySelf();
}

DistributedPythonNode::DistributedPythonNode(const DistributedPythonNode& other, ComposedNode *father):ServerNode(other,father),_context(0),_pyfuncSer(0),_pyfuncUnser(0)
{
  initMySelf();
}

DistributedPythonNode::~DistributedPythonNode()
{
  AutoGIL agil;
  Py_DECREF(_context);
}

void DistributedPythonNode::load()
{
  bool isContAlreadyStarted(false);
  if(_container)
    isContAlreadyStarted=_container->isAlreadyStarted(this);
  ServerNode::load();
  {
    AutoGIL agil;
    if( PyDict_SetItemString( _context, "__builtins__", getSALOMERuntime()->getBuiltins() ))
      {
        stringstream msg;
        msg << "Impossible to set builtins" << __FILE__ << ":" << __LINE__;
        _errorDetails=msg.str();
        throw Exception(msg.str());
      }
    const char picklizeScript[]="import cPickle\ndef pickleForDistPyth2009(*args,**kws):\n  return cPickle.dumps((args,kws),-1)\n\ndef unPickleForDistPyth2009(st):\n  args=cPickle.loads(st)\n  return args\n";
    PyObject *res=PyRun_String(picklizeScript,Py_file_input,_context,_context);
    if(res == NULL)
      {
        _errorDetails="";
        PyObject* new_stderr = newPyStdOut(_errorDetails);
        PySys_SetObject((char*)"stderr", new_stderr);
        PyErr_Print();
        PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
        Py_DECREF(new_stderr);
        throw Exception("Error during execution");
        return;
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
        throw Exception("Error during execution");
      }
    if(_pyfuncUnser == NULL)
      {
        _errorDetails="";
        PyObject* new_stderr = newPyStdOut(_errorDetails);
        PySys_SetObject((char*)"stderr", new_stderr);
        PyErr_Print();
        PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
        Py_DECREF(new_stderr);
        throw Exception("Error during execution");
      }

    Engines::Container_var objContainer=Engines::Container::_nil();
    if(!_container)
      throw Exception("No container specified !");
    SalomeContainer *containerCast0(dynamic_cast<SalomeContainer *>(_container));
    SalomeHPContainer *containerCast1(dynamic_cast<SalomeHPContainer *>(_container));
    if(containerCast0)
      objContainer=containerCast0->getContainerPtr(this);
    else if(containerCast1)
      {
        YACS::BASES::AutoCppPtr<SalomeContainerTmpForHP> tmpCont(SalomeContainerTmpForHP::BuildFrom(containerCast1,this));
        objContainer=tmpCont->getContainerPtr(this);
      }
    else
      throw Exception("Unrecognized type of container ! Salome one is expected !");
    if(CORBA::is_nil(objContainer))
      throw Exception("Container corba pointer is NULL !");

    try
    {
        if(containerCast0 || !isContAlreadyStarted)
          {
            _pynode = objContainer->createPyNode(getName().c_str(),getScript().c_str());
          }
        else
          {
            Engines::PyNode_var dftPyScript(objContainer->getDefaultPyNode(getName().c_str()));
            if(CORBA::is_nil(dftPyScript))
              _pynode = objContainer->createPyNode(getName().c_str(),getScript().c_str());
            else
              _pynode = dftPyScript;
          }
    }
    catch( const SALOME::SALOME_Exception& ex )
    {
        std::string msg="Exception on remote python node creation ";
        msg += '\n';
        msg += ex.details.text.in();
        _errorDetails=msg;
        throw Exception(msg);
    }

    if(CORBA::is_nil(_pynode))
      throw Exception("In DistributedPythonNode the ref in NULL ! ");


    DEBTRACE( "---------------End PyfuncSerNode::load function---------------" );
  }
}

void DistributedPythonNode::execute()
{
  YACSTRACE(1,"+++++++++++++++++ DistributedPythonNode::execute: " << getName() << " " << getFname() << " +++++++++++++++++" );
  //////
  PyObject* ob;
  if(!_pyfuncSer)
    throw Exception("DistributedPythonNode badly loaded");
  Engines::pickledArgs *serializationInputCorba(0);
  PyObject *args(0);
  {
    AutoGIL agil;

    DEBTRACE( "---------------DistributedPythonNode::inputs---------------" );
    args = PyTuple_New(getNumberOfInputPorts()) ;
    int pos=0;
    for(list<InputPort *>::iterator iter2 = _setOfInputPort.begin(); iter2 != _setOfInputPort.end(); iter2++,pos++)
      {
        InputPyPort *p=(InputPyPort *)*iter2;
        ob=p->getPyObj();
        Py_INCREF(ob);
        PyTuple_SetItem(args,pos,ob);
      }
    PyObject *serializationInput=PyObject_CallObject(_pyfuncSer,args);
    std::string serializationInputC=PyString_AsString(serializationInput);
    serializationInputCorba=new Engines::pickledArgs;
    int len=serializationInputC.length();
    serializationInputCorba->length(serializationInputC.length());
    for(int i=0;i<serializationInputC.length();i++)
      (*serializationInputCorba)[i]=serializationInputC[i];
    Py_DECREF(serializationInput);
  }
  //serializationInputCorba[serializationInputC.length()]='\0';
  DEBTRACE( "-----------------DistributedPythonNode starting remote python invocation-----------------" );
  Engines::pickledArgs *resultCorba;
  try
  {
      resultCorba=_pynode->execute(getFname().c_str(),*serializationInputCorba);
  }
  catch(...)
  {
      std::string msg="Exception on remote python invocation";
      _errorDetails=msg;
      throw Exception(msg);
  }
  DEBTRACE( "-----------------DistributedPythonNode end of remote python invocation-----------------" );
  //
  delete serializationInputCorba;
  char *resultCorbaC=new char[resultCorba->length()+1];
  resultCorbaC[resultCorba->length()]='\0';
  for(int i=0;i<resultCorba->length();i++)
    resultCorbaC[i]=(*resultCorba)[i];
  delete resultCorba;
  {
    AutoGIL agil;
    args = PyTuple_New(1);
    PyObject* resultPython=PyString_FromString(resultCorbaC);
    delete [] resultCorbaC;
    PyTuple_SetItem(args,0,resultPython);
    PyObject *finalResult=PyObject_CallObject(_pyfuncUnser,args);
    DEBTRACE( "-----------------DistributedPythonNode::outputs-----------------" );
    int nres=1;
    if(finalResult == Py_None)
      nres=0;
    else if(PyTuple_Check(finalResult))
      nres=PyTuple_Size(finalResult);

    if(getNumberOfOutputPorts() != nres)
      {
        std::string msg="Number of output arguments : Mismatch between definition and execution";
        Py_DECREF(finalResult);
        _errorDetails=msg;
        throw Exception(msg);
      }
    try
    {
        int pos(0);
        for(list<OutputPort *>::iterator iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); iter++, pos++)
          {
            OutputPyPort *p=(OutputPyPort *)*iter;
            DEBTRACE( "port name: " << p->getName() );
            DEBTRACE( "port kind: " << p->edGetType()->kind() );
            DEBTRACE( "port pos : " << pos );
            if(PyTuple_Check(finalResult))ob=PyTuple_GetItem(finalResult,pos) ;
            else ob=finalResult;
            DEBTRACE( "ob refcnt: " << ob->ob_refcnt );
            p->put(ob);
          }
    }
    catch(ConversionException& ex)
    {
        Py_DECREF(finalResult);
        _errorDetails=ex.what();
        throw;
    }
  }
  DEBTRACE( "++++++++++++++ End DistributedPythonNode::execute: " << getName() << " ++++++++++++++++++++" );
}

std::string DistributedPythonNode::getEffectiveKindOfServer() const
{
  return "Salome";
}

std::string DistributedPythonNode::getKind() const
{
  //! not a bug : this is to use classical python port translators.
  return PythonNode::KIND;
}

ServerNode *DistributedPythonNode::createNode(const std::string& name) const
{
  ServerNode *ret=new DistributedPythonNode(name);
  ret->setContainer(_container);
  return ret;
}

void DistributedPythonNode::initMySelf()
{
  _implementation = DistributedPythonNode::IMPL_NAME;
  AutoGIL agil;
  _context=PyDict_New();
}

void DistributedPythonNode::dealException(CORBA::Exception *exc, const char *method, const char *ref)
{
  if( exc )
    {
      DEBTRACE( "An exception was thrown!" );
      DEBTRACE( "The raised exception is of Type:" << exc->_name() );

      CORBA::SystemException* sysexc;
      sysexc=CORBA::SystemException::_downcast(exc);
      if(sysexc != NULL)
        {
          // It's a SystemException
          DEBTRACE( "minor code: " << sysexc->minor() );
          DEBTRACE( "completion code: " << sysexc->completed() );
          std::string text="Execution problem: ";
          std::string excname=sysexc->_name();
          if(excname == "BAD_OPERATION")
            {
              text=text+"bad operation detected";
            }
          else if(excname == "MARSHAL" && sysexc->minor() == omni::MARSHAL_PassEndOfMessage)
            {
              text=text+"probably an error in arguments of service '" + method + "' from component '" +ref+ "'";
            }
          else if(excname == "COMM_FAILURE" && sysexc->minor() == omni::COMM_FAILURE_UnMarshalResults)
            {
              text=text+"probably an error in output arguments of service '" + method + "' from component '" +ref+ "'";
            }
          else if(excname == "COMM_FAILURE" && sysexc->minor() == omni::COMM_FAILURE_UnMarshalArguments)
            {
              text=text+"probably an error in input arguments of service '" + method + "' from component '" +ref+ "'";
            }
          else if(excname == "COMM_FAILURE" && sysexc->minor() == omni::COMM_FAILURE_WaitingForReply)
            {
              text=text+"probably an error in input arguments of service '" + method + "' from component '" +ref+ "'";
            }
          else
            {
              DEBTRACE(sysexc->NP_minorString() );
              text=text+"System Exception "+ excname;
            }
          _errorDetails=text;
          throw Exception(text);
        }

      // Not a System Exception
      CORBA::UnknownUserException* userexc;
      userexc=CORBA::UnknownUserException::_downcast(exc);
      if(userexc != NULL)
        {
          CORBA::Any anyExcept = userexc->exception(); 

          const SALOME::SALOME_Exception* salexc;
          if(anyExcept >>= salexc)
            {
              DEBTRACE("SALOME_Exception: "<< salexc->details.sourceFile);
              DEBTRACE("SALOME_Exception: "<<salexc->details.lineNumber);
              _errorDetails=salexc->details.text;
              throw Exception("Execution problem: Salome Exception occurred" + getErrorDetails() );
            }
          std::string msg="Execution problem: User Exception occurred";
          _errorDetails=msg;
          throw Exception(msg);
        }
      std::string msg="Execution problem";
      _errorDetails=msg;
      throw Exception(msg);
    }
}
