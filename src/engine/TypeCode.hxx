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

#ifndef __TYPECODE_HXX__
#define __TYPECODE_HXX__

#include "YACSlibEngineExport.hxx"
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
    class YACSLIBENGINE_EXPORT TypeCode : public RefCounter
    {
    public:
      TypeCode(DynType kind);

      DynType kind() const;
      const char * getKindRepr() const;
      const TypeCode *subContentType(int lev) const;
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
      virtual int isEquivalent(const TypeCode* tc) const;
      virtual unsigned getSizeInByteOfAnyReprInSeq() const;
      virtual std::string getPrintStr() const;

      static const char *getKindRepr(DynType kind);
      static TypeCode * interfaceTc(const char* id, const char* name);
      static TypeCode * interfaceTc(const char* id, const char* name, const std::list<TypeCodeObjref *>& ltc);
      static TypeCode * sequenceTc (const char* id, const char* name, TypeCode *content);
      static TypeCode * structTc (const char* id, const char* name);

    protected:
      // --- These operators are placed here to avoid them being used externally
      TypeCode(const TypeCode& tc);
      TypeCode& operator=(const TypeCode& tc);
#ifndef SWIG
      virtual ~TypeCode();
#endif
    protected:
      const DynType _kind;
      static const char *KIND_STR_REPR [];
    };

    class YACSLIBENGINE_EXPORT TypeCodeComposed : public TypeCode
    {
    protected:
      TypeCodeComposed(const TypeCodeComposed& other);
      TypeCodeComposed(DynType kind, const char* repositoryId, const char* name);
    protected:
      const std::string _name;
      const std::string _repoId;
      std::string _shortName;
    };


/*! \brief Class for reference objects.
 *
 * \ingroup TypeCodes
 *
 */
    class YACSLIBENGINE_EXPORT TypeCodeObjref : public TypeCodeComposed
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
      virtual int isEquivalent(const TypeCode* tc) const;
    protected:
#ifndef SWIG
      virtual ~TypeCodeObjref();
#endif
      TypeCodeObjref(const TypeCodeObjref& other);
    private:
      std::list<TypeCodeObjref *> _listOfBases;
    };


/*! \brief Class for sequence objects.
 *
 * \ingroup TypeCodes
 *
 */
    class YACSLIBENGINE_EXPORT TypeCodeSeq: public TypeCodeComposed
    {
    public:
      TypeCodeSeq(const char* repositoryId, const char* name, const TypeCode *content);

      TypeCode *clone() const;
      void putReprAtPlace(char *pt, const char *val, bool deepCpy) const;
      void destroyZippedAny(char *data) const;
      virtual unsigned getSizeInByteOfAnyReprInSeq() const;
      AnyPtr getOrBuildAnyFromZippedData(char *data) const;
      const char * id()   const throw(Exception);
      const char * name() const throw(Exception);
      const char * shortName() const;
      virtual std::string getPrintStr() const;

      virtual const TypeCode * contentType() const throw(Exception);
      virtual int isA(const TypeCode* tc) const ;
      virtual int isAdaptable(const TypeCode* tc) const;
      virtual int isEquivalent(const TypeCode* tc) const;
    protected:
#ifndef SWIG
      virtual ~TypeCodeSeq();
#endif
      TypeCodeSeq(const TypeCodeSeq& tc);
    private:
      const TypeCode  *_content;
    };

/*! \brief Class for array objects.
 *
 * \ingroup TypeCodes
 *
 */
    class YACSLIBENGINE_EXPORT TypeCodeArray : public TypeCodeComposed
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
      virtual int isEquivalent(const TypeCode* tc) const;
      unsigned getSizeInByteOfAnyReprInSeq() const;
    protected:
#ifndef SWIG
      virtual ~TypeCodeArray();
#endif
      TypeCodeArray(const TypeCodeArray& tc);
    private:
      const TypeCode  *_content;
      const unsigned _staticLgth;
    };

    class StructAny;

/*! \brief Class for struct type.
 *
 * \ingroup TypeCodes
 *
 */
    class YACSLIBENGINE_EXPORT TypeCodeStruct : public TypeCodeComposed
    {
      friend class StructAny;//Access to _members attribute.
    public:
      TypeCodeStruct(const char* repositoryId, const char* name);
      TypeCode *clone() const;
      void putReprAtPlace(char *pt, const char *val, bool deepCpy) const;
      void destroyZippedAny(char *data) const;
      AnyPtr getOrBuildAnyFromZippedData(char *data) const;
      const char * id() const   throw(Exception);
      const char * name() const throw(Exception);
      const char * shortName() const;
      virtual unsigned getSizeInByteOfAnyReprInSeq() const;
      const TypeCode * contentType() const throw(Exception);
      virtual int isA(const char* repositoryId) const throw(Exception);
      virtual int isA(const TypeCode* tc) const ;
      virtual int isAdaptable(const TypeCode* tc) const;
      virtual int isEquivalent(const TypeCode* tc) const;
      //! The only non const method.
      virtual void addMember(const std::string& name,TypeCode* tc);
      const TypeCode *getMember(const std::string& name, unsigned& offset) const;
      int memberCount() const;
      const char*  memberName(int index) const;
      TypeCode*  memberType(int index) const;
    protected:
#ifndef SWIG
      virtual ~TypeCodeStruct();
#endif
      TypeCodeStruct(const TypeCodeStruct& tc);
    private:
      std::vector< std::pair<std::string,TypeCode*> > _members;
    };

  }
}
#endif
