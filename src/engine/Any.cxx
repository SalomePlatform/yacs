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

#include "Any.hxx"
#include "Runtime.hxx"
#include "TypeCode.hxx"
#include "InvalidExtractionException.hxx"

#include <cstring>
#include <cstdlib>

using namespace YACS::ENGINE;
using namespace std;

// forbidden value int=-269488145 double=-1.54947e+231 bool=239
const char SeqAlloc::DFT_CHAR_VAR=-17;//0xEF

StringOnHeap::StringOnHeap(const char *val):_dealloc(0),_str(strdup(val))
{
}

StringOnHeap::StringOnHeap(const std::string& val):_dealloc(0),_str(strdup(val.c_str()))
{
}

/*! 
 * \note : no copy is performed if a deallocator is given.
 * \param val     : String in C format that is NOT copied if
 *                  deAlloc != 0
 * \param deAlloc : pointer on function to deallocate val after
 *                  last use.
 */
StringOnHeap::StringOnHeap(char *val, Deallocator deAlloc):_dealloc(deAlloc)
{
  if(deAlloc)
    _str=val;
  else
    _str=strdup(val);
}

bool StringOnHeap::operator ==(const StringOnHeap& other) const
{
  return strcmp(_str, other._str)==0;
}

StringOnHeap *StringOnHeap::deepCopy() const
{
  return new StringOnHeap(_str);
}

StringOnHeap::~StringOnHeap()
{
  if(_dealloc)
    _dealloc(_str);
  else
    free(_str);
}

Any::Any(TypeCode* type):_type(type)
{
  _type->incrRef();
}

Any::Any(const Any& other):_type(other._type)
{
  _type->incrRef();
}

Any::~Any()
{
  _type->decrRef();
}

bool Any::IsNull(char *data)
{
  if(!data)
    return true;
  bool isNull(true);
  for(std::size_t i=0;i<sizeof(void *) && isNull;i++)
    isNull=(data[i]==SeqAlloc::DFT_CHAR_VAR);
  return isNull;
}

AtomAny::AtomAny(int val):Any(Runtime::_tc_int)
{
  _value._i=val;
}

AtomAny::AtomAny(bool val):Any(Runtime::_tc_bool)
{
  _value._b=val;
}

AtomAny::AtomAny(double val):Any(Runtime::_tc_double)
{
  _value._d=val;
}

AtomAny::AtomAny(const char *val):Any(Runtime::_tc_string)
{
  _value._s=new StringOnHeap(val);
}

AtomAny::AtomAny(const std::string& val):Any(Runtime::_tc_string)
{
  _value._s=new StringOnHeap(val);
}

AtomAny::AtomAny(const AtomAny& other):Any(other)
{
  if(_type->isA(Runtime::_tc_string))
    {
      StringOnHeap *cpy=(other._value._s)->deepCopy();
      memcpy(&_value._s,&cpy,_type->getSizeInByteOfAnyReprInSeq());
    }
  else if(_type->isA(Runtime::_tc_double))
    memcpy(&_value._d,&other._value._d,_type->getSizeInByteOfAnyReprInSeq());
  else if(_type->isA(Runtime::_tc_int))
    memcpy(&_value._i,&other._value._i,_type->getSizeInByteOfAnyReprInSeq());
  else if(_type->isA(Runtime::_tc_bool))
    memcpy(&_value._b,&other._value._b,_type->getSizeInByteOfAnyReprInSeq());
}

AtomAny::AtomAny(char *val, Deallocator deAlloc):Any(Runtime::_tc_string)
{
  _value._s=new StringOnHeap(val,deAlloc);
}

AtomAny::AtomAny(char *data, TypeCode* type):Any(type)
{
  if(type->isA(Runtime::_tc_string))
    {
      void **tmp=(void **)data;
      StringOnHeap *cpy=((StringOnHeap *)(*tmp))->deepCopy();
      memcpy(&_value._s,&cpy,type->getSizeInByteOfAnyReprInSeq());
    }
  else if(type->isA(Runtime::_tc_double))
    memcpy(&_value._d,data,type->getSizeInByteOfAnyReprInSeq());
  else if(type->isA(Runtime::_tc_int))
    memcpy(&_value._i,data,type->getSizeInByteOfAnyReprInSeq());
  else if(type->isA(Runtime::_tc_bool))
    memcpy(&_value._b,data,type->getSizeInByteOfAnyReprInSeq());
}

Any *AtomAny::clone() const
{
  return new AtomAny(*this);
}

AtomAny *AtomAny::New(char *val,Deallocator dealloc)
{
  return new AtomAny(val,dealloc);
}

AnyPtr AtomAny::operator[](int i) const throw(YACS::Exception)
{
  throw InvalidExtractionException(_type->kind(),Sequence);
}

