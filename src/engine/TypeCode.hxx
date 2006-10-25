#ifndef __TYPECODE_HXX__
#define __TYPECODE_HXX__

//#include <Python.h> // here, to avoid warning: "_POSIX_C_SOURCE" redefined

#include "Exception.hxx"

#include <string>
#include <list>

namespace YACS
{
  namespace ENGINE
  {

    typedef enum
      {
	None     = 0,
	Double   = 1,
	Int      = 2,
	String   = 3,
	Bool     = 4,
	Objref   = 5,
	Sequence = 6
      } DynType;

//     typedef enum DynType StreamType;

    class TypeCode_objref;

    class TypeCode
    {
    public:
      TypeCode(DynType kind);
      virtual ~TypeCode();

      DynType kind() const;

      virtual const char * name()       const throw(Exception);
      virtual const char * id()         const throw(Exception);
      virtual TypeCode * content_type() const throw(Exception);
      virtual int is_a(const char* repositoryId);
      virtual int is_a(TypeCode* tc);

      static TypeCode * interface_tc(const char* id, const char* name);
      static TypeCode * interface_tc(const char* id, const char* name, std::list<TypeCode_objref *> ltc);
      static TypeCode * sequence_tc (const char* id, const char* name, TypeCode *content);

    protected:
      // --- These operators are placed here to avoid them being used externally
      TypeCode(const TypeCode& tc);
      TypeCode& operator=(const TypeCode& tc);
      TypeCode() {}

      DynType _kind;
    };


    class TypeCode_objref: public TypeCode
    {
    public:
      TypeCode_objref(const char* repositoryId, const char* name);

      TypeCode_objref(const char* repositoryId, const char* name, std::list<TypeCode_objref *> ltc);
      virtual ~TypeCode_objref();

      const char * id() const   throw(Exception);
      const char * name() const throw(Exception);
      int is_a(const char* repositoryId) throw(Exception);
      virtual int is_a(TypeCode* tc) throw(Exception);

    private:
      TypeCode_objref() {}

      std::string _name;
      std::string _repoId;
      std::list<TypeCode_objref *> _listOfBases;
    };


    class TypeCode_seq: public TypeCode
    {
    public:
      TypeCode_seq(const char* repositoryId, const char* name, TypeCode *content);
      virtual ~TypeCode_seq();

      const char * id()   const throw(Exception);
      const char * name() const throw(Exception);

      virtual TypeCode * content_type() const throw(Exception);
      virtual int is_a(TypeCode* tc);

    private:
      TypeCode_seq() {}

      std::string _name;
      std::string _repoId;
      TypeCode  * _content;
    };
  }
}
#endif
