// Copyright (C) 2012-2016  CEA/DEN, EDF R&D
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
// Author : Anthony Geay (EDF R&D)

#include "YACSEvalSession.hxx"
#include "YACSEvalSessionInternal.hxx"

#include "AutoGIL.hxx"
#include "Exception.hxx"

#include <Python.h>

#include <sstream>

const char YACSEvalSession::KERNEL_ROOT_DIR[]="KERNEL_ROOT_DIR";

const char YACSEvalSession::CORBA_CONFIG_ENV_VAR_NAME[]="OMNIORB_CONFIG";

const char YACSEvalSession::NSPORT_VAR_NAME[]="NSPORT";

YACSEvalSession::YACSEvalSession():_isAttached(false),_isLaunched(false),_isForcedPyThreadSaved(false),_port(-1),_salomeInstanceModule(0),_salomeInstance(0),_internal(new YACSEvalSessionInternal)
{
  if(!Py_IsInitialized())
    Py_Initialize();
  //
  {
    YACS::ENGINE::AutoGIL gal;
    _salomeInstanceModule=PyImport_ImportModule(const_cast<char *>("salome_instance"));
  }
}

YACSEvalSession::~YACSEvalSession()
{
  delete _internal;
  YACS::ENGINE::AutoGIL gal;
  if(isLaunched() && !isAttached())
    {
      YACS::ENGINE::AutoPyRef terminateSession(PyObject_GetAttrString(_salomeInstance,const_cast<char *>("stop")));//new
      YACS::ENGINE::AutoPyRef res(PyObject_CallObject(terminateSession,0));
    }
  Py_XDECREF(_salomeInstance);
  Py_XDECREF(_salomeInstanceModule);
}

void YACSEvalSession::launch()
{
  if(isLaunched())
    return ;
  YACS::ENGINE::AutoGIL gal;
  PyObject *salomeInstance(PyObject_GetAttrString(_salomeInstanceModule,const_cast<char *>("SalomeInstance")));//new
  PyObject *startMeth(PyObject_GetAttrString(salomeInstance,const_cast<char *>("start")));
  Py_XDECREF(salomeInstance);
  YACS::ENGINE::AutoPyRef myArgs(PyTuple_New(0));//new
  YACS::ENGINE::AutoPyRef myKWArgs(PyDict_New());//new
  PyDict_SetItemString(myKWArgs,"shutdown_servers",Py_True);//Py_True ref not stolen
  _salomeInstance=PyObject_Call(startMeth,myArgs,myKWArgs);//new
  YACS::ENGINE::AutoPyRef getPortMeth(PyObject_GetAttrString(_salomeInstance,const_cast<char *>("get_port")));//new
  YACS::ENGINE::AutoPyRef portPy(PyObject_CallObject(getPortMeth,0));//new
  _port=PyInt_AsLong(portPy);
  //
  int dummy;
  _corbaConfigFileName=GetConfigAndPort(dummy);
  _isAttached=false; _isLaunched=true;
}

void YACSEvalSession::launchUsingCurrentSession()
{
  if(isLaunched())
    return ;
  YACS::ENGINE::AutoGIL gal;
  _corbaConfigFileName=GetConfigAndPort(_port);
  _isAttached=true; _isLaunched=true;
}

bool YACSEvalSession::isAlreadyPyThreadSaved() const
{
  return _isForcedPyThreadSaved;
}

void YACSEvalSession::checkLaunched() const
{
  if(!isLaunched())
    throw YACS::Exception("YACSEvalSession::checkLaunched : not launched !");
}

int YACSEvalSession::getPort() const
{
  checkLaunched();
  return _port;
}

std::string YACSEvalSession::getCorbaConfigFileName() const
{
  checkLaunched();
  return _corbaConfigFileName;
}

std::string YACSEvalSession::GetPathToAdd()
{
  std::string ret;
  YACS::ENGINE::AutoGIL gal;
  YACS::ENGINE::AutoPyRef osPy(PyImport_ImportModule(const_cast<char *>("os")));//new
  PyObject *kernelRootDir(0);// os.environ["KERNEL_ROOT_DIR"]
  {
    YACS::ENGINE::AutoPyRef environPy(PyObject_GetAttrString(osPy,const_cast<char *>("environ")));//new
    YACS::ENGINE::AutoPyRef kernelRootDirStr(PyString_FromString(const_cast<char *>(KERNEL_ROOT_DIR)));//new
    kernelRootDir=PyObject_GetItem(environPy,kernelRootDirStr);//new
  }
  {
    YACS::ENGINE::AutoPyRef pathPy(PyObject_GetAttrString(osPy,const_cast<char *>("path")));//new
    YACS::ENGINE::AutoPyRef joinPy(PyObject_GetAttrString(pathPy,const_cast<char *>("join")));//new
    YACS::ENGINE::AutoPyRef myArgs(PyTuple_New(4));
    Py_XINCREF(kernelRootDir); PyTuple_SetItem(myArgs,0,kernelRootDir);
    PyTuple_SetItem(myArgs,1,PyString_FromString(const_cast<char *>("bin")));
    PyTuple_SetItem(myArgs,2,PyString_FromString(const_cast<char *>("salome")));
    PyTuple_SetItem(myArgs,3,PyString_FromString(const_cast<char *>("appliskel")));
    YACS::ENGINE::AutoPyRef res(PyObject_CallObject(joinPy,myArgs));
    ret=PyString_AsString(res);
  }
  Py_XDECREF(kernelRootDir);
  return ret;
}

std::string YACSEvalSession::GetConfigAndPort(int& port)
{
  YACS::ENGINE::AutoPyRef osPy(PyImport_ImportModule(const_cast<char *>("os")));//new
  YACS::ENGINE::AutoPyRef environPy(PyObject_GetAttrString(osPy,const_cast<char *>("environ")));//new
  //
  YACS::ENGINE::AutoPyRef corbaConfigStr(PyString_FromString(const_cast<char *>(CORBA_CONFIG_ENV_VAR_NAME)));//new
  YACS::ENGINE::AutoPyRef corbaConfigFileNamePy(PyObject_GetItem(environPy,corbaConfigStr));//new
  std::string ret(PyString_AsString(corbaConfigFileNamePy));
  //
  YACS::ENGINE::AutoPyRef nsPortStr(PyString_FromString(const_cast<char *>(NSPORT_VAR_NAME)));//new
  YACS::ENGINE::AutoPyRef nsPortValuePy(PyObject_GetItem(environPy,nsPortStr));//new
  std::string portStr(PyString_AsString(nsPortValuePy));
  std::istringstream iss(portStr);
  iss >> port;
  return ret;
}