AnyPtr AtomAny::operator[](const char *key) const throw(YACS::Exception)
{
  throw Exception("AtomAny::operator[] : try to get a part of a partitionned data whereas atomical.");
}

bool AtomAny::operator ==(const Any& other) const
{
  if(!_type->isA(other.getType()))
    return false;
  const AtomAny& otherC=(const AtomAny&) other;//cast granted due to previous lines
  if(_type->isA(Runtime::_tc_double))
    return _value._d==otherC._value._d;
  else if(_type->isA(Runtime::_tc_int))
    return _value._i==otherC._value._i;
  else if(_type->isA(Runtime::_tc_bool))
    return _value._b==otherC._value._b;
  else if(_type->isA(Runtime::_tc_string))
    return (*_value._s)==*(otherC._value._s);
  else
    return false;
}

int AtomAny::getIntValue() const throw(YACS::Exception)
{
  if(_type->isA(Runtime::_tc_int))
    return _value._i;
  else
    throw Exception("Value is not an int");
}

bool AtomAny::getBoolValue() const throw(YACS::Exception)
{
  if(_type->isA(Runtime::_tc_bool))
    return _value._b;
  else
    throw Exception("Value is not a bool");
}

double AtomAny::getDoubleValue() const throw(YACS::Exception)
{
  if(_type->isA(Runtime::_tc_double))
    return _value._d;
  else
    throw Exception("Value is not a double");
}

std::string AtomAny::getStringValue() const throw(YACS::Exception)
{
  if(_type->isA(Runtime::_tc_string))
    return string(_value._s->cStr());
  else
    throw Exception("Value is not a string");
}

/*!
 * \note : This method put in data its zipped recursive content in data.
 *         The ownership of the recursive content is tranfered to data.
 *         So this owns nothing and its counter fall by 1.
 *         For memory space minimal use, not all of '*this' is pushed at data location. 
 * \param data : already allocated memory zone where to put compressed content of 'this'
 */
void AtomAny::putMyReprAtPlace(char *data) const
{
  if(_type->isA(Runtime::_tc_string))
    {
      StringOnHeap *tmp=_value._s->deepCopy();
      memcpy(data,&tmp,_type->getSizeInByteOfAnyReprInSeq());
    }
  else if(_type->isA(Runtime::_tc_double))
    memcpy(data,&_value._d,_type->getSizeInByteOfAnyReprInSeq());
  else if(_type->isA(Runtime::_tc_int))
    memcpy(data,&_value._i,_type->getSizeInByteOfAnyReprInSeq());
  else if(_type->isA(Runtime::_tc_bool))
    memcpy(data,&_value._b,_type->getSizeInByteOfAnyReprInSeq());
}

/*!
 * \note : This method put in data its zipped recursive content in data.
 *         The ownership of the recursive content is tranfered to data.
 *         So this owns nothing and its counter fall by 1.
 *         For memory space minimal use, not all of '*this' is pushed at data location.
 *         'deepCpy' param is not used here because by definition of AtomAny deep copy is performed.
 * \param data : already allocated memory zone where to put compressed content of 'this'
 * \param src :
 * \param type :
 * \param deepCpy :
 */
void AtomAny::putReprAtPlace(char *data, const char *src, const TypeCode *type, bool deepCpy)
{
  if(type->isA(Runtime::_tc_string))
    {
      void **tmp1=(void **)src;
      StringOnHeap *tmp=((const StringOnHeap *)(*tmp1))->deepCopy();
      memcpy(data,&tmp,type->getSizeInByteOfAnyReprInSeq());
    }
  else if(type->isA(Runtime::_tc_double))
    memcpy(data,src,type->getSizeInByteOfAnyReprInSeq());
  else if(type->isA(Runtime::_tc_int))
    memcpy(data,src,type->getSizeInByteOfAnyReprInSeq());
  else if(type->isA(Runtime::_tc_bool))
    memcpy(data,src,type->getSizeInByteOfAnyReprInSeq());
}

/*!
 * \note : Opposite method of putMyReprAtPlace. But static because due to data compression
 *         instance is lost.
 */
void AtomAny::destroyReprAtPlace(char *data, const TypeCode *type)
{
  DynType typ=type->kind();
  if(typ==String)
    {
      if(!Any::IsNull(data))
        {
          void **tmp=(void **)data;
          delete ((StringOnHeap *)(*tmp));
        }
    }
}

AnyPtr AtomAny::getOrBuildFromData(char *data, const TypeCode *type)
{
  Any *ret;
  ret=new AtomAny(data,(TypeCode *)type);
  return AnyPtr(ret);
}

bool AtomAny::takeInChargeStorageOf(TypeCode *type)
{
  DynType typ=type->kind();
  return (typ==Double || typ==Int || typ==Bool || typ==String);
}

