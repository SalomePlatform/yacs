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

#ifndef __YACSEVALSEQANY_HXX__
#define __YACSEVALSEQANY_HXX__

#include "YACSEvalYFXExport.hxx"
#include "RefCounter.hxx"
#include "Exception.hxx"

#include <vector>
#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class InputPyPort;
  }
}

class YACSEvalSeqAny
{
public:
  YACSEVALYFX_EXPORT static YACSEvalSeqAny *BuildEmptyFromType(const std::string& dataType);
  YACSEVALYFX_EXPORT virtual std::size_t size() const = 0;
  YACSEVALYFX_EXPORT virtual std::string getTypeOfConstitutingElements() const = 0;
  YACSEVALYFX_EXPORT virtual YACSEvalSeqAny *copy() const = 0;
  YACSEVALYFX_EXPORT virtual void initialize(YACS::ENGINE::InputPyPort *p) const = 0;
  YACSEVALYFX_EXPORT virtual ~YACSEvalSeqAny() { }
};

template<class T>
class YACSEvalSeqAnyInternal : public YACS::ENGINE::RefCounter
{
public:
  static YACSEvalSeqAnyInternal<T> *New(std::vector<T> *arr) { return new YACSEvalSeqAnyInternal(arr); }
  std::size_t size() const;
  T operator[](const std::size_t i) const { if(_arr) return (*_arr)[i]; else throw YACS::Exception("YACSEvalSeqAnyInternal[] : internal pointer is null !"); }
  std::vector<T> *getInternal() const { return _arr; }
private:
  ~YACSEvalSeqAnyInternal() { delete _arr; }
  YACSEvalSeqAnyInternal(std::vector<T> *arr):_arr(arr) { }
private:
  std::vector<T> *_arr;
};

class YACSEvalSeqAnyDouble : public YACSEvalSeqAny
{
public:
  YACSEVALYFX_EXPORT std::size_t size() const { return _arr->size(); }
  YACSEVALYFX_EXPORT ~YACSEvalSeqAnyDouble() { if(_arr) _arr->decrRef(); }
  YACSEVALYFX_EXPORT YACSEvalSeqAnyDouble(const std::vector<double>& arr);
  YACSEVALYFX_EXPORT YACSEvalSeqAnyDouble(const YACSEvalSeqAnyDouble& other);
  YACSEVALYFX_EXPORT YACSEvalSeqAnyDouble();
  YACSEVALYFX_EXPORT std::string getTypeOfConstitutingElements() const;
  YACSEVALYFX_EXPORT YACSEvalSeqAnyDouble *copy() const;
  YACSEVALYFX_EXPORT void initialize(YACS::ENGINE::InputPyPort *p) const;
  YACSEVALYFX_EXPORT std::vector<double> *getInternal() const;
private:
  YACSEvalSeqAnyInternal<double> *_arr;
};

class YACSEvalSeqAnyInt : public YACSEvalSeqAny
{
public:
  YACSEVALYFX_EXPORT std::size_t size() const { return _arr->size(); }
  YACSEVALYFX_EXPORT ~YACSEvalSeqAnyInt() { if(_arr) _arr->decrRef(); }
  YACSEVALYFX_EXPORT YACSEvalSeqAnyInt(const std::vector<int>& arr);
  YACSEVALYFX_EXPORT YACSEvalSeqAnyInt(const YACSEvalSeqAnyInt& other);
  YACSEVALYFX_EXPORT YACSEvalSeqAnyInt();
  YACSEVALYFX_EXPORT std::string getTypeOfConstitutingElements() const;
  YACSEVALYFX_EXPORT YACSEvalSeqAnyInt *copy() const;
  YACSEVALYFX_EXPORT void initialize(YACS::ENGINE::InputPyPort *p) const;
  YACSEVALYFX_EXPORT std::vector<int> *getInternal() const;
private:
  YACSEvalSeqAnyInternal<int> *_arr;
};

#endif
