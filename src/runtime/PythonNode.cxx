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
#include "PythonNode.hxx"
#include "PythonPorts.hxx"
#include "TypeCode.hxx"
#include "AutoGIL.hxx"
#include "Container.hxx"
#include "SalomeContainer.hxx"
#include "SalomeHPContainer.hxx"
#include "SalomeContainerTmpForHP.hxx"
#include "ConversionException.hxx"

#include "PyStdout.hxx"
#include <iostream>
#include <sstream>
#include <fstream>

#ifdef WIN32
#include <process.h>
#define getpid _getpid
#endif

#if PY_VERSION_HEX < 0x02050000 
typedef int Py_ssize_t;
#endif

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char PythonEntry::SCRIPT_FOR_SIMPLE_SERIALIZATION[]="import cPickle\n"
    "def pickleForVarSimplePyth2009(val):\n"
    "  return cPickle.dumps(val,-1)\n"
    "\n";

const char PythonNode::IMPL_NAME[]="Python";
const char PythonNode::KIND[]="Python";

const char PythonNode::SCRIPT_FOR_SERIALIZATION[]="import cPickle\n"
    "def pickleForDistPyth2009(kws):\n"
    "  return cPickle.dumps(((),kws),-1)\n"
    "\n"
    "def unPickleForDistPyth2009(st):\n"
    "  args=cPickle.loads(st)\n"
    "  return args\n";

const char PythonNode::REMOTE_NAME[]="remote";

const char PythonNode::DPL_INFO_NAME[]="my_dpl_localization";

const char PyFuncNode::SCRIPT_FOR_SERIALIZATION[]="import cPickle\n"
    "def pickleForDistPyth2009(*args,**kws):\n"
    "  return cPickle.dumps((args,kws),-1)\n"
    "\n"
    "def unPickleForDistPyth2009(st):\n"
    "  args=cPickle.loads(st)\n"
    "  return args\n";


PythonEntry::PythonEntry():_context(0),_pyfuncSer(0),_pyfuncUnser(0),_pyfuncSimpleSer(0)
{
}

PythonEntry::~PythonEntry()
{
  AutoGIL agil;
  DEBTRACE( "_context refcnt: " << _context->ob_refcnt );
  // not Py_XDECREF of _pyfuncUnser because it is returned by PyDict_GetItem -> borrowed
  // not Py_XDECREF of _pyfuncSer because it is returned by PyDict_GetItem -> borrowed
  Py_XDECREF(_context);
}

void PythonEntry::commonRemoteLoadPart1(InlineNode *reqNode)
{
  DEBTRACE( "---------------PythonEntry::CommonRemoteLoad function---------------" );
  Container *container(reqNode->getContainer());
  bool isContAlreadyStarted(false);
  if(container)
    {
      isContAlreadyStarted=container->isAlreadyStarted(reqNode);
      if(!isContAlreadyStarted)
        {
          try
          {
              container->start(reqNode);
          }
          catch(Exception& e)
          {
              reqNode->setErrorDetails(e.what());
              throw e;
          }
        }
    }
  else
    {
      std::string what("PythonEntry::CommonRemoteLoad : a load operation requested on \"");
      what+=reqNode->getName(); what+="\" with no container specified.";
      reqNode->setErrorDetails(what);
      throw Exception(what);
    }
}