AtomAny::~AtomAny()
{
  if(_type->kind() == String)
    delete _value._s;
}

ComposedAny::ComposedAny(const ComposedAny& other):Any(other)
{
}

ComposedAny::ComposedAny(TypeCode* type, bool isNew):Any(type)
{
  if(isNew)
    _type->decrRef();
}

AnyPtr ComposedAny::operator[](const char *key) const throw(YACS::Exception)
{
  throw Exception("AtomAny::operator[] : try to get a part of a partitionned data not localizable by a string.");
}

void ComposedAny::checkTypeOf(const Any *elem) const throw(YACS::Exception)
{
  if(!elem->getType()->isA(_type->contentType()))
    throw Exception("ComposedAny::checkTypeOf : invalid type.");
}

int ComposedAny::getIntValue() const throw(YACS::Exception)
{
 throw InvalidExtractionException(_type->kind(),Runtime::_tc_int->kind());
}

bool ComposedAny::getBoolValue() const throw(YACS::Exception)
{
  throw InvalidExtractionException(_type->kind(),Runtime::_tc_bool->kind());
}

double ComposedAny::getDoubleValue() const throw(YACS::Exception)
{
  throw InvalidExtractionException(_type->kind(),Runtime::_tc_double->kind());
}

std::string ComposedAny::getStringValue() const throw(YACS::Exception)
{
  throw InvalidExtractionException(_type->kind(),Runtime::_tc_string->kind());
}

SeqAlloc::SeqAlloc(const SeqAlloc& other):_sizeOf1Elm(other._sizeOf1Elm),_notStdDeAlloc(0),
 _start(0),_finish(0),_endOfStorage(0)
{
  _start=allocate(other._finish-other._start);
  _finish=_start+(other._finish-other._start);
  _endOfStorage=_finish;
}

SeqAlloc::SeqAlloc(unsigned int sizeOf1Elm):_sizeOf1Elm(sizeOf1Elm),_notStdDeAlloc(0),
                                            _start(0),_finish(0),_endOfStorage(0)
{
}

SeqAlloc::~SeqAlloc()
{
  deallocate(_start);
}

void SeqAlloc::clear()
{
  deallocate(_start);
  _start=0;
  _finish=0;
  _endOfStorage=0;
}

/*!
 * \note : This method is exclusively reserved for arrays of C++ built-in types because no
 *         constructor is applied atomically.
 */
void SeqAlloc::initCoarseMemory(char *mem, unsigned int size, Deallocator dealloc)
{
  unsigned sizeInByte=size*_sizeOf1Elm;
  if(dealloc)
    {
      _notStdDeAlloc=dealloc;
      _start=mem;
    }
  else
    {
      _start=allocate(sizeInByte);
      if(mem)
        memcpy(_start,mem,sizeInByte);
      else
        {
          for(unsigned int i=0;i<sizeInByte;i++) _start[i]=DFT_CHAR_VAR;// see getSetItems
        }
    }
  _finish=_start+sizeInByte;
  _endOfStorage=_finish;
}

void SeqAlloc::construct(char *pt, const Any *val)
{
  val->putMyReprAtPlace(pt);
}

/*!
 * \note: This performs the placement new or zip info into pt.
 * \param pt :
 * \param val     : the source from which the construction will be performed.
 * \param tc  :
 * \param deepCpy : If true in pt place a deep copy pointed by val will be put.
 */
void SeqAlloc::construct(char *pt, const char *val, const TypeCode *tc, bool deepCpy)
{
  tc->putReprAtPlace(pt,val,deepCpy);
}

char *SeqAlloc::allocate(unsigned int nbOfByte)
{ 
  if(nbOfByte>0)
    return (char *)::operator new(nbOfByte);
  else
    return 0;
}

// pt is not permitted to be a null pointer.
void SeqAlloc::deallocate(char *pt)
{ 
  if(pt)
    {
      if(!_notStdDeAlloc)
        ::operator delete(pt); 
      else
        {
          _notStdDeAlloc(pt);
          _notStdDeAlloc=0;
        }
    }
}

void SeqAlloc::destroy(char *pt, const TypeCode *tc) 
{ 
  tc->destroyZippedAny(pt);
}

unsigned int SeqAlloc::size() const
{
  return (_finish-_start)/_sizeOf1Elm;
}

std::vector<unsigned int> SeqAlloc::getSetItems() const
{
  std::vector<unsigned int> ret;
  unsigned int sz(size());
  for(unsigned int i=0;i<sz;i++)
    {
      const char *pt(_start+i*_sizeOf1Elm);
      for(unsigned j=0;j<_sizeOf1Elm && *pt==DFT_CHAR_VAR;j++,pt++); //see initCoarseMemory
      if(pt!=_start+(i+1)*_sizeOf1Elm)
        ret.push_back(i);
    }
  return ret;
}

