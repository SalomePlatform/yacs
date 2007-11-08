
#include "TypeCode.hxx"
#include <sstream>
#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char *TypeCode::KIND_STR_REPR []={ "None", "Double", "Int", "String", "Bool", "Objref", "Sequence", "Array","Struct" };

// --- TypeCode

TypeCode::TypeCode(DynType kind):_kind(kind)
{
}

TypeCode::TypeCode(const TypeCode& tc):_kind(tc._kind)
{
}

TypeCode::~TypeCode()
{
}

DynType TypeCode::kind() const
{
  return _kind;
}

TypeCode *TypeCode::clone() const
{
  return new TypeCode(*this);
}

void TypeCode::putReprAtPlace(char *pt, const char *val, bool deepCpy) const
{
  AtomAny::putReprAtPlace(pt,val,this,deepCpy);
}

void TypeCode::destroyZippedAny(char *data) const
{
  AtomAny::destroyReprAtPlace(data,this);
}

AnyPtr TypeCode::getOrBuildAnyFromZippedData(char *data) const
{
  return AtomAny::getOrBuildFromData(data,this);
}

const char * TypeCode::name() const throw(Exception)
{
  //throw Exception("No name");
  return id();
}

const char * TypeCode::shortName() const
{
  //throw Exception("No shortName");
  return id();
}

const char * TypeCode::id() const throw(Exception)
{
  switch(_kind)
    {
    case Double:
      return "Double";
    case Int:
      return "Int";
    case String:
      return "String";
    case Bool:
      return "Bool";
    default:
      return "";
    }
}

int TypeCode::isA(const char* id) const throw(Exception)
{
  throw Exception("Not implemented for this type");
}

int TypeCode::isA(const TypeCode* tc) const 
{
  if(_kind == tc->kind()) return 1;
  return 0;
}

//! Check if this TypeCode is adaptable to a given TypeCode (tc)
/*!
 * this TypeCode is adaptable to tc if tc type can be converted to this type
 *
 *   \param tc : the TypeCode that must be convertible to this
 */
int TypeCode::isAdaptable(const TypeCode* tc) const
{
  switch(_kind)
    {
    case Double:
      if (tc->kind() == Double) return 1;
      if (tc->kind() == Int) return 1;
      return 0;
    case Int:
      if (tc->kind() == Int) return 1;
      return 0;
    case String:
      if (tc->kind() == String) return 1;
      return 0;
    case Bool:
      if (tc->kind() == Bool) return 1;
      if (tc->kind() == Int) return 1;
      return 0;
    default:
      //objref, sequence, ...
      return 0;
    }
}

unsigned TypeCode::getSizeInByteOfAnyReprInSeq() const
{
  switch(_kind)
    {
    case Double:
      return sizeof(double);
    case Int:
      return sizeof(int);
    case String:
      return sizeof(StringOnHeap *);
    case Bool:
      return sizeof(bool);
    default:
      return sizeof(void *);
    }
}

const TypeCode * TypeCode::contentType() const throw(Exception)
{
  throw Exception("No content type");
};

static inline int validChar0(char c)
{
  return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

static inline int validNextChar(char c)
{
  return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
          (c >= '0' && c <= '9') || (c == '_') || (c == '/'));
}

static void checkValidName(const char* name)
{
  int ok = 1;
  if (*name) 
    {
      if (!validChar0(*name++)) ok = 0;
      for(; ok && *name; name++) if (!validNextChar(*name)) ok = 0; 
    }
  if (!ok)throw YACS::Exception("Invalid Name");
}

const char *TypeCode::getKindRepr(DynType kind)
{
  return KIND_STR_REPR[(int)kind];
}

const char * TypeCode::getKindRepr() const
{
  return KIND_STR_REPR[(int)_kind];
}

//! static factory of object reference type given an id and a name
TypeCode * TypeCode::interfaceTc(const char* id,
                                  const char* name)
{
  checkValidName(name);
  return new TypeCodeObjref(id, name);
};

//! static factory of object reference type given an id, a name and a list of base types
/*!
 *   \param id :  the id
 *   \param name :  the name
 *   \param ltc :  the list of base types
 *
 *   The name must be a valid one (throw Exception is not)
 */
TypeCode * TypeCode::interfaceTc(const char* id,
                                  const char* name,
                                  const std::list<TypeCodeObjref *>& ltc)
{
  checkValidName(name);
  return new TypeCodeObjref(id, name,ltc);
}