Engines::Container_var PythonEntry::commonRemoteLoadPart2(InlineNode *reqNode, bool& isInitializeRequested)
{
  Container *container(reqNode->getContainer());
  Engines::Container_var objContainer=Engines::Container::_nil();
  if(!container)
    throw Exception("No container specified !");
  SalomeContainer *containerCast0(dynamic_cast<SalomeContainer *>(container));
  SalomeHPContainer *containerCast1(dynamic_cast<SalomeHPContainer *>(container));
  if(containerCast0)
    objContainer=containerCast0->getContainerPtr(reqNode);
  else if(containerCast1)
    {
      YACS::BASES::AutoCppPtr<SalomeContainerTmpForHP> tmpCont(SalomeContainerTmpForHP::BuildFrom(containerCast1,reqNode));
      objContainer=tmpCont->getContainerPtr(reqNode);
    }
  else
    throw Exception("Unrecognized type of container ! Salome one is expected for PythonNode/PyFuncNode !");
  if(CORBA::is_nil(objContainer))
    throw Exception("Container corba pointer is NULL for PythonNode !");
  isInitializeRequested=false;
  try
  {
      if(containerCast0)
        {
          createRemoteAdaptedPyInterpretor(objContainer);
        }
      else
        {
          Engines::PyNodeBase_var dftPyScript(retrieveDftRemotePyInterpretorIfAny(objContainer));
          if(CORBA::is_nil(dftPyScript))
            {
              isInitializeRequested=true;
              createRemoteAdaptedPyInterpretor(objContainer);
            }
          else
            assignRemotePyInterpretor(dftPyScript);
        }
  }
  catch( const SALOME::SALOME_Exception& ex )
  {
      std::string msg="Exception on remote python node creation ";
      msg += '\n';
      msg += ex.details.text.in();
      reqNode->setErrorDetails(msg);
      throw Exception(msg);
  }
  Engines::PyNodeBase_var pynode(getRemoteInterpreterHandle());
  if(CORBA::is_nil(pynode))
    throw Exception("In PythonNode the ref in NULL ! ");
  return objContainer;
}

void PythonEntry::commonRemoteLoadPart3(InlineNode *reqNode, Engines::Container_ptr objContainer, bool isInitializeRequested)
{
  Container *container(reqNode->getContainer());
  Engines::PyNodeBase_var pynode(getRemoteInterpreterHandle());
  ///
  {
    AutoGIL agil;
    const char *picklizeScript(getSerializationScript());
    PyObject *res=PyRun_String(picklizeScript,Py_file_input,_context,_context);
    PyObject *res2(PyRun_String(SCRIPT_FOR_SIMPLE_SERIALIZATION,Py_file_input,_context,_context));
    if(res == NULL || res2==NULL)
      {
        std::string errorDetails;
        PyObject* new_stderr = newPyStdOut(errorDetails);
        reqNode->setErrorDetails(errorDetails);
        PySys_SetObject((char*)"stderr", new_stderr);
        PyErr_Print();
        PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
        Py_DECREF(new_stderr);
        throw Exception("Error during load");
      }
    Py_DECREF(res); Py_DECREF(res2);
    _pyfuncSer=PyDict_GetItemString(_context,"pickleForDistPyth2009");
    _pyfuncUnser=PyDict_GetItemString(_context,"unPickleForDistPyth2009");
    _pyfuncSimpleSer=PyDict_GetItemString(_context,"pickleForVarSimplePyth2009");
    if(_pyfuncSer == NULL)
      {
        std::string errorDetails;
        PyObject *new_stderr(newPyStdOut(errorDetails));
        reqNode->setErrorDetails(errorDetails);
        PySys_SetObject((char*)"stderr", new_stderr);
        PyErr_Print();
        PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
        Py_DECREF(new_stderr);
        throw Exception("Error during load");
      }
    if(_pyfuncUnser == NULL)
      {
        std::string errorDetails;
        PyObject *new_stderr(newPyStdOut(errorDetails));
        reqNode->setErrorDetails(errorDetails);
        PySys_SetObject((char*)"stderr", new_stderr);
        PyErr_Print();
        PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
        Py_DECREF(new_stderr);
        throw Exception("Error during load");
      }
    if(_pyfuncSimpleSer == NULL)
      {
        std::string errorDetails;
        PyObject *new_stderr(newPyStdOut(errorDetails));
        reqNode->setErrorDetails(errorDetails);
        PySys_SetObject((char*)"stderr", new_stderr);
        PyErr_Print();
        PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
        Py_DECREF(new_stderr);
        throw Exception("Error during load");
      }
  }
  if(isInitializeRequested)
    {//This one is called only once at initialization in the container if an init-script is specified.
      try
      {
          std::string zeInitScriptKey(container->getProperty(HomogeneousPoolContainer::INITIALIZE_SCRIPT_KEY));
          if(!zeInitScriptKey.empty())
            pynode->executeAnotherPieceOfCode(zeInitScriptKey.c_str());
      }
      catch( const SALOME::SALOME_Exception& ex )
      {
          std::string msg="Exception on PythonNode::loadRemote python invocation of initializisation py script !";
          msg += '\n';
          msg += ex.details.text.in();
          reqNode->setErrorDetails(msg);
          throw Exception(msg);
      }
      DEBTRACE( "---------------End PyNode::loadRemote function---------------" );
    }
}