void SequenceAny::clear()
{
  for (char *cur=_alloc._start;cur!=_alloc._finish;cur+=_alloc._sizeOf1Elm)
    _alloc.destroy(cur,_type->contentType());
  _alloc.clear();
}

void SequenceAny::popBack()
{
  _alloc._finish-=_alloc._sizeOf1Elm;
  _alloc.destroy(_alloc._finish,_type->contentType());
}

void SequenceAny::pushBack(const Any* elem)
{
  if(!elem->_type->isA(_type->contentType()))
    throw InvalidExtractionException(elem->_type->kind(),_type->contentType()->kind());
  if(_alloc._finish != _alloc._endOfStorage)
    {
      _alloc.construct(_alloc._finish, elem);
      _alloc._finish+=_alloc._sizeOf1Elm;
    }
  else
    realloc(_alloc._finish, elem);
}

bool SequenceAny::operator ==(const Any& other) const
{
  if(!_type->isA(other.getType()))
    return false;
  const SequenceAny& otherC=(const SequenceAny&) other;//cast granted due to previous lines
  if(size()!=otherC.size())
    return false;
  for(unsigned i=0;i<size();i++)
    if(!((*(*this)[i])==(*otherC[i])))
      return false;
  return true;
}

void SequenceAny::setEltAtRank(int i, const Any *elem) throw(YACS::Exception)
{
  checkTypeOf(elem);
  _alloc.destroy(_alloc._start+i*_alloc._sizeOf1Elm,_type->contentType());
  _alloc.construct(_alloc._start+i*_alloc._sizeOf1Elm,elem);
}

AnyPtr SequenceAny::operator[](int i) const throw(YACS::Exception)
{
  return _type->contentType()->getOrBuildAnyFromZippedData(_alloc._start+i*_alloc._sizeOf1Elm);
}

/*!
 * \note : Contrary to AtomAny 'this' (ref) is put in data NOT a deep copy.
 * \param data : already allocated memory zone where to put address of 'this'
 */
void SequenceAny::putMyReprAtPlace(char *data) const
{
  const void *tmp=(const void *)this;
  memcpy(data,&tmp,_type->getSizeInByteOfAnyReprInSeq());
  const void **tmp2=(const void **) data;
  ((SequenceAny *)(*tmp2))->incrRef();
  //::new((SequenceAny *)data) SequenceAny((SequenceAny&) (*this));
}

void SequenceAny::putReprAtPlace(char *data, const char *src, const TypeCode *type, bool deepCpy)
{
  void **tmp2=(void **) src;
  if(!deepCpy)
    {
      ((SequenceAny *)(*tmp2))->incrRef();
      memcpy(data,src,type->getSizeInByteOfAnyReprInSeq());
    }
  else
    {
      SequenceAny *cpy=new SequenceAny(*((SequenceAny *)(*tmp2)));
      memcpy(data,&cpy,type->getSizeInByteOfAnyReprInSeq());
    }
  //::new((SequenceAny *)data) SequenceAny((SequenceAny&) (*this));
}

void SequenceAny::destroyReprAtPlace(char *data, const TypeCode *type)
{
  void **tmp=(void **) data;
  if(!Any::IsNull(data))
    ((SequenceAny *)(*tmp))->decrRef();
  //((SequenceAny *)data)->~SequenceAny();
}

AnyPtr SequenceAny::getOrBuildFromData(char *data, const TypeCode *type)
{
  void **tmp=(void **) data;
  ((SequenceAny *) (*tmp))->incrRef();
  return AnyPtr((SequenceAny *)(*tmp));
}

Any *SequenceAny::clone() const
{
  return new SequenceAny(*this);
}

SequenceAny *SequenceAny::removeUnsetItemsFromThis() const
{
  std::vector<unsigned int> its(getSetItems());
  std::size_t sz(its.size());
  SequenceAny *ret(SequenceAny::New(getType()->contentType(),sz));
  for(std::size_t i=0;i<sz;i++)
    {
      AnyPtr obj((*this)[its[i]]);
      ret->setEltAtRank(i,obj);
    }
  return ret;
}

SequenceAny *SequenceAny::New(const TypeCode *typeOfContent)
{
  if(typeOfContent->kind() == Objref)
    {
      //In case of Objref, use a sequence of string
      return new SequenceAny(Runtime::_tc_string);
    }
  else
    return new SequenceAny(typeOfContent);
}

SequenceAny *SequenceAny::New(const TypeCode *typeOfContent, unsigned lgth)
{
  if(typeOfContent->kind() == Objref)
    {
      //In case of Objref, use a sequence of string
      return new SequenceAny(Runtime::_tc_string,lgth);
    }
  else
    return new SequenceAny(typeOfContent,lgth);
}