//! static factory of sequence type given an id, a name and a content type
TypeCode * TypeCode::sequenceTc(const char* id,
                                 const char* name,
                                 TypeCode *content)
{
  return new TypeCodeSeq(id, name,content);
};
//! static factory of struct type given an id and a name 
TypeCode * TypeCode::structTc(const char* id,
                                 const char* name)
{
  return new TypeCodeStruct(id, name);
};

TypeCodeComposed::TypeCodeComposed(const TypeCodeComposed& other):TypeCode(other),
                                                                  _name(other._name),_repoId(other._shortName),
                                                                  _shortName(other._shortName)
{
}

TypeCodeComposed::TypeCodeComposed(DynType kind, const char* repositoryId, const char* name):TypeCode(kind),
                                                                                             _repoId(repositoryId),_name(name)
{
  string::size_type debut =_name.find_last_of('/');
  if(debut == std::string::npos)
    _shortName= name;
  else
    _shortName=_name.substr(debut+1);
}

// --- TypeCodeObjref


TypeCodeObjref::TypeCodeObjref(const char* repositoryId, 
                               const char* name) : TypeCodeComposed(Objref,repositoryId,name)
{
}


TypeCodeObjref::~TypeCodeObjref()
{
  list<TypeCodeObjref *>::iterator iter;
  for(iter=_listOfBases.begin();iter != _listOfBases.end(); iter++)
    (*iter)->decrRef();
}

TypeCode *TypeCodeObjref::clone() const
{
  return new TypeCodeObjref(*this);
}

void TypeCodeObjref::putReprAtPlace(char *pt, const char *val, bool deepCpy) const
{
  throw Exception("Not implemented yet : YACS::Any for objs ref");
}

void TypeCodeObjref::destroyZippedAny(char *data) const
{
  throw Exception("Not implemented yet : YACS::Any for objs ref");
}

AnyPtr TypeCodeObjref::getOrBuildAnyFromZippedData(char *data) const
{
  throw Exception("Not implemented yet : YACS::Any for objs ref");
}

const char * TypeCodeObjref::id() const throw(Exception)
{
  return _repoId.c_str();
};

const char * TypeCodeObjref::name() const throw(Exception)
{
  return _name.c_str();
}

const char * TypeCodeObjref::shortName() const
{
  return _shortName.c_str();
}

TypeCodeObjref::TypeCodeObjref(const char* repositoryId,
                               const char* name,
                               const std::list<TypeCodeObjref *>& ltc) : TypeCodeComposed(Objref,repositoryId,name)
{
  _listOfBases=ltc;
  list<TypeCodeObjref *>::const_iterator iter;
  for(iter=_listOfBases.begin();iter != _listOfBases.end(); iter++)
    (*iter)->incrRef();
}

//! Check if this TypeCode is derived from a TypeCode with a given id
/*!
 *   \param id :  a given id
 *   \return     1 if true, 0 if false
 */
int TypeCodeObjref::isA(const char* id) const throw(Exception)
{
  if(_repoId == id)return 1;
  list<TypeCodeObjref *>::const_iterator iter;
  for(iter=_listOfBases.begin();iter != _listOfBases.end(); iter++)
    {
      if ((*iter)->isA(id)) return 1;
    }
  return 0;
}

//! Check if this TypeCode is derived from a given TypeCode
/*!
 *   \param tc : the given TypeCode
 *   \return    1 if true, 0 if false
 */
int TypeCodeObjref::isA(const TypeCode* tc) const
{
  return isA(tc->id());
}

//! Check if this TypeCode is adaptable to a given TypeCode (tc)
/*!
 *   \param tc : the given TypeCode
 *   \return    1 if true, 0 if false
 */
int TypeCodeObjref::isAdaptable(const TypeCode* tc) const
{
  if(_kind == tc->kind()) return isA(tc->id());
  return 0;
}

TypeCodeObjref::TypeCodeObjref(const TypeCodeObjref& other):TypeCodeComposed(other),
                                                            _listOfBases(other._listOfBases)
{
  list<TypeCodeObjref *>::const_iterator iter;
  for(iter=other._listOfBases.begin();iter!=other._listOfBases.end();iter++)
    (*iter)->incrRef();
}

// --- TypeCodeSeq


//! Create a sequence type with a given name, a given id and a given contained type.
/*!
 *   \param repositoryId : the given id
 *   \param name : the given name
 *   \param content : the given contained TypeCode
 */
TypeCodeSeq::TypeCodeSeq(const char* repositoryId,
                         const char* name, 
                         const TypeCode *content) : TypeCodeComposed(Sequence,repositoryId,name), _content(content)
{
  _content->incrRef();
}

TypeCodeSeq::~TypeCodeSeq()
{
  ((TypeCode *)_content)->decrRef();
}

TypeCode *TypeCodeSeq::clone() const
{
  return new TypeCodeSeq(*this);
}

