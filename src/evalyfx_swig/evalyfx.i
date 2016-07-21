// Copyright (C) 2015-2016  CEA/DEN, EDF R&D
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

%define EVALYFXDOCSTRING
"Module to evaluate Y=f(X) easily."
%enddef

%module(docstring=EVALYFXDOCSTRING) evalyfx

%feature("autodoc", "1");

%include "engtypemaps.i"

%{
#include "YACSEvalYFX.hxx"
#include "YACSEvalPort.hxx"
#include "YACSEvalSeqAny.hxx"
#include "YACSEvalResource.hxx"
#include "YACSEvalSession.hxx"

static void convertPyToIntArr(PyObject *pyLi, std::vector<int>& arr)
{
  if(PyList_Check(pyLi))
    {
      int size=PyList_Size(pyLi);
      arr.resize(size);
      for(int i=0;i<size;i++)
        {
          PyObject *o=PyList_GetItem(pyLi,i);
          if(PyInt_Check(o))
            {
              int val=(int)PyInt_AS_LONG(o);
              arr[i]=val;
            }
          else
            throw YACS::Exception("list must contain integers only");
        }
    }
  else if(PyTuple_Check(pyLi))
    {
      int size=PyTuple_Size(pyLi);
      arr.resize(size);
      for(int i=0;i<size;i++)
        {
          PyObject *o=PyTuple_GetItem(pyLi,i);
          if(PyInt_Check(o))
            {
              int val=(int)PyInt_AS_LONG(o);
              arr[i]=val;
            }
          else
            throw YACS::Exception("tuple must contain integers only");
        }
    }
  else
    {
      throw YACS::Exception("convertPyToIntArr : not a list nor a tuple");
    }
}

static PyObject *convertToPyToInt(const std::vector<unsigned int>& arr)
{
  std::size_t sz(arr.size());
  PyObject *ret(PyList_New(sz));
  for(std::size_t i=0;i<sz;i++)
    PyList_SetItem(ret,i,PyInt_FromLong(arr[i]));
  return ret;
}

static void convertPyToDblArr(PyObject *pyLi, std::vector<double>& arr)
{
  if(PyList_Check(pyLi))
    {
      int size=PyList_Size(pyLi);
      arr.resize(size);
      for(int i=0;i<size;i++)
        {
          PyObject *o=PyList_GetItem(pyLi,i);
          if(PyFloat_Check(o))
            {
              double val(PyFloat_AS_DOUBLE(o));
              arr[i]=val;
            }
          else
            throw YACS::Exception("list must contain integers only");
        }
    }
  else if(PyTuple_Check(pyLi))
    {
      int size=PyTuple_Size(pyLi);
      arr.resize(size);
      for(int i=0;i<size;i++)
        {
          PyObject *o=PyTuple_GetItem(pyLi,i);
          if(PyFloat_Check(o))
            {
              double val(PyFloat_AS_DOUBLE(o));
              arr[i]=val;
            }
          else
            throw YACS::Exception("tuple must contain floats only");
        }
    }
  else
    {
      throw YACS::Exception("convertPyToNewIntArr3 : not a list nor a tuple");
    }
}

static PyObject *convertVectOfSeqAny(const std::vector<YACSEvalSeqAny *>& retCpp)
{
  std::size_t sz(retCpp.size());
  PyObject *ret(PyList_New(sz));
  for(std::size_t i=0;i<sz;i++)
    {
      YACSEvalSeqAny *elt(retCpp[i]);
      YACSEvalSeqAnyDouble *elt1(dynamic_cast<YACSEvalSeqAnyDouble *>(elt));
      YACSEvalSeqAnyInt *elt2(dynamic_cast<YACSEvalSeqAnyInt *>(elt));
      if(elt1)
        {
          std::vector<double> *zeArr(elt1->getInternal());
          std::size_t sz2(zeArr->size());
          PyObject *ret2(PyList_New(sz2));
          for(std::size_t i2=0;i2<sz2;i2++)
            PyList_SetItem(ret2,i2,PyFloat_FromDouble((*zeArr)[i2]));
          PyList_SetItem(ret,i,ret2);
        }
      else if(elt2)
        {
          std::vector<int> *zeArr(elt2->getInternal());
          std::size_t sz2(zeArr->size());
          PyObject *ret2(PyList_New(sz2));
          for(std::size_t i2=0;i2<sz2;i2++)
            PyList_SetItem(ret2,i2,PyInt_FromLong((*zeArr)[i2]));
          PyList_SetItem(ret,i,ret2);
        }
      else
        throw YACS::Exception("wrap of YACSEvalYFX.getResults : unrecognized type !");
      delete elt;
    }
  return ret;
}
%}