bool SequenceAny::takeInChargeStorageOf(TypeCode *type)
{
  DynType typ=type->kind();
  return (typ==Sequence);
}

SequenceAny::SequenceAny(const SequenceAny& other):ComposedAny(other),_alloc(other._alloc)
{
  const char *srcCur=other._alloc._start;
  for(char *cur=_alloc._start;srcCur != other._alloc._finish; srcCur+=_alloc._sizeOf1Elm, cur+=_alloc._sizeOf1Elm)
    _alloc.construct(cur, srcCur, _type->contentType(),true);
}

SequenceAny::~SequenceAny()
{
  for (char *cur=_alloc._start;cur!=_alloc._finish;cur+=_alloc._sizeOf1Elm)
    _alloc.destroy(cur,_type->contentType());
}

/*!
 * \param typeOfContent : typeCode of the type of elements stored in sequence.
 */
SequenceAny::SequenceAny(const TypeCode *typeOfContent):ComposedAny(new TypeCodeSeq("","",typeOfContent)),
                                                        _alloc(typeOfContent->getSizeInByteOfAnyReprInSeq())
{
}

SequenceAny::SequenceAny(const TypeCode *typeOfContent, unsigned lgth):ComposedAny(new TypeCodeSeq("","",typeOfContent)),
                                                                       _alloc(typeOfContent->getSizeInByteOfAnyReprInSeq())
{
  _alloc.initCoarseMemory(0,lgth,0);
}

SequenceAny::SequenceAny(int *val, unsigned int lgth, Deallocator deAlloc):ComposedAny(new TypeCodeSeq("","",Runtime::_tc_int)),
                                                                           _alloc(Runtime::_tc_int->getSizeInByteOfAnyReprInSeq())
{
  _alloc.initCoarseMemory((char *)val,lgth,deAlloc);
}

SequenceAny::SequenceAny(bool *val, unsigned int lgth, Deallocator deAlloc):ComposedAny(new TypeCodeSeq("","",Runtime::_tc_bool)),
                                                                            _alloc(Runtime::_tc_bool->getSizeInByteOfAnyReprInSeq())
{
  _alloc.initCoarseMemory((char *)val,lgth,deAlloc);
}

SequenceAny::SequenceAny(double *val, unsigned int lgth, Deallocator deAlloc):ComposedAny(new TypeCodeSeq("","",Runtime::_tc_double)),
                                                                              _alloc(Runtime::_tc_double->getSizeInByteOfAnyReprInSeq())
{
  _alloc.initCoarseMemory((char *)val,lgth,deAlloc);
}

SequenceAny::SequenceAny(const std::vector<int>& val):ComposedAny(new TypeCodeSeq("","",Runtime::_tc_int)),
                                                      _alloc(Runtime::_tc_int->getSizeInByteOfAnyReprInSeq())
{
  _alloc.initCoarseMemory((char *)&val[0],val.size(),0);
}

SequenceAny::SequenceAny(const std::vector<bool>& val):ComposedAny(new TypeCodeSeq("","",Runtime::_tc_bool)),
                                                       _alloc(Runtime::_tc_bool->getSizeInByteOfAnyReprInSeq())
{
  for(vector<bool>::const_iterator iter=val.begin();iter!=val.end();iter++)
    {
      AtomAnyPtr tmp=AtomAny::New(*iter);
      pushBack(tmp);
    }
}

SequenceAny::SequenceAny(const std::vector<double>& val):ComposedAny(new TypeCodeSeq("","",Runtime::_tc_double)),
                                                         _alloc(Runtime::_tc_double->getSizeInByteOfAnyReprInSeq())
{
  _alloc.initCoarseMemory((char *)&val[0],val.size(),0);
}

SequenceAny::SequenceAny(const std::vector<std::string>& val):ComposedAny(new TypeCodeSeq("","",Runtime::_tc_string)),
                                                              _alloc(Runtime::_tc_string->getSizeInByteOfAnyReprInSeq())
{
  for(vector<string>::const_iterator iter=val.begin();iter!=val.end();iter++)
    {
      AtomAnyPtr tmp=AtomAny::New(*iter);
      pushBack(tmp);
    }
}

void SequenceAny::realloc(char *endOfCurrentAllocated, const Any *elem)
{
  unsigned int oldSize=_alloc._finish-_alloc._start;
  unsigned int newSize = oldSize != 0 ? 2 * oldSize : _alloc._sizeOf1Elm;
  char *newStart=_alloc.allocate(newSize);
  //
  char *newFinish=performCpy(_alloc._start, endOfCurrentAllocated,newStart);
  _alloc.construct(newFinish, elem);
  newFinish+=_alloc._sizeOf1Elm;
  newFinish=performCpy(endOfCurrentAllocated, _alloc._finish, newFinish);
  //
  for (char *cur=_alloc._start;cur!=_alloc._finish;cur+=_alloc._sizeOf1Elm)
    _alloc.destroy(cur,_type->contentType());
  _alloc.deallocate(_alloc._start);
  _alloc._start = newStart;
  _alloc._finish = newFinish;
  _alloc._endOfStorage=newStart+newSize;
}

