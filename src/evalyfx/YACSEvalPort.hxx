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

#ifndef __YACSEVALPORT_HXX__
#define __YACSEVALPORT_HXX__

#include "YACSEvalYFXExport.hxx"

#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class Any;
    class DataPort;
    class InputPort;
    class OutputPort;
    class InputPyPort;
  }
}

class YACSEvalSeqAny;

class YACSEvalAny
{
public:
  YACSEVALYFX_EXPORT virtual ~YACSEvalAny() { }
  YACSEVALYFX_EXPORT virtual std::string getTypeOfData() const = 0;
  YACSEVALYFX_EXPORT virtual int toInt() const = 0;
  YACSEVALYFX_EXPORT virtual double toDouble() const = 0;
  YACSEVALYFX_EXPORT virtual YACSEvalAny *deepCpy() const = 0;
};

class YACSEvalAnyDouble : public YACSEvalAny
{
public:
  YACSEVALYFX_EXPORT YACSEvalAnyDouble(double val):_v(val) { }
  YACSEVALYFX_EXPORT std::string getTypeOfData() const { return std::string(TYPE_REPR); }
  YACSEVALYFX_EXPORT int toInt() const;
  YACSEVALYFX_EXPORT double toDouble() const;
  YACSEVALYFX_EXPORT YACSEvalAnyDouble *deepCpy() const;
  YACSEVALYFX_EXPORT ~YACSEvalAnyDouble() { }
private:
  double _v;
public:
  YACSEVALYFX_EXPORT static const char TYPE_REPR[];
};

class YACSEvalAnyInt : public YACSEvalAny
{
public:
  YACSEVALYFX_EXPORT YACSEvalAnyInt(int val):_v(val) { }
  YACSEVALYFX_EXPORT std::string getTypeOfData() const { return std::string(TYPE_REPR); }
  YACSEVALYFX_EXPORT int toInt() const;
  YACSEVALYFX_EXPORT double toDouble() const;
  YACSEVALYFX_EXPORT YACSEvalAnyInt *deepCpy() const;
  YACSEVALYFX_EXPORT ~YACSEvalAnyInt() { }
private:
  int _v;
public:
  YACSEVALYFX_EXPORT static const char TYPE_REPR[];
};

class YACSEvalPort
{
public:
  YACSEVALYFX_EXPORT virtual std::string getTypeOfData() const = 0;
  YACSEVALYFX_EXPORT virtual ~YACSEvalPort() { }
public:
  YACSEVALYFX_EXPORT static bool IsInputPortPublishable(const YACS::ENGINE::InputPort *port);
  YACSEVALYFX_EXPORT static bool IsOutputPortPublishable(const YACS::ENGINE::OutputPort *port);
protected:
  YACSEVALYFX_EXPORT static std::string GetTypeOfData(const YACS::ENGINE::DataPort *port);
};

class YACSEvalInputPort : public YACSEvalPort
{
public:
  YACSEVALYFX_EXPORT YACSEvalInputPort(YACS::ENGINE::InputPort *ptr);
  YACSEVALYFX_EXPORT std::string getName() const;
  YACSEVALYFX_EXPORT std::string getTypeOfData() const;
  YACSEVALYFX_EXPORT bool isOKForLock() const;
  YACSEVALYFX_EXPORT bool isLocked() const;
  //
  YACSEVALYFX_EXPORT bool hasDefaultValueDefined() const;
  YACSEVALYFX_EXPORT YACSEvalAny *getDefaultValueDefined() const;
  YACSEVALYFX_EXPORT void setDefaultValue(const YACSEvalAny *parameter);
  YACSEVALYFX_EXPORT void setSequenceOfValuesToEval(const YACSEvalSeqAny* vals);
  YACSEVALYFX_EXPORT bool hasSequenceOfValuesToEval() const;
  YACSEVALYFX_EXPORT YACSEvalSeqAny *getSequenceOfValuesToEval() const { return _mySeq; }
  YACSEVALYFX_EXPORT bool isRandomVar() const { return _isRandom; }
  YACSEVALYFX_EXPORT void declareRandomnessStatus(bool isRandom);
  //
  YACSEVALYFX_EXPORT YACS::ENGINE::InputPort *getUndergroundPtr() const { return _ptr; }
  YACSEVALYFX_EXPORT std::size_t initializeUndergroundWithSeq() const;
  //
  YACSEVALYFX_EXPORT virtual ~YACSEvalInputPort();
  void lock() const { _isLocked=true; }
  void unlock() const { _isLocked=false; _isRandom=false; }
  void setUndergroundPortToBeSet(YACS::ENGINE::InputPyPort *p) const;
private:
  YACSEvalAny *convertFromInternalAnyToExternal(YACS::ENGINE::Any *data) const;
  void checkForNonConstMethod() const;
private:
  YACS::ENGINE::InputPort * _ptr;
  YACSEvalSeqAny *_mySeq;
  mutable bool _isRandom;
  mutable bool _isLocked;
  mutable YACS::ENGINE::InputPyPort *_undergroundPort;
};

class YACSEvalOutputPort : public YACSEvalPort
{
public:
  YACSEVALYFX_EXPORT YACSEvalOutputPort(YACS::ENGINE::OutputPort *ptr);
  YACSEVALYFX_EXPORT std::string getName() const;
  YACSEVALYFX_EXPORT std::string getTypeOfData() const;
  YACSEVALYFX_EXPORT bool setQOfInterestStatus(bool newStatus) { _isQOfInt=newStatus; return _isQOfInt; }
  YACSEVALYFX_EXPORT bool isQOfInterest() const { return _isQOfInt; }
  //
  YACSEVALYFX_EXPORT YACS::ENGINE::OutputPort *getUndergroundPtr() const { return _ptr; }
  //
private:
  YACS::ENGINE::OutputPort * _ptr;
  bool _isQOfInt;
};

#endif