%types(YACSEvalInputPort,YACSEvalOutputPort);
/*%types(YACS::ENGINE::Node *,YACS::ENGINE::Proc *);
%types(YACS::ENGINE::InputPort *,YACS::ENGINE::OutputPort *,YACS::ENGINE::InputDataStreamPort *,YACS::ENGINE::OutputDataStreamPort *);
%types(YACS::ENGINE::InGate *,YACS::ENGINE::OutGate *,YACS::ENGINE::InPort *,YACS::ENGINE::OutPort *,YACS::ENGINE::Port *);
%types(YACS::ENGINE::Container *, YACS::ENGINE::HomogeneousPoolContainer *);*/

%import "loader.i"

%newobject YACSEvalYFX::BuildFromFile;
%newobject YACSEvalYFX::BuildFromScheme;

%typemap(out) std::vector<YACSEvalInputPort *>
{
  std::vector<YACSEvalInputPort *>::const_iterator it;
  $result = PyList_New($1.size());
  int i = 0;
  for (it = $1.begin(); it != $1.end(); ++it, ++i)
    {
      PyList_SetItem($result,i,SWIG_NewPointerObj(SWIG_as_voidptr(*it),SWIGTYPE_p_YACSEvalInputPort, 0 | 0 ));
    }
}

%typemap(out) std::vector<YACSEvalOutputPort *>
{
  std::vector<YACSEvalOutputPort *>::const_iterator it;
  $result = PyList_New($1.size());
  int i = 0;
  for (it = $1.begin(); it != $1.end(); ++it, ++i)
    {
      PyList_SetItem($result,i,SWIG_NewPointerObj(SWIG_as_voidptr(*it),SWIGTYPE_p_YACSEvalOutputPort, 0 | 0 ));
    }
}

%typemap(out) YACSEvalAny *
{
  $result = 0;
  YACSEvalAnyDouble *val0(dynamic_cast<YACSEvalAnyDouble *>($1));
  YACSEvalAnyInt *val1(dynamic_cast<YACSEvalAnyInt *>($1));
  if(val0)
    {
      $result = PyFloat_FromDouble(val0->toDouble());
      delete $1;
    }
  else if(val1)
    {
      $result = PyInt_FromLong(val1->toInt());
      delete $1;
    }
  else
    {
      delete $1;
      throw YACS::Exception("PyWrap of YACSEvalInputPort::getDefaultValueDefined : unrecognized type !");
    }
}

%typemap(in) const std::list< YACSEvalOutputPort * >& 
{
  
}

class YACSEvalPort
{
public:
  virtual std::string getTypeOfData() const;
private:
  YACSEvalPort();
};

class YACSEvalInputPort : public YACSEvalPort
{
public:
  std::string getName() const;
  bool hasDefaultValueDefined() const;
  YACSEvalAny *getDefaultValueDefined() const;
  bool isRandomVar() const;
  void declareRandomnessStatus(bool isRandom);
  bool hasSequenceOfValuesToEval() const;
  %extend
     {
       void setDefaultValue(PyObject *parameter)
       {
         if(parameter==Py_None)
           self->setDefaultValue(0);
         else if(PyFloat_Check(parameter))
           {
             YACSEvalAnyDouble tmp(PyFloat_AsDouble(parameter));
             self->setDefaultValue(&tmp);
           }
         else if(PyInt_Check(parameter))
           {
             YACSEvalAnyInt tmp((int)PyInt_AsLong(parameter));
             self->setDefaultValue(&tmp);
           }
         else
           throw YACS::Exception("PyWrap of YACSEvalInputPort::setParameter : unrecognized type !");
       }
       
       void setSequenceOfValuesToEval(PyObject *vals)
       {
         if(!PyList_Check(vals))
           {
             PyErr_SetString(PyExc_TypeError,"not a list");
             return ;
           }
         int size(PyList_Size(vals));
         YACSEvalSeqAny *valsCpp(0);
         if(size>0)
           {
             PyObject *elt0(PyList_GetItem(vals,0));
             if(PyFloat_Check(elt0))
               {
                 std::vector<double> zeVals;
                 convertPyToDblArr(vals,zeVals);
                 valsCpp=new YACSEvalSeqAnyDouble(zeVals);
               }
             else if(PyInt_Check(elt0))
               {
                 std::vector<int> zeVals;
                 convertPyToIntArr(vals,zeVals);
                 valsCpp=new YACSEvalSeqAnyInt(zeVals);
               }
             else
               throw YACS::Exception("YACSEvalInputPort::setSequenceOfValuesToEval : only list[float] and list[int] actualy supported !");
           }
         else
           valsCpp=YACSEvalSeqAny::BuildEmptyFromType(self->getTypeOfData());
         self->setSequenceOfValuesToEval(valsCpp);
         delete valsCpp;
       }
     }
private:
  YACSEvalInputPort();
};