char *SequenceAny::performCpy(char *srcStart, char *srcFinish, char *destStart)
{
  char *cur=destStart;
  for (;srcStart != srcFinish; srcStart+=_alloc._sizeOf1Elm, cur+=_alloc._sizeOf1Elm)
    _alloc.construct(cur, srcStart, _type->contentType(),false);
  return cur;
}

ArrayAny::~ArrayAny()
{
  const TypeCode *subType=_type->contentType();
  unsigned sizePerContent=subType->getSizeInByteOfAnyReprInSeq();
  unsigned int size=((TypeCodeArray *)_type)->getStaticLgth();
  char *tmp=_data;
  for(unsigned i=0;i<size;i++,tmp+=sizePerContent)
    subType->destroyZippedAny(tmp);
  delete [] _data;
}

ArrayAny::ArrayAny(const TypeCode *typeOfContent, unsigned int lgth):ComposedAny(new TypeCodeArray("","",typeOfContent,lgth))
{
  _data=new char[_type->getSizeInByteOfAnyReprInSeq()];
  for(unsigned int i=0;i<_type->getSizeInByteOfAnyReprInSeq();i++)
    _data[i]=SeqAlloc::DFT_CHAR_VAR;
}

ArrayAny::ArrayAny(char *data, TypeCodeArray * type):ComposedAny(type,false),_data(0)
{
  _data=new char[_type->getSizeInByteOfAnyReprInSeq()];
  const TypeCode *subType=_type->contentType();
  unsigned sizePerContent=subType->getSizeInByteOfAnyReprInSeq();
  for(unsigned i=0;i<type->getStaticLgth();i++)
    subType->putReprAtPlace(_data+i*sizePerContent,data+i*sizePerContent,false);
}

ArrayAny::ArrayAny(const ArrayAny& other):ComposedAny(other)
{
  _data=new char[_type->getSizeInByteOfAnyReprInSeq()];
  const TypeCode *subType=_type->contentType();
  unsigned sizePerContent=subType->getSizeInByteOfAnyReprInSeq();
  for(unsigned i=0;i<((TypeCodeArray *)_type)->getStaticLgth();i++)
    subType->putReprAtPlace(_data+i*sizePerContent,other._data+i*sizePerContent,true);
}

ArrayAny::ArrayAny(const int *val, unsigned int lgth):ComposedAny(new TypeCodeArray("","",Runtime::_tc_int,lgth)),
                                                      _data(0)
{
  _data=new char[_type->getSizeInByteOfAnyReprInSeq()];
  memcpy(_data,val,_type->getSizeInByteOfAnyReprInSeq());
}

ArrayAny::ArrayAny(const bool *val, unsigned int lgth):ComposedAny(new TypeCodeArray("","",Runtime::_tc_bool,lgth)),
                                                       _data(0)
{
  _data=new char[_type->getSizeInByteOfAnyReprInSeq()];
  memcpy(_data,val,_type->getSizeInByteOfAnyReprInSeq());
}

ArrayAny::ArrayAny(const double *val, unsigned int lgth):ComposedAny(new TypeCodeArray("","",Runtime::_tc_double,lgth)),
                                                         _data(0)
{
  _data=new char[_type->getSizeInByteOfAnyReprInSeq()];
  memcpy(_data,val,_type->getSizeInByteOfAnyReprInSeq());
}

ArrayAny::ArrayAny(const std::vector<int>& val):ComposedAny(new TypeCodeArray("","",Runtime::_tc_int,val.size())),
                                                _data(0)
{
  _data=new char[_type->getSizeInByteOfAnyReprInSeq()];
  memcpy(_data,&val[0],_type->getSizeInByteOfAnyReprInSeq());
}

ArrayAny::ArrayAny(const std::vector<double>& val):ComposedAny(new TypeCodeArray("","",Runtime::_tc_double,val.size())),
                                                _data(0)
{
  _data=new char[_type->getSizeInByteOfAnyReprInSeq()];
  memcpy(_data,&val[0],_type->getSizeInByteOfAnyReprInSeq());
}

ArrayAny::ArrayAny(const std::vector<std::string>& val):ComposedAny(new TypeCodeArray("","",Runtime::_tc_string,val.size())),
                                                        _data(0)
{
  _data=new char[_type->getSizeInByteOfAnyReprInSeq()];
  unsigned i=0;
  const TypeCode *subType=_type->contentType();
  unsigned sizePerContent=subType->getSizeInByteOfAnyReprInSeq();
  for(vector<std::string>::const_iterator iter=val.begin();iter!=val.end();iter++,i++)
    {
      StringOnHeap *st=new StringOnHeap(*iter);
      memcpy(_data+i*sizePerContent,&st,sizePerContent);
    }
}