std::string PythonEntry::GetContainerLog(const std::string& mode, Container *container, const Task *askingTask)
{
  if(mode=="local")
    return "";

  std::string msg;
  try
  {
      SalomeContainer *containerCast(dynamic_cast<SalomeContainer *>(container));
      SalomeHPContainer *objContainer2(dynamic_cast<SalomeHPContainer *>(container));
      if(containerCast)
        {
          Engines::Container_var objContainer(containerCast->getContainerPtr(askingTask));
          CORBA::String_var logname = objContainer->logfilename();
          DEBTRACE(logname);
          msg=logname;
          std::string::size_type pos = msg.find(":");
          msg=msg.substr(pos+1);
        }
      else if(objContainer2)
        {
          msg="Remote PythonNode is on HP Container : no log because no info of the location by definition of HP Container !";
        }
      else
        {
          msg="Not implemented yet for container log for that type of container !";
        }
  }
  catch(...)
  {
      msg = "Container no longer reachable";
  }
  return msg;
}

void PythonEntry::commonRemoteLoad(InlineNode *reqNode)
{
  commonRemoteLoadPart1(reqNode);
  bool isInitializeRequested;
  Engines::Container_var objContainer(commonRemoteLoadPart2(reqNode,isInitializeRequested));
  commonRemoteLoadPart3(reqNode,objContainer,isInitializeRequested);
}

PythonNode::PythonNode(const PythonNode& other, ComposedNode *father):InlineNode(other,father)
{
  _implementation=IMPL_NAME;
  {
    AutoGIL agil;
    _context=PyDict_New();
    if( PyDict_SetItemString( _context, "__builtins__", getSALOMERuntime()->getBuiltins() ))
      {
        stringstream msg;
        msg << "Impossible to set builtins" << __FILE__ << ":" << __LINE__;
        _errorDetails=msg.str();
        throw Exception(msg.str());
      }
  }
}

PythonNode::PythonNode(const std::string& name):InlineNode(name)
{
  _implementation=IMPL_NAME;
  {
    AutoGIL agil;
    _context=PyDict_New();
    if( PyDict_SetItemString( _context, "__builtins__", getSALOMERuntime()->getBuiltins() ))
      {
        stringstream msg;
        msg << "Impossible to set builtins" << __FILE__ << ":" << __LINE__;
        _errorDetails=msg.str();
        throw Exception(msg.str());
      }
  }
}

PythonNode::~PythonNode()
{
  if(!CORBA::is_nil(_pynode))
    {
      _pynode->UnRegister();
    }
}

void PythonNode::checkBasicConsistency() const throw(YACS::Exception)
{
  DEBTRACE("checkBasicConsistency");
  InlineNode::checkBasicConsistency();
  {
    AutoGIL agil;
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
        throw Exception(error);
      }
    else
      Py_XDECREF(res);
  }
}

void PythonNode::load()
{
  DEBTRACE( "---------------PyNode::load function---------------" );
  if(_mode==PythonNode::REMOTE_NAME)
    loadRemote();
  else
    loadLocal();
}

void PythonNode::loadLocal()
{
  DEBTRACE( "---------------PyNode::loadLocal function---------------" );
  // do nothing
}

void PythonNode::loadRemote()
{
  commonRemoteLoad(this);
}

void PythonNode::execute()
{
  if(_mode==PythonNode::REMOTE_NAME)
    executeRemote();
  else
    executeLocal();
}