void TypeCodeSeq::putReprAtPlace(char *pt, const char *val, bool deepCpy) const
{
  SequenceAny::putReprAtPlace(pt,val,this,deepCpy);
}

void TypeCodeSeq::destroyZippedAny(char *data) const
{
  SequenceAny::destroyReprAtPlace(data,this);
}

unsigned TypeCodeSeq::getSizeInByteOfAnyReprInSeq() const
{
  return sizeof(void*);
}

AnyPtr TypeCodeSeq::getOrBuildAnyFromZippedData(char *data) const
{
  return SequenceAny::getOrBuildFromData(data,this);
}

const char * TypeCodeSeq::id() const throw(Exception)
{
  return _repoId.c_str();
}

const char * TypeCodeSeq::name() const throw(Exception)
{
  return _name.c_str();
}
const char * TypeCodeSeq::shortName() const
{
  return _shortName.c_str();
}

const TypeCode * TypeCodeSeq::contentType() const throw(Exception)
{
  return _content;
}

int TypeCodeSeq::isA(const TypeCode* tc) const
{
  if(_kind == tc->kind())
    return _content->isA(tc->contentType());
  return 0;
}

//! Check if this TypeCode is adaptable to a given TypeCode (tc)
/*!
 *   \param tc : the given TypeCode
 *   \return    1 if true, 0 if false
 */
int TypeCodeSeq::isAdaptable(const TypeCode* tc) const
{
  if(_kind == tc->kind())
    return contentType()->isAdaptable(tc->contentType());
  return 0;
}

TypeCodeSeq::TypeCodeSeq(const TypeCodeSeq& tc):TypeCodeComposed(tc),
                                                _content(tc._content)
{
  _content->incrRef();
}

// --- TypeCodeArray


//! Create an Array type with a given name, a given id and a given contained type.
/*!
 *   \param repositoryId : the given id
 *   \param name : the given name
 *   \param content : the given contained TypeCode
 */
TypeCodeArray::TypeCodeArray(const char* repositoryId,
                             const char* name, 
                             const TypeCode *content,
                             unsigned staticLgth) : TypeCodeComposed(Array,repositoryId,name), _content(content),_staticLgth(staticLgth)
{
  _content->incrRef();
}

TypeCodeArray::~TypeCodeArray()
{
  ((TypeCode *)_content)->decrRef();
}

TypeCode *TypeCodeArray::clone() const
{
  return new TypeCodeArray(*this);
}

void TypeCodeArray::putReprAtPlace(char *pt, const char *val, bool deepCpy) const
{
  ArrayAny::putReprAtPlace(pt,val,this,deepCpy);
}

void TypeCodeArray::destroyZippedAny(char *data) const
{
  ArrayAny::destroyReprAtPlace(data,this);
}

AnyPtr TypeCodeArray::getOrBuildAnyFromZippedData(char *data) const
{
  return ArrayAny::getOrBuildFromData(data,this);
}

const char * TypeCodeArray::id() const throw(Exception)
{
  return _repoId.c_str();
}

const char * TypeCodeArray::name() const throw(Exception)
{
  return _name.c_str();
}
const char * TypeCodeArray::shortName() const
{
  return _shortName.c_str();
}

unsigned TypeCodeArray::getStaticLgth() const
{
  return _staticLgth;
}

const TypeCode * TypeCodeArray::contentType() const throw(Exception)
{
  return _content;
}

int TypeCodeArray::isA(const TypeCode* tc) const
{
  if(_kind == tc->kind())
    if(_content->isA(tc->contentType()))
      {
        const TypeCodeArray *tcC=dynamic_cast<const TypeCodeArray *>(tc);
        if(tcC)
          return tcC->getStaticLgth()==_staticLgth;
        return 0;
      }
  return 0;
}

//! Check if this TypeCode is adaptable to a given TypeCode (tc)
/*!
 *   \param tc : the given TypeCode
 *   \return    1 if true, 0 if false
 */
int TypeCodeArray::isAdaptable(const TypeCode* tc) const
{
  if(_kind == tc->kind())
    return contentType()->isAdaptable(tc->contentType());
  return 0;
}

TypeCodeArray::TypeCodeArray(const TypeCodeArray& tc):TypeCodeComposed(tc),
                                                      _content(tc._content),
                                                      _staticLgth(tc._staticLgth)
{
  _content->incrRef();
}

unsigned TypeCodeArray::getSizeInByteOfAnyReprInSeq() const
{
  return _staticLgth*_content->getSizeInByteOfAnyReprInSeq();
}

// --- TypeCodeStruct


