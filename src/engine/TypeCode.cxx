
#include "TypeCode.hxx"

using namespace YACS::ENGINE;
using namespace std;

// --- TypeCode

TypeCode::TypeCode(DynType kind)
{
  _kind=kind;
}

TypeCode::~TypeCode()
{
}

DynType TypeCode::kind() const
{
  return _kind;
}

const char * TypeCode::name() const throw(Exception)
{
  throw Exception("No name");
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
    default:
      return "";
    }
}

int TypeCode::is_a(const char* id)
{
  throw Exception("Not implemented for this type");
}

int TypeCode::is_a(TypeCode* tc)
{
  if(_kind == tc->kind()) return 1;
  return 0;
}

TypeCode * TypeCode::content_type() const throw(Exception)
{
  throw Exception("No content type");
};

/**
 * static factory of object reference types
 */

TypeCode * TypeCode::interface_tc(const char* id,
				  const char* name)
{
  return new TypeCode_objref(id, name);
};

TypeCode * TypeCode::interface_tc(const char* id,
				  const char* name,
				  list<TypeCode_objref *> ltc)
{
  return new TypeCode_objref(id, name,ltc);
}


/**
 * static factory of sequence types
 */

TypeCode * TypeCode::sequence_tc(const char* id,
				 const char* name,
				 TypeCode *content)
{
  return new TypeCode_seq(id, name,content);
};


// --- TypeCode_objref


TypeCode_objref::TypeCode_objref(const char* repositoryId, 
				 const char* name)
  : TypeCode(Objref)
{
  _repoId = repositoryId;
  _name = name;
}


TypeCode_objref::~TypeCode_objref()
{
}

const char * TypeCode_objref::id() const throw(Exception)
{
  return _repoId.c_str();
};

const char * TypeCode_objref::name() const throw(Exception)
{
  return _name.c_str();
}

TypeCode_objref::TypeCode_objref(const char* repositoryId,
				 const char* name,
				 list<TypeCode_objref *> ltc)
  : TypeCode(Objref)
{
  _repoId = repositoryId;
  _name = name;
  _listOfBases=ltc;
}

int TypeCode_objref::is_a(const char* id) throw(Exception)
{
  if(_repoId.c_str() == id)return 1;
  list<TypeCode_objref *>::iterator iter;
  for(iter=_listOfBases.begin();iter != _listOfBases.end(); iter++)
    {
      if ((*iter)->is_a(id)) return 1;
    }
  return 0;
}

int TypeCode_objref::is_a(TypeCode* tc) throw(Exception)
{
  return is_a(tc->id());
}

// --- TypeCode_seq


TypeCode_seq::TypeCode_seq(const char* repositoryId,
			   const char* name, 
			   TypeCode *content)
  : TypeCode(Sequence)
{
  _repoId = repositoryId;
  _name = name;
  _content= content;
}

TypeCode_seq::~TypeCode_seq()
{
}

const char * TypeCode_seq::id() const throw(Exception)
{
  return _repoId.c_str();
}

const char * TypeCode_seq::name() const throw(Exception)
{
  return _name.c_str();
}

TypeCode * TypeCode_seq::content_type() const throw(Exception)
{
  return _content;
}

int TypeCode_seq::is_a(TypeCode* tc)
{
  if(_kind == tc->kind())
    {
      if(_content->is_a(tc->content_type())) return 1;
    }
  return 0;
}
