// Copyright (C) 2012-2025  CEA, EDF
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

#include "PythonCppUtils.hxx"
#include "Exception.hxx"

#include <Python.h>

#include <sstream>
#include <cstdlib>

const char YACSEvalSession::KERNEL_ROOT_DIR[]="KERNEL_ROOT_DIR";

const char YACSEvalSession::CORBA_CONFIG_ENV_VAR_NAME[]="OMNIORB_CONFIG";

const char YACSEvalSession::NSPORT_VAR_NAME[]="NSPORT";

YACSEvalSession::YACSEvalSession():_isAttached(false),_isLaunched(false),_isForcedPyThreadSaved(false),_port(-1),_salomeInstanceModule(0),_salomeInstance(0),_internal(new YACSEvalSessionInternal)
{
  AutoGIL gal;
  _salomeInstanceModule=PyImport_ImportModule(const_cast<char *>("salome.kernel.salome_instance_impl"));
}

YACSEvalSession::~YACSEvalSession()
{
  delete _internal;
  AutoGIL gal;
  if(isLaunched() && !isAttached())
    {
      AutoPyRef terminateSession(PyObject_GetAttrString(_salomeInstance,const_cast<char *>("stop")));//new
      AutoPyRef res(PyObject_CallObject(terminateSession,0));
    }
  Py_XDECREF(_salomeInstance);
  Py_XDECREF(_salomeInstanceModule);
}

void YACSEvalSession::launch()
{
  if(isLaunched())
    return ;
  AutoGIL gal;
  PyObject *salomeInstance(PyObject_GetAttrString(_salomeInstanceModule,const_cast<char *>("SalomeInstance")));//new
  PyObject *startMeth(PyObject_GetAttrString(salomeInstance,const_cast<char *>("start")));
  Py_XDECREF(salomeInstance);
  AutoPyRef myArgs(PyTuple_New(0));//new
  AutoPyRef myKWArgs(PyDict_New());//new
  PyDict_SetItemString(myKWArgs,"shutdown_servers",Py_True);//Py_True ref not stolen
  _salomeInstance=PyObject_Call(startMeth,myArgs,myKWArgs);//new
  AutoPyRef getPortMeth(PyObject_GetAttrString(_salomeInstance,const_cast<char *>("get_port")));//new
  AutoPyRef portPy(PyObject_CallObject(getPortMeth,0));//new
  _port=PyLong_AsLong(portPy);
  //
  int dummy;
  _corbaConfigFileName=GetConfigAndPort(dummy);
  _isAttached=false; _isLaunched=true;
}

void YACSEvalSession::launchUsingCurrentSession()
{
  if(isLaunched())
    return ;
  AutoGIL gal;
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
  AutoGIL gal;
  AutoPyRef osPy(PyImport_ImportModule(const_cast<char *>("os")));//new
  PyObject *kernelRootDir(0);// os.environ["KERNEL_ROOT_DIR"]
  {
    AutoPyRef environPy(PyObject_GetAttrString(osPy,const_cast<char *>("environ")));//new
    AutoPyRef kernelRootDirStr(PyBytes_FromString(const_cast<char *>(KERNEL_ROOT_DIR)));//new
    kernelRootDir=PyObject_GetItem(environPy,kernelRootDirStr);//new
  }
  {
    AutoPyRef pathPy(PyObject_GetAttrString(osPy,const_cast<char *>("path")));//new
    AutoPyRef joinPy(PyObject_GetAttrString(pathPy,const_cast<char *>("join")));//new
    AutoPyRef myArgs(PyTuple_New(4));
    Py_XINCREF(kernelRootDir); PyTuple_SetItem(myArgs,0,kernelRootDir);
    PyTuple_SetItem(myArgs,1,PyBytes_FromString(const_cast<char *>("bin")));
    PyTuple_SetItem(myArgs,2,PyBytes_FromString(const_cast<char *>("salome")));
    PyTuple_SetItem(myArgs,3,PyBytes_FromString(const_cast<char *>("appliskel")));
    AutoPyRef res(PyObject_CallObject(joinPy,myArgs));
    ret=PyBytes_AsString(res);
  }
  Py_XDECREF(kernelRootDir);
  return ret;
}

std::string YACSEvalSession::GetConfigAndPort(int& port)
{
  std::istringstream iss(std::getenv(NSPORT_VAR_NAME));
  iss >> port;
  return std::getenv(CORBA_CONFIG_ENV_VAR_NAME);
}