//! Create a struct type with a given name and a given id 
/*!
 *   \param repositoryId : the given id
 *   \param name : the given name
 */
TypeCodeStruct::TypeCodeStruct(const char* repositoryId, 
                               const char* name) : TypeCodeComposed(Struct,repositoryId,name)
{
}

TypeCodeStruct::~TypeCodeStruct()
{
}

TypeCode *TypeCodeStruct::clone() const
{
  return new TypeCodeStruct(*this);
}

TypeCodeStruct::TypeCodeStruct(const TypeCodeStruct& tc):TypeCodeComposed(tc),_members(tc._members)
{
  for(vector< std::pair<std::string,TypeCode*> >::iterator iter=_members.begin();iter!=_members.end();iter++)
    (*iter).second->incrRef();
}

void TypeCodeStruct::putReprAtPlace(char *pt, const char *val, bool deepCpy) const
{
  StructAny::putReprAtPlace(pt,val,this,deepCpy);
}

void TypeCodeStruct::destroyZippedAny(char *data) const
{
  StructAny::destroyReprAtPlace(data,this);
}

AnyPtr TypeCodeStruct::getOrBuildAnyFromZippedData(char *data) const
{
  return StructAny::getOrBuildFromData(data,this);
}

const char * TypeCodeStruct::id() const throw(Exception)
{
  return _repoId.c_str();
};

const char * TypeCodeStruct::name() const throw(Exception)
{
  return _name.c_str();
}

const char * TypeCodeStruct::shortName() const
{
  return _shortName.c_str();
}

unsigned TypeCodeStruct::getSizeInByteOfAnyReprInSeq() const
{
  unsigned ret=0;
  for(vector< pair<string,TypeCode*> >::const_iterator iter=_members.begin();iter!=_members.end();iter++)
    ret+=(*iter).second->getSizeInByteOfAnyReprInSeq();
  return ret;
}

const TypeCode *TypeCodeStruct::contentType() const throw(Exception)
{
  const char what[]="Content type is specified by giving a key.";
  throw Exception(what);
}

//! Check if this TypeCode is derived from a TypeCode with a given id
/*!
 *   \param id :  a given id
 *   \return     1 if true, 0 if false
 */
int TypeCodeStruct::isA(const char* id) const throw(Exception)
{
  if(_repoId == id)return 1;
  return 0;
}

//! Check if this TypeCode is derived from a given TypeCode
/*!
 *   \param tc : the given TypeCode
 *   \return    1 if true, 0 if false
 */
int TypeCodeStruct::isA(const TypeCode* tc) const 
{
  return isA(tc->id());
}

//! Check if this TypeCode is adaptable to a given TypeCode (tc)
/*!
 *   \param tc : the given TypeCode
 *   \return    1 if true, 0 if false
 */
int TypeCodeStruct::isAdaptable(const TypeCode* tc) const
{
  if(_kind == tc->kind()) return isA(tc->id());
  return 0;
}

void TypeCodeStruct::addMember(const std::string& name,TypeCode* tc)
{
  DEBTRACE(name << " " << tc->name());
  std::vector< std::pair<std::string,TypeCode*> >::const_iterator iter;
  for(iter=_members.begin();iter != _members.end(); iter++)
    {
      if((*iter).first == name)
        throw Exception("Struct member " + name + " already defined");
    }
  _members.push_back(std::pair<std::string,TypeCode*>(name,tc));
}

/*!
 * If name is not an existing key, 0 is returned.
 * \param offset : Out parameter, that specified the location of start of data discriminated by name key.
 */
const TypeCode *TypeCodeStruct::getMember(const std::string& name, unsigned& offset) const
{
  std::vector< std::pair<std::string,TypeCode*> >::const_iterator iter;
  offset=0;
  for(iter=_members.begin();iter != _members.end(); iter++)
    {
      if((*iter).first==name)
        return (*iter).second;
      offset+=(*iter).second->getSizeInByteOfAnyReprInSeq();
    }
  return 0;
}

int TypeCodeStruct::memberCount() const
{
  return _members.size();
}

const char*  TypeCodeStruct::memberName(int index) const
{
  if(index > _members.size())
    {
      stringstream msg;
      msg << "Struct size less than " << index;
      msg << " : " << __FILE__ << ":" << __LINE__;
      throw Exception(msg.str());
    }
  return _members[index].first.c_str();
}

TypeCode*  TypeCodeStruct::memberType(int index) const
{
  if(index > _members.size())
    {
      stringstream msg;
      msg << "Struct size less than " << index;
      msg << " : " << __FILE__ << ":" << __LINE__;
      throw Exception(msg.str());
    }
  return _members[index].second;
}


