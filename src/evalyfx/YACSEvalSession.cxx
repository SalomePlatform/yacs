// Copyright (C) 2012-2015  CEA/DEN, EDF R&D
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
#include "Exception.hxx"

#include <Python.h>

const char YACSEvalSession::KERNEL_ROOT_DIR[]="KERNEL_ROOT_DIR";

const char YACSEvalSession::CORBA_CONFIG_ENV_VAR_NAME[]="OMNIORB_CONFIG";

YACSEvalSession::YACSEvalSession():_isLaunched(false),_port(-1),_salomeInstanceModule(0),_salomeInstance(0)
{
  if(!Py_IsInitialized())
    Py_Initialize();
  //
  _salomeInstanceModule=PyImport_ImportModule(const_cast<char *>("salome_instance"));
}

YACSEvalSession::~YACSEvalSession()
{
  if(isLaunched())
    {
      PyObject *terminateSession(PyObject_GetAttrString(_salomeInstance,const_cast<char *>("stop")));//new
      PyObject *res(PyObject_CallObject(terminateSession,0));
      Py_XDECREF(res);
      Py_XDECREF(terminateSession);
    }
  Py_XDECREF(_salomeInstance);
  Py_XDECREF(_salomeInstanceModule);
}

void YACSEvalSession::launch()
{
  if(isLaunched())
    return ;
  PyObject *salomeInstance(PyObject_GetAttrString(_salomeInstanceModule,const_cast<char *>("SalomeInstance")));//new
  PyObject *startMeth(PyObject_GetAttrString(salomeInstance,const_cast<char *>("start")));
  Py_XDECREF(salomeInstance);
  PyObject *myArgs(PyTuple_New(0));//new
  PyObject *myKWArgs(PyDict_New());//new
  PyDict_SetItemString(myKWArgs,"shutdown_servers",Py_True);//Py_True ref not stolen
  _salomeInstance=PyObject_Call(startMeth,myArgs,myKWArgs);//new
  PyObject *getPortMeth(PyObject_GetAttrString(_salomeInstance,const_cast<char *>("get_port")));//new
  PyObject *portPy(PyObject_CallObject(getPortMeth,0));//new
  _port=PyInt_AsLong(portPy);
  Py_XDECREF(portPy);
  Py_XDECREF(getPortMeth);
  Py_XDECREF(myKWArgs);
  Py_XDECREF(myArgs);
  //
  PyObject *osPy(PyImport_ImportModule(const_cast<char *>("os")));//new
  PyObject *environPy(PyObject_GetAttrString(osPy,const_cast<char *>("environ")));//new
  PyObject *corbaConfigStr(PyString_FromString(const_cast<char *>(CORBA_CONFIG_ENV_VAR_NAME)));//new
  PyObject *corbaConfigFileNamePy(PyObject_GetItem(environPy,corbaConfigStr));//new
  _corbaConfigFileName=PyString_AsString(corbaConfigFileNamePy);
  Py_XDECREF(corbaConfigFileNamePy);
  Py_XDECREF(corbaConfigStr);
  Py_XDECREF(environPy);
  Py_XDECREF(osPy);
  _isLaunched=true;
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
  PyObject *osPy(PyImport_ImportModule(const_cast<char *>("os")));//new
  PyObject *kernelRootDir(0);// os.environ["KERNEL_ROOT_DIR"]
  {
    PyObject *environPy(PyObject_GetAttrString(osPy,const_cast<char *>("environ")));//new
    PyObject *kernelRootDirStr(PyString_FromString(const_cast<char *>(KERNEL_ROOT_DIR)));//new
    kernelRootDir=PyObject_GetItem(environPy,kernelRootDirStr);//new
    Py_XDECREF(kernelRootDirStr);
    Py_XDECREF(environPy);
  }
  {
    PyObject *pathPy(PyObject_GetAttrString(osPy,const_cast<char *>("path")));//new
    PyObject *joinPy(PyObject_GetAttrString(pathPy,const_cast<char *>("join")));//new
    PyObject *myArgs(PyTuple_New(4));
    Py_XINCREF(kernelRootDir); PyTuple_SetItem(myArgs,0,kernelRootDir);
    PyTuple_SetItem(myArgs,1,PyString_FromString(const_cast<char *>("bin")));
    PyTuple_SetItem(myArgs,2,PyString_FromString(const_cast<char *>("salome")));
    PyTuple_SetItem(myArgs,3,PyString_FromString(const_cast<char *>("appliskel")));
    PyObject *res(PyObject_CallObject(joinPy,myArgs));
    ret=PyString_AsString(res);
    Py_XDECREF(res);
    Py_XDECREF(myArgs);
    Py_XDECREF(joinPy);
    Py_XDECREF(pathPy);
  }
  Py_XDECREF(kernelRootDir);
  Py_XDECREF(osPy);
  return ret;
}