void ArrayAny::setEltAtRank(int i, const Any *elem) throw(YACS::Exception)
{
  checkTypeOf(elem);
  const TypeCode *subType=_type->contentType();
  subType->destroyZippedAny(_data+i*subType->getSizeInByteOfAnyReprInSeq());
  elem->putMyReprAtPlace(_data+i*subType->getSizeInByteOfAnyReprInSeq());
}

bool ArrayAny::operator ==(const Any& other) const
{
  if(!_type->isA(other.getType()))
    return false;
  const ArrayAny& otherC=(const ArrayAny&) other;//cast granted due to previous lines
  for(unsigned i=0;i<((const TypeCodeArray *)_type)->getStaticLgth();i++)
    if(!((*(*this)[i])==(*otherC[i])))
      return false;
  return true;
}

AnyPtr ArrayAny::operator[](int i) const throw(YACS::Exception)
{
  const TypeCode *subType=_type->contentType();
  unsigned sizePerContent=subType->getSizeInByteOfAnyReprInSeq();
  if(i<0 || i>=((TypeCodeArray *)_type)->getStaticLgth())
    throw Exception("Trying to access to an invalid index in an Any Tuple");
  return _type->contentType()->getOrBuildAnyFromZippedData(_data+i*sizePerContent);
}

unsigned int ArrayAny::size() const
{
  return ((TypeCodeArray *)_type)->getStaticLgth();
}

Any *ArrayAny::clone() const
{
  return new ArrayAny(*this);
}

ArrayAny *ArrayAny::New(const TypeCode *typeOfContent, unsigned int lgth)
{
  return new ArrayAny(typeOfContent,lgth);
}

void ArrayAny::putMyReprAtPlace(char *data) const
{
  const TypeCode *subType=_type->contentType();
  unsigned sizePerContent=subType->getSizeInByteOfAnyReprInSeq();
  for(unsigned i=0;i<((const TypeCodeArray *)_type)->getStaticLgth();i++)
    subType->putReprAtPlace(data+i*sizePerContent,_data+i*sizePerContent,false);
}

void ArrayAny::putReprAtPlace(char *data, const char *src, const TypeCodeArray *type, bool deepCpy)
{
  const TypeCode *subType=type->contentType();
  unsigned sizePerContent=subType->getSizeInByteOfAnyReprInSeq();
  for(unsigned i=0;i<type->getStaticLgth();i++)
    subType->putReprAtPlace(data+i*sizePerContent,src+i*sizePerContent,deepCpy);
}

void ArrayAny::destroyReprAtPlace(char *data, const TypeCodeArray *type)
{
  const TypeCode *subType=type->contentType();
  unsigned sizePerContent=subType->getSizeInByteOfAnyReprInSeq();
  for(unsigned i=0;i<type->getStaticLgth();i++)
    subType->destroyZippedAny(data+i*sizePerContent);
}

AnyPtr ArrayAny::getOrBuildFromData(char *data, const TypeCodeArray *type)
{
  Any *ret;
  ret=new ArrayAny(data,(TypeCodeArray *)type);
  return AnyPtr(ret);
}

bool ArrayAny::takeInChargeStorageOf(TypeCode *type)
{
  DynType typ=type->kind();
  return (typ==Array);
}

Any *StructAny::clone() const
{
  return new StructAny(*this);
}

bool StructAny::operator ==(const Any& other) const
{
  if(!_type->isA(other.getType()))
    return false;
  const TypeCodeStruct *typeC=(const TypeCodeStruct *)_type;
  vector< pair<string,TypeCode*> >::const_iterator iter;
  for(iter=typeC->_members.begin();iter!=typeC->_members.end();iter++)
    if(!((*(*this)[(*iter).first.c_str()]==(*other[(*iter).first.c_str()]))))
      return false;
  return true;
}

AnyPtr StructAny::operator[](int i) const throw(YACS::Exception)
{
  const char what[]="StructAny::operator[](int i) : Struct key are strings not integers.";
  throw Exception(what);
}

AnyPtr StructAny::operator[](const char *key) const throw(YACS::Exception)
{
  const TypeCodeStruct *typeC=(const TypeCodeStruct *)_type;
  char *whereToGet=_data;
  vector< pair<string,TypeCode*> >::const_iterator iter;
  for(iter=typeC->_members.begin();iter!=typeC->_members.end();iter++)
    if((*iter).first!=key)
      whereToGet+=(*iter).second->getSizeInByteOfAnyReprInSeq();
    else
      break;
  if(iter==typeC->_members.end())
    {
      string what("Unexisting key \""); what+=key; what+="\" for struct extraction.";
      throw Exception(what);
    }
  return (*iter).second->getOrBuildAnyFromZippedData(whereToGet);
}

