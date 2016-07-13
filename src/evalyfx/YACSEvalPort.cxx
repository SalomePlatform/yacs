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

#include "YACSEvalPort.hxx"
#include "YACSEvalSeqAny.hxx"
#include "OutputPort.hxx"
#include "InputPort.hxx"
#include "TypeCode.hxx"

#include "CORBAPorts.hxx"
#include "PythonPorts.hxx"
#include "AnyInputPort.hxx"

#include <limits>
#include <sstream>
#include <typeinfo>

const char YACSEvalAnyDouble::TYPE_REPR[]="double";

const char YACSEvalAnyInt::TYPE_REPR[]="int";

int YACSEvalAnyDouble::toInt() const
{
  throw YACS::Exception("YACSEvalAnyDouble::toInt : invalid type requested !");
}

double YACSEvalAnyDouble::toDouble() const
{
  return _v;
}

YACSEvalAnyDouble *YACSEvalAnyDouble::deepCpy() const
{
  return new YACSEvalAnyDouble(*this);
}

int YACSEvalAnyInt::toInt() const
{
  return _v;
}

double YACSEvalAnyInt::toDouble() const
{
  throw YACS::Exception("YACSEvalAnyInt::toDouble : invalid type requested !");
}

YACSEvalAnyInt *YACSEvalAnyInt::deepCpy() const
{
  return new YACSEvalAnyInt(*this);
}

bool YACSEvalPort::IsInputPortPublishable(const YACS::ENGINE::InputPort *port)
{
  YACS::ENGINE::TypeCode *tc(port->edGetType());
  if(!tc)
    throw YACS::Exception("YACSEvalPort::IsInPortPublishable : null type code !");
  if(tc->kind()==YACS::ENGINE::Double || tc->kind()==YACS::ENGINE::Int)
    return true;
  if(tc->kind()==YACS::ENGINE::String)
    {
      if(port->edIsManuallyInitialized())
        return false;
    }
  return true;
}

bool YACSEvalPort::IsOutputPortPublishable(const YACS::ENGINE::OutputPort *port)
{
  YACS::ENGINE::TypeCode *tc(port->edGetType());
  if(!tc)
    throw YACS::Exception("YACSEvalPort::IsOutputPortPublishable : null type code !");
  return tc->kind()==YACS::ENGINE::Double || tc->kind()==YACS::ENGINE::Int;
}

std::string YACSEvalPort::GetTypeOfData(const YACS::ENGINE::DataPort *port)
{
  YACS::ENGINE::TypeCode *tc(port->edGetType());
  if(!tc)
    throw YACS::Exception("YACSEvalPort::GetTypeOfData : null type code !");
  if(tc->kind()==YACS::ENGINE::Double)
    return std::string(YACSEvalAnyDouble::TYPE_REPR);
  if(tc->kind()==YACS::ENGINE::Int)
    return std::string(YACSEvalAnyInt::TYPE_REPR);
  throw YACS::Exception("YACSEvalPort::GetTypeOfData : Unrecognized type of data ! Must be int or double for the moment !");
}

YACSEvalInputPort::YACSEvalInputPort(YACS::ENGINE::InputPort *ptr):_ptr(ptr),_mySeq(0),_isRandom(false),_isLocked(false),_undergroundPort(0)
{
  GetTypeOfData(_ptr);
}

std::string YACSEvalInputPort::getName() const
{
  return _ptr->getName();
}

std::string YACSEvalInputPort::getTypeOfData() const
{
  return GetTypeOfData(_ptr);
}

bool YACSEvalInputPort::isOKForLock() const
{
  if(_isLocked)
    return true;
  return hasDefaultValueDefined();
}

bool YACSEvalInputPort::isLocked() const
{
  return _isLocked;
}

bool YACSEvalInputPort::hasDefaultValueDefined() const
{
  return _ptr->edIsManuallyInitialized();
}

/*!
 * The caller has the ownership of the returned instance.
 */