void PythonNode::executeRemote()
{
  DEBTRACE( "++++++++++++++ PyNode::executeRemote: " << getName() << " ++++++++++++++++++++" );
  if(!_pyfuncSer)
    throw Exception("DistributedPythonNode badly loaded");
  //
  if(dynamic_cast<HomogeneousPoolContainer *>(getContainer()))
    {
      bool dummy;
      commonRemoteLoadPart2(this,dummy);
      _pynode->assignNewCompiledCode(getScript().c_str());
    }
  //
  Engines::pickledArgs_var serializationInputCorba(new Engines::pickledArgs);
  {
      AutoGIL agil;
      PyObject *args(0),*ob(0);
      //===========================================================================
      // Get inputs in input ports, build a Python dict and pickle it
      //===========================================================================
      args = PyDict_New();
      std::list<InputPort *>::iterator iter2;
      int pos(0);
      for(iter2 = _setOfInputPort.begin(); iter2 != _setOfInputPort.end(); ++iter2)
        {
          InputPyPort *p=(InputPyPort *)*iter2;
          ob=p->getPyObj();
          PyDict_SetItemString(args,p->getName().c_str(),ob);
          pos++;
        }
#ifdef _DEVDEBUG_
      PyObject_Print(args,stderr,Py_PRINT_RAW);
      std::cerr << endl;
#endif
      PyObject *serializationInput(PyObject_CallFunctionObjArgs(_pyfuncSer,args,NULL));
      Py_DECREF(args);
      //The pickled string may contain NULL characters so use PyString_AsStringAndSize
      char *serializationInputC(0);
      Py_ssize_t len;
      if (PyString_AsStringAndSize(serializationInput, &serializationInputC, &len))
        throw Exception("DistributedPythonNode problem in python pickle");
      serializationInputCorba->length(len);
      for(int i=0; i < len ; i++)
        serializationInputCorba[i]=serializationInputC[i];
      Py_DECREF(serializationInput);
  }

  //get the list of output argument names
  std::list<OutputPort *>::iterator iter;
  Engines::listofstring myseq;
  myseq.length(getNumberOfOutputPorts());
  int pos=0;
  for(iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); ++iter)
    {
      OutputPyPort *p=(OutputPyPort *)*iter;
      myseq[pos]=p->getName().c_str();
      DEBTRACE( "port name: " << p->getName() );
      DEBTRACE( "port kind: " << p->edGetType()->kind() );
      DEBTRACE( "port pos : " << pos );
      pos++;
    }
  //===========================================================================
  // Execute in remote Python node
  //===========================================================================
  DEBTRACE( "-----------------starting remote python invocation-----------------" );
  Engines::pickledArgs_var resultCorba;
  try
    {
      //pass outargsname and dict serialized
      resultCorba=_pynode->execute(myseq,serializationInputCorba);
    }
  catch( const SALOME::SALOME_Exception& ex )
    {
      std::string msg="Exception on remote python invocation";
      msg += '\n';
      msg += ex.details.text.in();
      _errorDetails=msg;
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

  {
      AutoGIL agil;
      PyObject *args(0),*ob(0);
      PyObject* resultPython=PyString_FromStringAndSize(resultCorbaC,resultCorba->length());
      delete [] resultCorbaC;
      args = PyTuple_New(1);
      PyTuple_SetItem(args,0,resultPython);
      PyObject *finalResult=PyObject_CallObject(_pyfuncUnser,args);
      Py_DECREF(args);

      if (finalResult == NULL)
        {
          std::stringstream msg;
          msg << "Conversion with pickle of output ports failed !";
          msg << " : " << __FILE__ << ":" << __LINE__;
          _errorDetails=msg.str();
          throw YACS::ENGINE::ConversionException(msg.str());
        }

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
          _errorDetails=msg;
          throw Exception(msg);
        }

      pos=0;
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
          _errorDetails=ex.what();
          throw;
      }
  }
  DEBTRACE( "++++++++++++++ ENDOF PyNode::executeRemote: " << getName() << " ++++++++++++++++++++" );
}

void PythonNode::executeLocal()
{
  DEBTRACE( "++++++++++++++ PyNode::executeLocal: " << getName() << " ++++++++++++++++++++" );
  {
    AutoGIL agil;

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
        throw Exception("Error during execution");
      }
    PyObject *res = PyEval_EvalCode((PyCodeObject *)code, _context, _context);

    Py_DECREF(code);
    Py_XDECREF(res);
    DEBTRACE( "_context refcnt: " << _context->ob_refcnt );
    fflush(stdout);
    fflush(stderr);
    if(PyErr_Occurred ())
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
        throw Exception("Error during execution");
      }

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
            if(ob==NULL)
              {
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
        _errorDetails=ex.what();
        throw;
    }

    DEBTRACE( "-----------------End PyNode::outputs-----------------" );
  }
  DEBTRACE( "++++++++++++++ End PyNode::execute: " << getName() << " ++++++++++++++++++++" );
}