class YACSEvalOutputPort : public YACSEvalPort
{
public:
  std::string getName() const;
  bool setQOfInterestStatus(bool newStatus);
  bool isQOfInterest() const;
private:
  YACSEvalOutputPort();
};

class YACSEvalVirtualYACSContainer
{
public:
  std::string getChosenMachine() const;
  void setWantedMachine(const std::string& machine);
  std::vector<std::string> listOfPropertyKeys() const;
  std::string getValueOfKey(const char *key) const;
  void setProperty(const std::string& key, const std::string &value);
  std::string getName() const;
private:
  YACSEvalVirtualYACSContainer();
};

class YACSEvalResource
{
public:
  std::vector<std::string> getAllChosenMachines() const;
  std::vector<std::string> getAllFittingMachines() const;
  void setWantedMachine(const std::string& machine);
  std::size_t size() const;
  YACSEvalVirtualYACSContainer *at(std::size_t i) const;
  %extend
     {
       std::size_t __len__() const
       {
         return self->size();
       }
       YACSEvalVirtualYACSContainer *__getitem__(std::size_t i) const
       {
         return self->at(i);
       }
     }
private:
  YACSEvalResource();
};

class YACSEvalParamsForCluster
{
public:
  bool getExclusiveness() const;
  void setExclusiveness(bool newStatus);
  std::string getRemoteWorkingDir();
  void setRemoteWorkingDir(const std::string& remoteWorkingDir);
  std::string getLocalWorkingDir();
  void setLocalWorkingDir(const std::string& localWorkingDir);
  std::string getWCKey() const;
  void setWCKey(const std::string& wcKey);
  unsigned int getNbProcs() const;
  void setNbProcs(unsigned int nbProcs);
  void setMaxDuration(const std::string& maxDuration);
  std::string getMaxDuration() const;
  void checkConsistency() const;
private:
  YACSEvalParamsForCluster();
};

class YACSEvalListOfResources
{
public:
  std::vector<std::string> getAllChosenMachines() const;
  std::vector<std::string> getAllFittingMachines() const;
  void setWantedMachine(const std::string& machine);
  std::size_t size() const;
  bool isInteractive() const;
  bool isMachineInteractive(const std::string& machine) const;
  YACSEvalResource *at(std::size_t i) const;
  unsigned int getNumberOfProcsDeclared() const;
  void checkOKForRun() const;
  YACSEvalParamsForCluster& getAddParamsForCluster();
  %extend
     {
       std::size_t __len__() const
       {
         return self->size();
       }
       YACSEvalResource *__getitem__(std::size_t i) const
       {
         return self->at(i);
       }
     }
private:
  YACSEvalListOfResources();
};

class YACSEvalSession
{
public:
  YACSEvalSession();
  ~YACSEvalSession();
  void launch();
  bool isLaunched() const;
  bool isAttached() const;
  bool isAlreadyPyThreadSaved() const;
  bool getForcedPyThreadSavedStatus() const;
  void setForcedPyThreadSavedStatus(bool status);
  void checkLaunched() const;
  int getPort() const;
  std::string getCorbaConfigFileName() const;
};

class YACSEvalExecParams
{
public:
  bool getStopASAPAfterErrorStatus() const;
  void setStopASAPAfterErrorStatus(bool newStatus);
  bool getFetchRemoteDirForClusterStatus() const;
  void setFetchRemoteDirForClusterStatus(bool newStatus);
private:
  YACSEvalExecParams();
};