YACSEvalAny *YACSEvalInputPort::getDefaultValueDefined() const
{
  if(!hasDefaultValueDefined())
    throw YACS::Exception("YACSEvalInputPort::getDefaultValueDefined : no default value defined !");
  _ptr->exRestoreInit();
  YACS::ENGINE::InputPyPort *i0(dynamic_cast<YACS::ENGINE::InputPyPort *>(_ptr));
  if(i0)
    {
      PyObject *data(i0->getPyObj());
      if(!data)
        throw YACS::Exception("YACSEvalInputPort::getDefaultValueDefined : internal error !");
      if(PyFloat_Check(data))
        return new YACSEvalAnyDouble(PyFloat_AsDouble(data));
      if(PyInt_Check(data))
        return new YACSEvalAnyInt((int)PyInt_AsLong(data));
      throw YACS::Exception("YACSEvalInputPort::getDefaultValueDefined : unmanaged types different from int and double for py !");
    }
  YACS::ENGINE::AnyInputPort *i1(dynamic_cast<YACS::ENGINE::AnyInputPort *>(_ptr));
  if(i1)
    {
      YACS::ENGINE::Any *data(i1->getValue());
      return convertFromInternalAnyToExternal(data);
    }
  YACS::ENGINE::InputCorbaPort *i2(dynamic_cast<YACS::ENGINE::InputCorbaPort *>(_ptr));
  if(i2)
    {
      CORBA::Any *data(i2->getAny());
      if(!data)
        throw YACS::Exception("YACSEvalInputPort::getDefaultValueDefined : internal error # 3 !");
      std::string td(getTypeOfData());
      if(td==YACSEvalAnyDouble::TYPE_REPR)
        {
          CORBA::Double v;
          if((*data)>>=v)
            return new YACSEvalAnyDouble(v);
          else
            throw YACS::Exception("YACSEvalInputPort::getDefaultValueDefined : internal error # 31 !");
        }
      if(td==YACSEvalAnyInt::TYPE_REPR)
        {
          CORBA::Long v;
          if((*data)>>=v)
            return new YACSEvalAnyInt(v);
          else
            throw YACS::Exception("YACSEvalInputPort::getDefaultValueDefined : internal error # 32 !");
        }
    }
  std::ostringstream oss;
  oss << "YACSEvalInputPort::getDefaultValueDefined : Please contact anthony.geay@edf.fr with -> \"" << typeid(*_ptr).name() << "\" !";
  throw YACS::Exception(oss.str());
}

/*!
 * This method does not steal the ownership of \a parameter (a deep copy is performed).
 */
void YACSEvalInputPort::setDefaultValue(const YACSEvalAny *parameter)
{
  checkForNonConstMethod();
  if(parameter)
    {
      if(parameter->getTypeOfData()!=getTypeOfData())
        {
          std::ostringstream oss; oss << "YACSEvalInputPort::setParameter : Type of data mismatch ! Expecting \"" << getTypeOfData() << "\" having \"" <<  parameter->getTypeOfData() << "\" !";
          throw YACS::Exception(oss.str());
        }
    }
  //
  YACS::ENGINE::InputPyPort *i0(dynamic_cast<YACS::ENGINE::InputPyPort *>(_ptr));
  YACS::ENGINE::AnyInputPort *i1(dynamic_cast<YACS::ENGINE::AnyInputPort *>(_ptr));
  YACS::ENGINE::InputCorbaPort *i2(dynamic_cast<YACS::ENGINE::InputCorbaPort *>(_ptr));
  if(parameter)
    {
      const YACSEvalAnyDouble *par0(dynamic_cast<const YACSEvalAnyDouble *>(parameter));
      const YACSEvalAnyInt *par1(dynamic_cast<const YACSEvalAnyInt *>(parameter));
      if(i0)
        {
          if(par0)
            {
              PyObject *obj(PyFloat_FromDouble(par0->toDouble()));
              i0->put(obj);
              Py_XDECREF(obj);
            }
          else if(par1)
            {
              PyObject *obj(PyInt_FromLong(par1->toInt()));
              i0->put(obj);
              Py_XDECREF(obj);
            }
          else
            throw YACS::Exception("YACSEvalInputPort::setDefaultValueDefined : unmanaged types different from int and double for py !");
        }
      else if(i1)
        {
          if(par0)
            {
              YACS::ENGINE::AtomAny *v(YACS::ENGINE::AtomAny::New(par0->toDouble()));
              i1->put(v);
              v->decrRef();
            }
          else if(par1)
            {
              YACS::ENGINE::AtomAny *v(YACS::ENGINE::AtomAny::New(par1->toInt()));
              i1->put(v);
              v->decrRef();
            }
          else
            throw YACS::Exception("YACSEvalInputPort::setDefaultValueDefined : unmanaged types different from int and double for yacsport !");
        }
      else if(i2)
        {
          CORBA::Any v;
          if(par0)
            v<<=par0->toDouble();
          else if(par1)
            v<<=(CORBA::Short)par1->toInt();
          else
            throw YACS::Exception("YACSEvalInputPort::setDefaultValueDefined : unmanaged types different from int and double for corbaport !");
          i2->put(&v);
        }
      else
        {
          std::ostringstream oss;
          oss << "YACSEvalInputPort::getDefaultValueDefined : Please contact anthony.geay@edf.fr with -> \"" << typeid(*_ptr).name() << "\" !";
          throw YACS::Exception(oss.str());
        }
      _ptr->exSaveInit();
    }
  else
    _ptr->edRemoveManInit();
}