std::string PythonNode::getContainerLog()
{
  return PythonEntry::GetContainerLog(_mode,_container,this);
}

void PythonNode::shutdown(int level)
{
  DEBTRACE("PythonNode::shutdown " << level);
  if(_mode=="local")return;
  if(_container)
    {
      if(!CORBA::is_nil(_pynode)) _pynode->UnRegister();
      _pynode=Engines::PyScriptNode::_nil();
      _container->shutdown(level);
    }
}

Node *PythonNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new PythonNode(*this,father);
}

void PythonNode::createRemoteAdaptedPyInterpretor(Engines::Container_ptr objContainer)
{
  if(!CORBA::is_nil(_pynode))
    _pynode->UnRegister();
  _pynode=objContainer->createPyScriptNode(getName().c_str(),getScript().c_str());
}

Engines::PyNodeBase_var PythonNode::retrieveDftRemotePyInterpretorIfAny(Engines::Container_ptr objContainer) const
{
  Engines::PyScriptNode_var ret(objContainer->getDefaultPyScriptNode(getName().c_str()));
  if(!CORBA::is_nil(ret))
    {
      ret->Register();
    }
  return Engines::PyNodeBase::_narrow(ret);
}

void PythonNode::assignRemotePyInterpretor(Engines::PyNodeBase_var remoteInterp)
{
  if(!CORBA::is_nil(_pynode))
    {
      Engines::PyScriptNode_var tmpp(Engines::PyScriptNode::_narrow(remoteInterp));
      if(_pynode->_is_equivalent(tmpp))
        return ;
    }
  if(!CORBA::is_nil(_pynode))
    _pynode->UnRegister();
  _pynode=Engines::PyScriptNode::_narrow(remoteInterp);
}

Engines::PyNodeBase_var PythonNode::getRemoteInterpreterHandle()
{
  return Engines::PyNodeBase::_narrow(_pynode);
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

void PythonNode::applyDPLScope(ComposedNode *gfn)
{
  std::vector< std::pair<std::string,int> > ret(getDPLScopeInfo(gfn));
  if(ret.empty())
    return ;
  //
  PyObject *ob(0);
  {
    AutoGIL agil;
    std::size_t sz(ret.size());
    ob=PyList_New(sz);
    for(std::size_t i=0;i<sz;i++)
      {
        const std::pair<std::string,int>& p(ret[i]);
        PyObject *elt(PyTuple_New(2));
        PyTuple_SetItem(elt,0,PyString_FromString(p.first.c_str()));
        PyTuple_SetItem(elt,1,PyLong_FromLong(p.second));
        PyList_SetItem(ob,i,elt);
      }
  }
  if(_mode==REMOTE_NAME)
    {
      Engines::pickledArgs_var serializationInputCorba(new Engines::pickledArgs);
      {
        AutoGIL agil;
        PyObject *serializationInput(PyObject_CallFunctionObjArgs(_pyfuncSimpleSer,ob,NULL));
        Py_XDECREF(ob);
        char *serializationInputC(0);
        Py_ssize_t len;
        if (PyString_AsStringAndSize(serializationInput, &serializationInputC, &len))
          throw Exception("DistributedPythonNode problem in python pickle");
        serializationInputCorba->length(len);
        for(int i=0; i < len ; i++)
          serializationInputCorba[i]=serializationInputC[i];
        Py_XDECREF(serializationInput);
      }
      _pynode->defineNewCustomVar(DPL_INFO_NAME,serializationInputCorba);
    }
  else
    {
      AutoGIL agil;
      PyDict_SetItemString(_context,DPL_INFO_NAME,ob);
      Py_XDECREF(ob);
    }
}

PyFuncNode::PyFuncNode(const PyFuncNode& other, ComposedNode *father):InlineFuncNode(other,father),_pyfunc(0)
{
  _implementation = PythonNode::IMPL_NAME;
  {
    AutoGIL agil;
    _context=PyDict_New();
    DEBTRACE( "_context refcnt: " << _context->ob_refcnt );
    if( PyDict_SetItemString( _context, "__builtins__", getSALOMERuntime()->getBuiltins() ))
      {
        stringstream msg;
        msg << "Not possible to set builtins" << __FILE__ << ":" << __LINE__;
        _errorDetails=msg.str();
        throw Exception(msg.str());
      }
  }
}

PyFuncNode::PyFuncNode(const std::string& name): InlineFuncNode(name),_pyfunc(0)
{

  _implementation = PythonNode::IMPL_NAME;
  DEBTRACE( "PyFuncNode::PyFuncNode " << name );
  {
    AutoGIL agil;
    _context=PyDict_New();
    DEBTRACE( "_context refcnt: " << _context->ob_refcnt );
    if( PyDict_SetItemString( _context, "__builtins__", getSALOMERuntime()->getBuiltins() ))
      {
        stringstream msg;
        msg << "Not possible to set builtins" << __FILE__ << ":" << __LINE__;
        _errorDetails=msg.str();
        throw Exception(msg.str());
      }
  }
}

PyFuncNode::~PyFuncNode()
{
  if(!CORBA::is_nil(_pynode))
    {
      _pynode->UnRegister();
    }
}

void PyFuncNode::init(bool start)
{
  initCommonPartWithoutStateManagement(start);
  if(_state == YACS::DISABLED)
    {
      exDisabledState(); // to refresh propagation of DISABLED state
      return ;
    }
  if(start) //complete initialization
    setState(YACS::READY);
  else if(_state > YACS::LOADED)// WARNING FuncNode has internal vars (CEA usecase) ! Partial initialization (inside a loop). Exclusivity of funcNode.
    setState(YACS::TORECONNECT);
}

void PyFuncNode::checkBasicConsistency() const throw(YACS::Exception)
{
  DEBTRACE("checkBasicConsistency");
  InlineFuncNode::checkBasicConsistency();
  {
    AutoGIL agil;
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
        throw Exception(error);
      }
    else
      Py_XDECREF(res);
  }
}

