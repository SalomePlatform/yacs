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

#include "YACSEvalSeqAny.hxx"
#include "YACSEvalPort.hxx"
#include "Exception.hxx"

#include "PythonPorts.hxx"

YACSEvalSeqAny *YACSEvalSeqAny::BuildEmptyFromType(const std::string& dataType)
{
  if(dataType==YACSEvalAnyDouble::TYPE_REPR)
    return new YACSEvalSeqAnyDouble;
  else if(dataType==YACSEvalAnyInt::TYPE_REPR)
    return new YACSEvalSeqAnyInt;
  else
    throw YACS::Exception("YACSEvalSeqAny::BuildEmptyFromType : Only int and double are actualy managed !");
}

template<class T>
std::size_t YACSEvalSeqAnyInternal<T>::size() const
{
  if(!_arr)
    throw YACS::Exception("YACSEvalSeqAnyDouble<T>::size : empty array !");
  return _arr->size();
}

YACSEvalSeqAnyDouble::YACSEvalSeqAnyDouble(const std::vector<double>& arr):_arr(0)
{
  std::vector<double> *zeArr(new std::vector<double>(arr));
  _arr=YACSEvalSeqAnyInternal<double>::New(zeArr);
}

YACSEvalSeqAnyDouble::YACSEvalSeqAnyDouble(const YACSEvalSeqAnyDouble& other):_arr(other._arr)
{
  if(_arr)
    _arr->incrRef();
}

YACSEvalSeqAnyDouble::YACSEvalSeqAnyDouble():_arr(YACSEvalSeqAnyInternal<double>::New(new std::vector<double>(0)))
{
}

std::string YACSEvalSeqAnyDouble::getTypeOfConstitutingElements() const
{
  return std::string(YACSEvalAnyDouble::TYPE_REPR);
}

YACSEvalSeqAnyDouble *YACSEvalSeqAnyDouble::copy() const
{
  return new YACSEvalSeqAnyDouble(*this);
}

void YACSEvalSeqAnyDouble::initialize(YACS::ENGINE::InputPyPort *p) const
{
  std::size_t sz(size());
  PyObject *ob(PyList_New(sz));
  for(std::size_t i=0;i<sz;i++)
    {
      PyList_SetItem(ob,i,PyFloat_FromDouble((*_arr)[i]));
    }
  p->put(ob);
  p->exSaveInit();
  Py_XDECREF(ob);
}

std::vector<double> *YACSEvalSeqAnyDouble::getInternal() const
{
  if(!_arr)
    throw YACS::Exception("YACSEvalSeqAnyDouble::getInternal : null internal ref !");
  return _arr->getInternal();
}

YACSEvalSeqAnyInt::YACSEvalSeqAnyInt(const std::vector<int>& arr):_arr(0)
{
  std::vector<int> *zeArr(new std::vector<int>(arr));
  _arr=YACSEvalSeqAnyInternal<int>::New(zeArr);
}

YACSEvalSeqAnyInt::YACSEvalSeqAnyInt(const YACSEvalSeqAnyInt& other):_arr(other._arr)
{
  if(_arr)
    _arr->incrRef();
}

YACSEvalSeqAnyInt::YACSEvalSeqAnyInt():_arr(YACSEvalSeqAnyInternal<int>::New(new std::vector<int>(0)))
{
}

std::string YACSEvalSeqAnyInt::getTypeOfConstitutingElements() const
{
  return std::string(YACSEvalAnyInt::TYPE_REPR);
}

YACSEvalSeqAnyInt *YACSEvalSeqAnyInt::copy() const
{
  return new YACSEvalSeqAnyInt(*this);
}

void YACSEvalSeqAnyInt::initialize(YACS::ENGINE::InputPyPort *p) const
{
  std::size_t sz(size());
  PyObject *ob(PyList_New(sz));
  for(std::size_t i=0;i<sz;i++)
    {
      PyList_SetItem(ob,i,PyInt_FromLong((*_arr)[i]));
    }
  p->put(ob);
  p->exSaveInit();
  Py_XDECREF(ob);
}

std::vector<int> *YACSEvalSeqAnyInt::getInternal() const
{
  if(!_arr)
    throw YACS::Exception("YACSEvalSeqAnyInt::getInternal : null internal ref !");
  return _arr->getInternal();
}
