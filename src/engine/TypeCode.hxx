#ifndef __TYPECODE_HXX__
#define __TYPECODE_HXX__

#include "RefCounter.hxx"
#include "Exception.hxx"
#include "Any.hxx"

#include <string>
#include <list>

namespace YACS
{
  namespace ENGINE
  {
    class Visitor;

    typedef enum
      {
        NONE     = 0,
        Double   = 1,
        Int      = 2,
        String   = 3,
        Bool     = 4,
        Objref   = 5,
        Sequence = 6,
        Array    = 7,
        Struct   = 8
      } DynType;

//     typedef enum DynType StreamType;

    class TypeCodeObjref;

/*! \brief Base class for all type objects.
 *
 * \ingroup TypeCodes
 *
 * All type objects should be a subclass of TypeCode.  Some type objects,
 * TypeCodeObjref for example, represent one individual type.  Other type
 * objects, such as TypeCodeSeq, are composite types (sequence, here)
 *
 * \see TypeCodeObjref
 * \see TypeCodeSeq
 * \see TypeCodeStruct
 * \see TypeCodeArray
 */
    class TypeCode : public RefCounter
    {
    public:
      TypeCode(DynType kind);

      DynType kind() const;
      const char * getKindRepr() const;
      
      virtual TypeCode *clone() const;
      virtual void putReprAtPlace(char *pt, const char *val, bool deepCpy) const;
      virtual void destroyZippedAny(char *data) const;
      virtual AnyPtr getOrBuildAnyFromZippedData(char *data) const;
      virtual const char * name()       const throw(Exception);
      virtual const char * shortName()  const;
      virtual const char * id()         const throw(Exception);
      virtual const TypeCode * contentType() const throw(Exception);
      virtual int isA(const char* repositoryId) const throw(Exception);
      virtual int isA(const TypeCode* tc) const ;
      virtual int isAdaptable(const TypeCode* tc) const;
      virtual unsigned getSizeInByteOfAnyReprInSeq() const;

      static const char *getKindRepr(DynType kind);
      static TypeCode * interfaceTc(const char* id, const char* name);
      static TypeCode * interfaceTc(const char* id, const char* name, const std::list<TypeCodeObjref *>& ltc);
      static TypeCode * sequenceTc (const char* id, const char* name, TypeCode *content);
      static TypeCode * structTc (const char* id, const char* name);

    protected:
      // --- These operators are placed here to avoid them being used externally
      TypeCode(const TypeCode& tc);
      TypeCode& operator=(const TypeCode& tc);
      virtual ~TypeCode();
    protected:
      DynType _kind;
      static const char *KIND_STR_REPR [];
    };


/*! \brief Class for reference objects.
 *
 * \ingroup TypeCodes
 *
 */
    class TypeCodeObjref: public TypeCode
    {
      friend class Visitor;
    public:
      TypeCodeObjref(const char* repositoryId, const char* name);

      TypeCodeObjref(const char* repositoryId, const char* name, const std::list<TypeCodeObjref *>& ltc);

      TypeCode *clone() const;
      void putReprAtPlace(char *pt, const char *val, bool deepCpy) const;
      void destroyZippedAny(char *data) const;
      AnyPtr getOrBuildAnyFromZippedData(char *data) const;
      const char * id() const   throw(Exception);
      const char * name() const throw(Exception);
      const char * shortName() const;
      int isA(const char* repositoryId) const throw(Exception);
      virtual int isA(const TypeCode* tc) const ;
      virtual int isAdaptable(const TypeCode* tc) const;
    protected:
      ~TypeCodeObjref();
      TypeCodeObjref(const TypeCodeObjref& other);
    private:
      std::string _name;
      std::string _shortName;
      std::string _repoId;
      std::list<TypeCodeObjref *> _listOfBases;
    };


/*! \brief Class for sequence objects.
 *
 * \ingroup TypeCodes
 *
 */
    class TypeCodeSeq: public TypeCode
    {
    public:
      TypeCodeSeq(const char* repositoryId, const char* name, const TypeCode *content);

      TypeCode *clone() const;
      void putReprAtPlace(char *pt, const char *val, bool deepCpy) const;
      void destroyZippedAny(char *data) const;
      AnyPtr getOrBuildAnyFromZippedData(char *data) const;
      const char * id()   const throw(Exception);
      const char * name() const throw(Exception);
      const char * shortName() const;

      virtual const TypeCode * contentType() const throw(Exception);
      virtual int isA(const TypeCode* tc) const ;
      virtual int isAdaptable(const TypeCode* tc) const;
    protected:
      ~TypeCodeSeq();
      TypeCodeSeq(const TypeCodeSeq& tc);
    private:
      std::string _name;
      std::string _shortName;
      std::string _repoId;
      const TypeCode  *_content;
    };

/*! \brief Class for array objects.
 *
 * \ingroup TypeCodes
 *
 */
    class TypeCodeArray : public TypeCode
    {
    public:
      TypeCodeArray(const char* repositoryId, const char* name, const TypeCode *content, unsigned staticLgth);
      TypeCode *clone() const;
      void putReprAtPlace(char *pt, const char *val, bool deepCpy) const;
      void destroyZippedAny(char *data) const;
      AnyPtr getOrBuildAnyFromZippedData(char *data) const;
      const char * id()   const throw(Exception);
      const char * name() const throw(Exception);
      const char * shortName() const;
      unsigned getStaticLgth() const;

      virtual const TypeCode * contentType() const throw(Exception);
      virtual int isA(const TypeCode* tc) const ;
      virtual int isAdaptable(const TypeCode* tc) const;
      unsigned getSizeInByteOfAnyReprInSeq() const;
    protected:
      ~TypeCodeArray();
      TypeCodeArray(const TypeCodeArray& tc);
    private:
      std::string _name;
      std::string _shortName;
      std::string _repoId;
      unsigned _staticLgth;
      const TypeCode  *_content;
    };

/*! \brief Class for struct type.
 *
 * \ingroup TypeCodes
 *
 */
    class TypeCodeStruct : public TypeCode
    {
    public:
      TypeCodeStruct(const char* repositoryId, const char* name);
      TypeCode *clone() const;
      void putReprAtPlace(char *pt, const char *val, bool deepCpy) const;
      void destroyZippedAny(char *data) const;
      AnyPtr getOrBuildAnyFromZippedData(char *data) const;
      const char * id() const   throw(Exception);
      const char * name() const throw(Exception);
      const char * shortName() const;
      virtual int isA(const char* repositoryId) const throw(Exception);
      virtual int isA(const TypeCode* tc) const ;
      virtual int isAdaptable(const TypeCode* tc) const;
      virtual void addMember(const std::string& name,TypeCode* tc);
      int memberCount() const;
      const char*  memberName(int index) const;
      TypeCode*  memberType(int index) const;
    protected:
      ~TypeCodeStruct();
      TypeCodeStruct(const TypeCodeStruct& tc);
    private:
      std::string _name;
      std::string _shortName;
      std::string _repoId;
      std::vector< std::pair<std::string,TypeCode*> > _members;
    };

  }
}
#endif