void PyFuncNode::load()
{
  DEBTRACE( "---------------PyfuncNode::load function---------------" );
  if(_mode==PythonNode::REMOTE_NAME)
    loadRemote();
  else
    loadLocal();
}

void PyFuncNode::loadRemote()
{
  commonRemoteLoad(this);
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

  {
    AutoGIL agil;
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
        throw Exception("Error during execution");
      }
    PyObject *res = PyEval_EvalCode((PyCodeObject *)code, _context, _context);
    Py_DECREF(code);
    Py_XDECREF(res);

    DEBTRACE( "_context refcnt: " << _context->ob_refcnt );
    if(PyErr_Occurred ())
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
        throw Exception("Error during execution");
        return;
      }
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
        throw Exception("Error during execution");
      }
    DEBTRACE( "---------------End PyFuncNode::load function---------------" );
  }
}

void PyFuncNode::execute()
{
  if(_mode==PythonNode::REMOTE_NAME)
    executeRemote();
  else
    executeLocal();
}

void PyFuncNode::executeRemote()
{
  DEBTRACE( "++++++++++++++ PyFuncNode::executeRemote: " << getName() << " ++++++++++++++++++++" );
  if(!_pyfuncSer)
    throw Exception("DistributedPythonNode badly loaded");
  //
  if(dynamic_cast<HomogeneousPoolContainer *>(getContainer()))
    {
      bool dummy;
      commonRemoteLoadPart2(this,dummy);
      _pynode->executeAnotherPieceOfCode(getScript().c_str());
    }
  //
  Engines::pickledArgs_var serializationInputCorba(new Engines::pickledArgs);;
  {
      AutoGIL agil;
      PyObject *ob(0);
      //===========================================================================
      // Get inputs in input ports, build a Python tuple and pickle it
      //===========================================================================
      PyObject *args(PyTuple_New(getNumberOfInputPorts()));
      int pos(0);
      for(std::list<InputPort *>::iterator iter2 = _setOfInputPort.begin(); iter2 != _setOfInputPort.end(); iter2++,pos++)
        {
          InputPyPort *p=(InputPyPort *)*iter2;
          ob=p->getPyObj();
          Py_INCREF(ob);
          PyTuple_SetItem(args,pos,ob);
        }
#ifdef _DEVDEBUG_
      PyObject_Print(args,stderr,Py_PRINT_RAW);
      std::cerr << endl;
#endif
      PyObject *serializationInput=PyObject_CallObject(_pyfuncSer,args);
      Py_DECREF(args);
      //The pickled string may contain NULL characters so use PyString_AsStringAndSize
      char *serializationInputC(0);
      Py_ssize_t len;
      if (PyString_AsStringAndSize(serializationInput, &serializationInputC, &len))
        throw Exception("DistributedPythonNode problem in python pickle");

      serializationInputCorba->length(len);
      for(int i=0; i < len ; i++)
        serializationInputCorba[i]=serializationInputC[i];
      Py_DECREF(serializationInput);
  }

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

  {
      AutoGIL agil;

      PyObject *resultPython(PyString_FromStringAndSize(resultCorbaC,resultCorba->length()));
      delete [] resultCorbaC;
      PyObject *args(PyTuple_New(1)),*ob(0);
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
          _errorDetails=msg;
          throw Exception(msg);
        }

      try
      {
          int pos(0);
          for(std::list<OutputPort *>::iterator iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); iter++, pos++)
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
            }
          Py_DECREF(finalResult);
      }
      catch(ConversionException& ex)
      {
          Py_DECREF(finalResult);
          _errorDetails=ex.what();
          throw;
      }
  }

  DEBTRACE( "++++++++++++++ ENDOF PyFuncNode::executeRemote: " << getName() << " ++++++++++++++++++++" );
}