void StructAny::setEltAtRank(int i, const Any *elem) throw(YACS::Exception)
{
  const char what[]="Struct key are strings not integers.";
  throw Exception(what);
}

void StructAny::setEltAtRank(const char *key, const Any *elem) throw(YACS::Exception)
{
  const TypeCodeStruct *typeC=(const TypeCodeStruct *)_type;
  unsigned offset;
  const TypeCode *tcOnKey=typeC->getMember(key,offset);
  if(!tcOnKey)
    throw Exception("StructAny::setEltAtRank : invalid key given.");
  if(!elem->getType()->isA(tcOnKey))
    throw Exception("StructAny::setEltAtRank : invalid data type on the specified given key.");
  tcOnKey->destroyZippedAny(_data+offset);
  elem->putMyReprAtPlace(_data+offset);
}

void StructAny::putMyReprAtPlace(char *data) const
{
  const TypeCodeStruct *typeC=(const TypeCodeStruct *)_type;
  unsigned offset=0;
  vector< pair<string,TypeCode*> >::const_iterator iter;
  for(iter=typeC->_members.begin();iter!=typeC->_members.end();iter++)
    {
      (*iter).second->putReprAtPlace(data+offset,_data+offset,false);
      offset+=(*iter).second->getSizeInByteOfAnyReprInSeq();
    }
}

void StructAny::putReprAtPlace(char *data, const char *src, const TypeCodeStruct *type, bool deepCpy)
{
  unsigned offset=0;
  vector< pair<string,TypeCode*> >::const_iterator iter;
  for(iter=type->_members.begin();iter!=type->_members.end();iter++)
    {
      (*iter).second->putReprAtPlace(data+offset,src+offset,deepCpy);
      offset+=(*iter).second->getSizeInByteOfAnyReprInSeq();
    }
}

void StructAny::destroyReprAtPlace(char *data, const TypeCodeStruct *type)
{
  char *whereToGet=data;
  vector< pair<string,TypeCode*> >::const_iterator iter;
  for(iter=type->_members.begin();iter!=type->_members.end();iter++)
    {
      (*iter).second->destroyZippedAny(whereToGet);
      whereToGet+=(*iter).second->getSizeInByteOfAnyReprInSeq();
    }
}

AnyPtr StructAny::getOrBuildFromData(char *data, const TypeCodeStruct *type)
{
  Any *ret;
  ret=new StructAny(data,(TypeCodeStruct *)type);
  return AnyPtr(ret);
}

StructAny::~StructAny()
{
  const TypeCodeStruct *typeC=(const TypeCodeStruct *)_type;
  vector< pair<string,TypeCode*> >::const_iterator iter;
  char *whereToGet=_data;
  for(iter=typeC->_members.begin();iter!=typeC->_members.end();iter++)
    {
      (*iter).second->destroyZippedAny(whereToGet);
      whereToGet+=(*iter).second->getSizeInByteOfAnyReprInSeq();
    }
  delete [] _data;
}

StructAny::StructAny(TypeCodeStruct *type):ComposedAny(type,false)
{
  _data=new char[_type->getSizeInByteOfAnyReprInSeq()];
  for(unsigned int i=0;i<_type->getSizeInByteOfAnyReprInSeq();i++)
    _data[i]=SeqAlloc::DFT_CHAR_VAR;
}

StructAny::StructAny(const StructAny& other):ComposedAny(other)
{
  _data=new char[_type->getSizeInByteOfAnyReprInSeq()];
  const TypeCodeStruct *typeC=(const TypeCodeStruct *)_type;
  vector< pair<string,TypeCode*> >::const_iterator iter;
  unsigned offset=0;
  for(iter=typeC->_members.begin();iter!=typeC->_members.end();iter++)
    {
     (*iter).second->putReprAtPlace(_data+offset,other._data+offset,true);
     offset+=(*iter).second->getSizeInByteOfAnyReprInSeq();
    }
}

StructAny::StructAny(char *data, TypeCodeStruct * type):ComposedAny(type,false),_data(0)
{
  _data=new char[_type->getSizeInByteOfAnyReprInSeq()];
  vector< pair<string,TypeCode*> >::const_iterator iter;
  unsigned offset=0;
  for(iter=type->_members.begin();iter!=type->_members.end();iter++)
    {
      (*iter).second->putReprAtPlace(_data+offset,data+offset,false);
      offset+=(*iter).second->getSizeInByteOfAnyReprInSeq();
    }
}

StructAny *StructAny::New(TypeCodeStruct *type)
{
  return new StructAny(type);
}
