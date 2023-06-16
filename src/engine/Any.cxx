// Copyright (C) 2006-2023  CEA/DEN, EDF R&D
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

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>

#include <algorithm>
#include <cstring>
#include <cstdlib>

using namespace YACS::ENGINE;
using namespace std;

// forbidden value int=-269488145 double=-1.54947e+231 bool=239
const char SeqAlloc::DFT_CHAR_VAR=-17;//0xEF

constexpr unsigned NB_BITS = 6;

constexpr unsigned char TAB[64]={46, 61, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122};

unsigned char BitAtPosSimple(char val, std::size_t bitPos)
{
  return (val >> bitPos) & 0x1;
}

unsigned char BitAtPos(char pt0, char pt1, std::size_t bitPos)
{
  if(bitPos<8)
    return BitAtPosSimple(pt0,bitPos);
  else
    return BitAtPosSimple(pt1,bitPos-8);
}

unsigned char ChunkInternal(char pt0, char pt1, std::size_t startBitIdInByte)
{
  unsigned char ret(0);
  for(unsigned i = 0; i<NB_BITS; ++i)
    {
      ret |= BitAtPos(pt0,pt1,startBitIdInByte+i);
      ret <<= 1;
    }
  ret >>= 1;
  return ret;
}

unsigned char ChunkAtPos(const char *pt, std::size_t len, std::size_t posChunk)
{
  std::size_t startByte((posChunk*NB_BITS)/8);
  std::size_t startBitIdInByte((posChunk*NB_BITS)%8);
  char pt1(startByte!=len-1?pt[startByte+1]:pt[startByte]);
  return ChunkInternal(pt[startByte],pt1,startBitIdInByte);
}

std::size_t OnOff(std::size_t i)
{
  if(i!=0)
    return 1;
  return 0;
}

std::string YACS::ENGINE::ToBase64(const std::string& bytes)
{//64 == 2**6
  const char *bytesPt(bytes.c_str());
  std::size_t input_len(bytes.size());
  std::size_t input_len_bit(input_len*8);
  std::size_t nb_chunks( input_len_bit/NB_BITS + OnOff((NB_BITS - input_len_bit%NB_BITS)%NB_BITS) );
  std::string ret(nb_chunks,'\0');
  for(std::size_t i=0;i<nb_chunks;++i)
    {
      unsigned char cp(ChunkAtPos(bytesPt,input_len, i));
      ret[i] = TAB[cp];
    }
  return ret;
}

constexpr unsigned MAX_VAL_TAB2=123;

constexpr unsigned NOT_OK_VAL = 128;

constexpr unsigned char TAB2[MAX_VAL_TAB2] = { NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, 0, NOT_OK_VAL, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, 1, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, NOT_OK_VAL, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63 };

unsigned char BitAtPosSimple2(char val, std::size_t bitPos)
{
  return ( val >> (5-bitPos) ) & 0x1;
}

char BitAtPosOnChunk(char pt0, char pt1, std::size_t bitPos)
{
  if(bitPos<6)
    return BitAtPosSimple2(pt0,bitPos);
  else
    return BitAtPosSimple2(pt1,bitPos-6);
}

unsigned char CheckEntry(char c)
{
  if( ((unsigned) c) < MAX_VAL_TAB2 )
    {
      unsigned char ret(TAB2[(unsigned char)c]);
      if(ret != NOT_OK_VAL)
        return ret;
      throw YACS::Exception("Invalid character found !");
    }
  throw YACS::Exception("Invalid character found !");
}

char ByteInternal(char c0, char c1, std::size_t startBitIdInByte)
{
  unsigned char ret(0);
  char ct0(CheckEntry(c0)),ct1(CheckEntry(c1));
  for(int i = 7; i>=0; --i)
    {
      ret |= BitAtPosOnChunk(ct0,ct1,startBitIdInByte+i);
      if(i!=0)
        ret <<= 1;
    }
  return ret;
}

char ByteAtPos(const char *chunckPt, std::size_t bytePos)
{
  std::size_t startChunk((bytePos*8)/NB_BITS);
  std::size_t startBitId((bytePos*8)%NB_BITS);
  return ByteInternal(chunckPt[startChunk],chunckPt[startChunk+1],startBitId);
}

std::string YACS::ENGINE::FromBase64(const std::string& bytes)
{
  std::size_t nb_chunks(bytes.size());
  const char *chunckPt(bytes.c_str());
  std::size_t nb_bytes_output((nb_chunks*NB_BITS)/8);
  std::string ret(nb_bytes_output,'\0');
  for(std::size_t i = 0; i<nb_bytes_output; ++i)
    {
      ret[i] = ByteAtPos(chunckPt,i);
    }
  return ret;
}

/*!
 * Method used at load time in case of non base64 bytes in input (a throw during decoding). If so, the input bytes is returned.
 */
std::string YACS::ENGINE::FromBase64Safe(const std::string& bytes)
{
  try
    {
      return FromBase64(bytes);
    }
  catch(const YACS::Exception& e)
    {
      return bytes;
    }
}

StringOnHeap::StringOnHeap(const char *val):_str(strdup(val)),_len(strlen(val)),_dealloc(0)
{
}