void YACSEvalInputPort::setSequenceOfValuesToEval(const YACSEvalSeqAny *vals)
{
  if(!_isRandom && _isLocked)
    throw YACS::Exception("YACSEvalInputPort::setSequenceOfValuesToEval : trying to set a sequence on a var not declared as random !");
  //checkForNonConstMethod not called here because it is a special non const method !
  if(!vals)
    throw YACS::Exception("YACSEvalInputPort::setSequenceOfValuesToEval : input is NULL !");
  if(vals==_mySeq)
    return ;
  if(_mySeq)
    delete _mySeq;
  _mySeq=vals->copy();
  _isRandom=true;
}

bool YACSEvalInputPort::hasSequenceOfValuesToEval() const
{
  return (_mySeq!=0);
}

void YACSEvalInputPort::declareRandomnessStatus(bool isRandom)
{
  checkForNonConstMethod();
  _isRandom=isRandom;
  if(!_isRandom)
    _undergroundPort=0;
}

std::size_t YACSEvalInputPort::initializeUndergroundWithSeq() const
{
  if(!_undergroundPort)
    throw YACS::Exception("YACSEvalInputPort::initializeUndergroundWithSeq : internal error ! this input has not been locked properly ! Need to be locked !");
  if(!_mySeq)
    throw YACS::Exception("YACSEvalInputPort::initializeUndergroundWithSeq : this is not set as a sequence !");
  _mySeq->initialize(_undergroundPort);
  return _mySeq->size();
}

YACSEvalInputPort::~YACSEvalInputPort()
{
  delete _mySeq;
}

void YACSEvalInputPort::setUndergroundPortToBeSet(YACS::ENGINE::InputPyPort *p) const
{
  if(!_isRandom)
    throw YACS::Exception("YACSEvalInputPort::setUndergroundPortToBeSet : trying to set a non random var !");
  _undergroundPort=p;
}

YACSEvalAny *YACSEvalInputPort::convertFromInternalAnyToExternal(YACS::ENGINE::Any *data) const
{
  if(!data)
    throw YACS::Exception("YACSEvalInputPort::convertFromInternalAnyToExternal : internal error # 2 !");
  YACS::ENGINE::AtomAny *data2(dynamic_cast<YACS::ENGINE::AtomAny *>(data));
  if(!data2)
    throw YACS::Exception("YACSEvalInputPort::convertFromInternalAnyToExternal : internal error # 21 !");
  std::string td(getTypeOfData());
  if(td==YACSEvalAnyDouble::TYPE_REPR)
    return new YACSEvalAnyDouble(data2->getDoubleValue());
  if(td==YACSEvalAnyInt::TYPE_REPR)
    return new YACSEvalAnyInt(data2->getIntValue());
  throw YACS::Exception("YACSEvalInputPort::convertFromInternalAnyToExternal : unmanaged types different from int and double for any !");
}

void YACSEvalInputPort::checkForNonConstMethod() const
{
  if(_isLocked)
    throw YACS::Exception("YACSEvalInputPort::checkForNonConstMethod : trying to modify a locked input ! To release it call unlockAll on YACSEvalYFX instance owning this !");
}

YACSEvalOutputPort::YACSEvalOutputPort(YACS::ENGINE::OutputPort *ptr):_ptr(ptr),_isQOfInt(false)
{
  GetTypeOfData(_ptr);
}

std::string YACSEvalOutputPort::getName() const
{
  return _ptr->getName();
}

std::string YACSEvalOutputPort::getTypeOfData() const
{
  return GetTypeOfData(_ptr);
}