void PyFuncNode::executeLocal()
{
  DEBTRACE( "++++++++++++++ PyFuncNode::execute: " << getName() << " ++++++++++++++++++++" );

  int pos=0;
  PyObject* ob;
  if(!_pyfunc)throw Exception("PyFuncNode badly loaded");
  {
      AutoGIL agil;
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
          _errorDetails=ex.what();
          throw;
      }
      DEBTRACE( "-----------------End PyFuncNode::outputs-----------------" );
      Py_DECREF(result);
  }
  DEBTRACE( "++++++++++++++ End PyFuncNode::execute: " << getName() << " ++++++++++++++++++++" );
}

Node *PyFuncNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new PyFuncNode(*this,father);
}

void PyFuncNode::createRemoteAdaptedPyInterpretor(Engines::Container_ptr objContainer)
{
  if(!CORBA::is_nil(_pynode))
    _pynode->UnRegister();
  _pynode=objContainer->createPyNode(getName().c_str(),getScript().c_str());
}

Engines::PyNodeBase_var PyFuncNode::retrieveDftRemotePyInterpretorIfAny(Engines::Container_ptr objContainer) const
{
  Engines::PyNode_var ret(objContainer->getDefaultPyNode(getName().c_str()));
  if(!CORBA::is_nil(ret))
    {
      ret->Register();
    }
  return Engines::PyNodeBase::_narrow(ret);
}

void PyFuncNode::assignRemotePyInterpretor(Engines::PyNodeBase_var remoteInterp)
{
  if(!CORBA::is_nil(_pynode))
    {
      Engines::PyNode_var tmpp(Engines::PyNode::_narrow(remoteInterp));
      if(_pynode->_is_equivalent(tmpp))
        return ;
    }
  if(!CORBA::is_nil(_pynode))
    _pynode->UnRegister();
  _pynode=Engines::PyNode::_narrow(remoteInterp);
}

Engines::PyNodeBase_var PyFuncNode::getRemoteInterpreterHandle()
{
  return Engines::PyNodeBase::_narrow(_pynode);
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
  return PythonEntry::GetContainerLog(_mode,_container,this);
}

void PyFuncNode::shutdown(int level)
{
  DEBTRACE("PyFuncNode::shutdown " << level);
  if(_mode=="local")return;
  if(_container)
    {
      if(!CORBA::is_nil(_pynode)) _pynode->UnRegister();
      _pynode=Engines::PyNode::_nil();
      _container->shutdown(level);
    }
}