StringOnHeap::StringOnHeap(const char *val, std::size_t len):_dealloc(0),_len(len)
{
  _str=(char *)malloc(len+1);
  std::copy(val,val+len,_str);
  _str[len]='\0';
}

StringOnHeap::StringOnHeap(const std::string& val):_dealloc(0),_len(val.size()),_str(nullptr)
{
  _str=(char *)malloc(val.size()+1);
  std::copy(val.cbegin(),val.cend(),_str);
  _str[val.size()]='\0';
}

/*! 
 * \note : no copy is performed if a deallocator is given.
 * \param val     : String in C format that is NOT copied if
 *                  deAlloc != 0
 * \param deAlloc : pointer on function to deallocate val after
 *                  last use.
 */
StringOnHeap::StringOnHeap(char *val, Deallocator deAlloc):_len(0),_dealloc(deAlloc)
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
  if(_len==0)
    return new StringOnHeap(_str);
  else
    return new StringOnHeap(_str,_len);
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

AtomAny::AtomAny(const std::string& val, TypeCode* type):Any(type)
{
  _value._s=new StringOnHeap(val);
}

AtomAny::AtomAny(const AtomAny& other):Any(other)
{
  if(_type->isA(Runtime::_tc_string) || _type->kind()==YACS::ENGINE::Objref)
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
  if(type->isA(Runtime::_tc_string) || _type->kind()==YACS::ENGINE::Objref)
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

AtomAny *AtomAny::New(const std::string& val, TypeCode *type)
{
  return new AtomAny(val,type);
}

AnyPtr AtomAny::operator[](int i) const
{
  throw InvalidExtractionException(_type->kind(),Sequence);
}

AnyPtr AtomAny::operator[](const char *key) const
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
  else if(_type->isA(Runtime::_tc_string) || _type->kind()==Objref)
    return (*_value._s)==*(otherC._value._s);
  else
    return false;
}

int AtomAny::getIntValue() const
{
  if(_type->isA(Runtime::_tc_int))
    return _value._i;
  else
    throw Exception("Value is not an int");
}

bool AtomAny::getBoolValue() const
{
  if(_type->isA(Runtime::_tc_bool))
    return _value._b;
  else
    throw Exception("Value is not a bool");
}

double AtomAny::getDoubleValue() const
{
  if(_type->isA(Runtime::_tc_double))
    return _value._d;
  else
    throw Exception("Value is not a double");
}

std::string AtomAny::getStringValue() const
{
  if(_type->isA(Runtime::_tc_string) || _type->kind()==YACS::ENGINE::Objref)
    {
      std::size_t sz(_value._s->size());
      if(sz==0)
        return string(_value._s->cStr());
      else
        return string(_value._s->cStr(),sz);
    }
  else
    throw Exception("Value is not a string");
}

const char *AtomAny::getBytesValue(std::size_t& len) const
{
  if(_type->isA(Runtime::_tc_string) || _type->kind()==YACS::ENGINE::Objref)
    {
      len=_value._s->size();
      return _value._s->cStr();
    }
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
  if(_type->isA(Runtime::_tc_string) || _type->kind()==YACS::ENGINE::Objref)
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
  if(type->isA(Runtime::_tc_string) || type->kind()==YACS::ENGINE::Objref)
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
  if(typ==String || typ==Objref)
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
  if(_type->kind() == String || _type->kind()==Objref)
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

AnyPtr ComposedAny::operator[](const char *key) const
{
  throw Exception("AtomAny::operator[] : try to get a part of a partitionned data not localizable by a string.");
}

void ComposedAny::checkTypeOf(const Any *elem) const
{
  if(!elem->getType()->isA(_type->contentType()))
    throw Exception("ComposedAny::checkTypeOf : invalid type.");
}

int ComposedAny::getIntValue() const
{
 throw InvalidExtractionException(_type->kind(),Runtime::_tc_int->kind());
}

bool ComposedAny::getBoolValue() const
{
  throw InvalidExtractionException(_type->kind(),Runtime::_tc_bool->kind());
}

double ComposedAny::getDoubleValue() const
{
  throw InvalidExtractionException(_type->kind(),Runtime::_tc_double->kind());
}

std::string ComposedAny::getStringValue() const
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

void SequenceAny::setEltAtRank(int i, const Any *elem)
{
  checkTypeOf(elem);
  _alloc.destroy(_alloc._start+i*_alloc._sizeOf1Elm,_type->contentType());
  _alloc.construct(_alloc._start+i*_alloc._sizeOf1Elm,elem);
}

AnyPtr SequenceAny::operator[](int i) const
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
    return new SequenceAny(typeOfContent);
}

SequenceAny *SequenceAny::New(const TypeCode *typeOfContent, unsigned lgth)
{
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

void ArrayAny::setEltAtRank(int i, const Any *elem)
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

AnyPtr ArrayAny::operator[](int i) const
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

AnyPtr StructAny::operator[](int i) const
{
  const char what[]="StructAny::operator[](int i) : Struct key are strings not integers.";
  throw Exception(what);
}

AnyPtr StructAny::operator[](const char *key) const
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

void StructAny::setEltAtRank(int i, const Any *elem)
{
  const char what[]="Struct key are strings not integers.";
  throw Exception(what);
}

void StructAny::setEltAtRank(const char *key, const Any *elem)
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