class YACSEvalYFX
{
public:
  static YACSEvalYFX *BuildFromFile(const std::string& xmlOfScheme);
  static YACSEvalYFX *BuildFromScheme(YACS::ENGINE::Proc *schema);
  YACSEvalExecParams *getParams() const;
  std::vector<YACSEvalInputPort *> getFreeInputPorts() const;
  std::vector<YACSEvalOutputPort *> getFreeOutputPorts() const;
  void unlockAll();
  bool isLocked() const;
  YACS::ENGINE::Proc *getUndergroundGeneratedGraph() const;
  YACSEvalListOfResources *giveResources();
  std::string getErrorDetailsInCaseOfFailure() const;
  std::string getStatusOfRunStr() const;
  void setParallelizeStatus(bool newVal);
  bool getParallelizeStatus() const;
  //void registerObserver(YACSEvalObserver *observer);
  %extend
     {
       void lockPortsForEvaluation(PyObject *inputsOfInterest, PyObject *outputsOfInterest)
       {
         std::vector<YACSEvalOutputPort *> outputsOfInterestCpp;
         if(PyList_Check(outputsOfInterest))
           {
             int size(PyList_Size(outputsOfInterest));
             for(int i=0;i<size;i++)
               {
                 PyObject *obj(PyList_GetItem(outputsOfInterest,i));
                 void *argp(0);
                 int status(SWIG_ConvertPtr(obj,&argp,SWIGTYPE_p_YACSEvalOutputPort,0|0));
                 if(!SWIG_IsOK(status))
                   {
                     std::ostringstream oss; oss << "Input elt #" << i << " in list is not a YACSEvalOutputPort instance !";
                     throw YACS::Exception(oss.str());
                   }
                 outputsOfInterestCpp.push_back(reinterpret_cast<YACSEvalOutputPort *>(argp));
               }
           }
         else
           {
             PyErr_SetString(PyExc_TypeError,"not a list");
             return ;
           }
         //
         std::vector< YACSEvalInputPort * > inputsOfInterestCpp;
         if(PyList_Check(inputsOfInterest))
           {
             int size(PyList_Size(inputsOfInterest));
             for(int i=0;i<size;i++)
               {
                 PyObject *obj(PyList_GetItem(inputsOfInterest,i));
                 void *argp(0);
                 int status(SWIG_ConvertPtr(obj,&argp,SWIGTYPE_p_YACSEvalInputPort,0|0));
                 if(!SWIG_IsOK(status))
                   {
                     std::ostringstream oss; oss << "Input elt #" << i << " in list is not a YACSEvalInputPort instance !";
                     throw YACS::Exception(oss.str());
                   }
                 inputsOfInterestCpp.push_back(reinterpret_cast<YACSEvalInputPort *>(argp));
               }
           }
         else
           {
             PyErr_SetString(PyExc_TypeError,"not a list");
             return ;
           }
         self->lockPortsForEvaluation(inputsOfInterestCpp,outputsOfInterestCpp);
       }

       PyObject *getResults() const
       {
         std::vector<YACSEvalSeqAny *> retCpp(self->getResults());
         return convertVectOfSeqAny(retCpp);
       }

       PyObject *getResultsInCaseOfFailure() const
       {
         std::vector<unsigned int> ret1Cpp;
         std::vector<YACSEvalSeqAny *> ret0Cpp(self->getResultsInCaseOfFailure(ret1Cpp));
         PyObject *retPy(PyTuple_New(2));
         PyTuple_SetItem(retPy,0,convertVectOfSeqAny(ret0Cpp));
         PyTuple_SetItem(retPy,1,convertToPyToInt(ret1Cpp));
         return retPy;
       }

       PyObject *run(YACSEvalSession *session)
       {
         int ret1;
         bool ret0(self->run(session,ret1));
         PyObject *ret(PyTuple_New(2));
         PyObject *ret0Py(ret0?Py_True:Py_False);
         Py_XINCREF(ret0Py);
         PyTuple_SetItem(ret,0,ret0Py);
         PyTuple_SetItem(ret,1,PyInt_FromLong(ret1));
         return ret;
       }
     }
private:
  YACSEvalYFX();
};
