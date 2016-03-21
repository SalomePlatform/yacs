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

//#define REFCNT
//
#ifdef REFCNT
#define private public
#define protected public
#include <omniORB4/CORBA.h>
#include <omniORB4/internal/typecode.h>
#endif

#include "TypeConversions.hxx"
#include "ConversionException.hxx"
#include "RuntimeSALOME.hxx"
#include "Salome_file_i.hxx"
#include "TypeCode.hxx"
#include "Cstr2d.hxx"
#include "SALOME_GenericObj.hh"

#include <iostream>
#include <iomanip>
#include <sstream>

#ifdef WIN32
#include <fcntl.h>
#define _S_IREAD 256
#define _S_IWRITE 128
int mkstemp(char *tmpl)
{
  int ret=-1;
  mktemp(tmpl); ret=open(tmpl,O_RDWR|O_BINARY|O_CREAT|O_EXCL|_O_SHORT_LIVED, _S_IREAD|_S_IWRITE);
  return ret;
}
#endif

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

namespace YACS
{
  namespace ENGINE
  {
    void printbin(const std::string& bin)
      {
        register char c;
        for(int i=0;i<bin.length();i++)
          {
            c=bin[i];
            if (c < ' ' || c >= 0x7f) 
              {
                fprintf(stderr,"\\x%02x",c & 0xff);
              }
            else
              fprintf(stderr,"%c",c);
          }
        fprintf(stderr,"\n");
      }

    std::string getImplName(ImplType impl)
      {
         switch(impl)
           {
           case CORBAImpl:
             return "CORBA";
           case PYTHONImpl:
             return "PYTHON";
           case NEUTRALImpl:
             return "NEUTRAL";
           case XMLImpl:
             return "XML";
           case CPPImpl:
             return "CPP";
           default:
             return "UNKNOWN";
           }
      }
    /*
     * Functions to return a CORBA TypeCode equivalent to a YACS TypeCode
     */

    typedef CORBA::TypeCode_ptr (*getCorbaTCFn)(const TypeCode *);

    CORBA::TypeCode_ptr getCorbaTCNull(const TypeCode *t)
      {
        stringstream msg;
        msg << "Conversion not implemented: kind= " << t->kind();
        msg << " : " << __FILE__ << ":" << __LINE__;
        throw YACS::ENGINE::ConversionException(msg.str());
      }

    CORBA::TypeCode_ptr getCorbaTCDouble(const TypeCode *t)
    {
      return CORBA::TypeCode::_duplicate(CORBA::_tc_double);
    }

    CORBA::TypeCode_ptr getCorbaTCInt(const TypeCode *t)
    {
      return CORBA::TypeCode::_duplicate(CORBA::_tc_long);
    }

    CORBA::TypeCode_ptr getCorbaTCString(const TypeCode *t)
    {
      return CORBA::TypeCode::_duplicate(CORBA::_tc_string);
    }

    CORBA::TypeCode_ptr getCorbaTCBool(const TypeCode *t)
    {
      return CORBA::TypeCode::_duplicate(CORBA::_tc_boolean);
    }

    CORBA::TypeCode_ptr getCorbaTCObjref(const TypeCode *t)
    {
      DEBTRACE( t->name() << " " << t->shortName() << " " << t->id());
      CORBA::TypeCode_ptr tc;
      if(strncmp(t->id(),"python",6)==0 )
        tc= CORBA::TypeCode::_duplicate(Engines::_tc_fileBlock);
      else if(strncmp(t->id(),"json",4)==0)
        tc= CORBA::TypeCode::_duplicate(CORBA::_tc_string);
      else
        tc= getSALOMERuntime()->getOrb()->create_interface_tc(t->id(),t->shortName());
#ifdef REFCNT
      DEBTRACE("refcount CORBA tc Objref: " << ((omni::TypeCode_base*)tc)->pd_ref_count);
#endif
      return tc;
    }

    CORBA::TypeCode_ptr getCorbaTCSequence(const TypeCode *t)
    {
      CORBA::TypeCode_var content_type=getCorbaTC(t->contentType());
      CORBA::TypeCode_ptr tc= getSALOMERuntime()->getOrb()->create_sequence_tc(0,content_type);
#ifdef REFCNT
      DEBTRACE("refcount CORBA content_type: " << ((omni::TypeCode_base*)content_type.in())->pd_ref_count);
      DEBTRACE("refcount CORBA tc: " << ((omni::TypeCode_base*)tc)->pd_ref_count);
#endif
      return tc;
    }

    CORBA::TypeCode_ptr getCorbaTCStruct(const TypeCode *t)
    {
      CORBA::StructMemberSeq mseq;
      YACS::ENGINE::TypeCodeStruct* tst=(YACS::ENGINE::TypeCodeStruct*)t;
      int nMember=tst->memberCount();
      mseq.length(nMember);
      for(int i=0;i<nMember;i++)
        {
          const char * name=tst->memberName(i);
          TypeCode* tm=tst->memberType(i);
          mseq[i].name=CORBA::string_dup(name);
          mseq[i].type=getCorbaTC(tm);
        }
      CORBA::TypeCode_ptr tc= getSALOMERuntime()->getOrb()->create_struct_tc(t->id(),t->shortName(),mseq);
#ifdef REFCNT
      DEBTRACE("refcount CORBA tc: " << ((omni::TypeCode_base*)tc)->pd_ref_count);
#endif
      return tc;
    }

    getCorbaTCFn getCorbaTCFns[]=
      {
        getCorbaTCNull,
        getCorbaTCDouble,
        getCorbaTCInt,
        getCorbaTCString,
        getCorbaTCBool,
        getCorbaTCObjref,
        getCorbaTCSequence,
        getCorbaTCNull,
        getCorbaTCStruct,
      };

    CORBA::TypeCode_ptr getCorbaTC(const TypeCode *t)
    {
      int tk=t->kind();
      return getCorbaTCFns[tk](t);
    }

    /*
     * End of Functions to return a CORBA TypeCode equivalent to a YACS TypeCode
     */

    /*
     * Section that defines functions to check adaptation from one implementation to another
     * isAdaptable is template function that checks if TypeCode t1 from implementation IMPLIN
     * can be converted to TypeCode t2 from implementation IMPLOUT
     * IMPLIN is the implementation of an output port
     * IMPLOUT is the implementation of an input port
     * If the check is True, the input port can be adapted to the output port
     */

    template <ImplType IMPLIN,ImplType IMPLOUT> inline int isAdaptable(const TypeCode *t1,const TypeCode* t2);

    template <ImplType IMPLIN,ImplType IMPLOUT>
    struct isAdaptableDouble
      {
        static inline int apply(const TypeCode *t1,const TypeCode* t2)
          {
            if(t1->kind() == Double)return 1;
            if(t1->kind() == Int)return 1;
            return 0;
          }
      };
    template <ImplType IMPLIN,ImplType IMPLOUT>
    struct isAdaptableInt
      {
        static inline int apply(const TypeCode *t1,const TypeCode* t2)
          {
            if(t1->kind() == Int)return 1;
            return 0;
          }
      };
    template <ImplType IMPLIN,ImplType IMPLOUT>
    struct isAdaptableString
      {
        static inline int apply(const TypeCode *t1,const TypeCode* t2)
          {
            if(t1->kind() == String)return 1;
            return 0;
          }
      };
    template <ImplType IMPLIN,ImplType IMPLOUT>
    struct isAdaptableBool
      {
        static inline int apply(const TypeCode *t1,const TypeCode* t2)
          {
            if(t1->kind() == Bool)return 1;
            if(t1->kind() == Int)return 1;
            return 0;
          }
      };
    template <ImplType IMPLIN,ImplType IMPLOUT>
    struct isAdaptableObjref
      {
        static inline int apply(const TypeCode *t1,const TypeCode* t2)
          {
            if(t1->kind() == Objref)
              {
                //The inport type must be more general than outport type
                if( t1->isA(t2->id()) )
                  return 1;
              }
            else if(t1->kind() == Sequence)
              {
                const TypeCodeSeq *t1c(dynamic_cast<const TypeCodeSeq *>(t1));
                if(!t1c)
                  return 0;
                const TypeCode *t1cc(t1c->contentType());
                if(t1cc==t2)
                  return 1;
                if(t1cc->kind() == Objref && std::string(t1cc->id())==std::string(t2->id()))
                  return 1;
              }
            return 0;
          }
      };
    template <ImplType IMPLIN,ImplType IMPLOUT>
    struct isAdaptableSequence
      {
        static inline int apply(const TypeCode *t1,const TypeCode* t2)
          {
            if(t1->kind() == Sequence)
              {
                if(isAdaptable<IMPLIN,IMPLOUT>(t1->contentType(),t2->contentType()))
                  {
                    return 1;
                  }
              }
            return 0;
          }
      };
    template <ImplType IMPLIN,ImplType IMPLOUT>
    struct isAdaptableArray
      {
        static inline int apply(const TypeCode *t1,const TypeCode* t2)
          {
            return 0;
          }
      };
    template <ImplType IMPLIN,ImplType IMPLOUT>
    struct isAdaptableStruct
      {
        static inline int apply(const TypeCode *t1,const TypeCode* t2)
          {
            if(t1->kind() == Struct)
              {
                if( t1->isA(t2) )
                  return 1;
              }
            return 0;
          }
      };

    /*
     * Function to check adaptation from implementation 1 (IMPLIN,t1) to implementation 2 (IMPLOUT,t2)
     * t1 is the IMPLIN output port type
     * t2 is the IMPLOUT input port type
     */
    template <ImplType IMPLIN,ImplType IMPLOUT>
    inline int isAdaptable(const TypeCode *t1,const TypeCode* t2)
      {
         switch(t2->kind())
           {
           case Double:
             return isAdaptableDouble<IMPLIN,IMPLOUT>::apply(t1,t2);
           case Int:
             return isAdaptableInt<IMPLIN,IMPLOUT>::apply(t1,t2);
           case String:
             return isAdaptableString<IMPLIN,IMPLOUT>::apply(t1,t2);
           case Bool:
             return isAdaptableBool<IMPLIN,IMPLOUT>::apply(t1,t2);
           case Objref:
             return isAdaptableObjref<IMPLIN,IMPLOUT>::apply(t1,t2);
           case Sequence:
             return isAdaptableSequence<IMPLIN,IMPLOUT>::apply(t1,t2);
           case Array:
             return isAdaptableArray<IMPLIN,IMPLOUT>::apply(t1,t2);
           case Struct:
             return isAdaptableStruct<IMPLIN,IMPLOUT>::apply(t1,t2);
           default:
             break;
           }
         return 0;
      }

    //xxx to Python adaptations
    int isAdaptableCorbaPyObject(const TypeCode *t1,const TypeCode *t2)
    {
      return isAdaptable<PYTHONImpl,CORBAImpl>(t1,t2);
    }
    int isAdaptableNeutralPyObject(const TypeCode * t1, const TypeCode * t2)
    {
      return isAdaptable<PYTHONImpl,NEUTRALImpl>(t1,t2);
    }
    int isAdaptablePyObjectPyObject(const TypeCode *t1,const TypeCode *t2)
    {
      return isAdaptable<PYTHONImpl,PYTHONImpl>(t1,t2);
    }

    //xxx to Neutral adaptations
    int isAdaptableCorbaNeutral(const TypeCode *t1,const TypeCode *t2)
    {
      return isAdaptable<NEUTRALImpl,CORBAImpl>(t1,t2);
    }
    int isAdaptablePyObjectNeutral(const TypeCode *t1,const TypeCode *t2)
    {
      return isAdaptable<NEUTRALImpl,PYTHONImpl>(t1,t2);
    }
    int isAdaptableXmlNeutral(const TypeCode *t1,const TypeCode *t2)
    {
      return isAdaptable<NEUTRALImpl,XMLImpl>(t1,t2);
    }
    int isAdaptableNeutralNeutral(const TypeCode *t1, const TypeCode *t2)
    {
      return isAdaptableNeutralCorba(t1, t2);
    }

    //xxx to XML adaptations
    int isAdaptableNeutralXml(const TypeCode * t1, const TypeCode * t2)
    {
      return isAdaptable<XMLImpl,NEUTRALImpl>(t1,t2);
    }

    //xxx to Corba adaptations
    int isAdaptableNeutralCorba(const TypeCode *t1,const TypeCode *t2)
    {
      return isAdaptable<CORBAImpl,NEUTRALImpl>(t1,t2);
    }
    int isAdaptableXmlCorba(const TypeCode *t1,const TypeCode *t2)
    {
      return isAdaptable<CORBAImpl,XMLImpl>(t1,t2);
    }
    int isAdaptableCorbaCorba(const TypeCode *t1,const TypeCode *t2)
    {
      return isAdaptable<CORBAImpl,CORBAImpl>(t1,t2);
    }
    int isAdaptablePyObjectCorba(const TypeCode *t1,const TypeCode *t2)
    {
      return isAdaptable<CORBAImpl,PYTHONImpl>(t1,t2);
    }

    //! Basic template convertor from type TIN to Yacs<TOUT> type
    /*!
     * This convertor does nothing : throws exception
     * It must be partially specialize for a specific type (TIN)
     */
    template <ImplType IMPLIN,class TIN,class TIN2,ImplType IMPLOUT, class TOUT>
    struct convertToYacsDouble
    {
      static inline double convert(const TypeCode *t,TIN o,TIN2 aux)
        {
          stringstream msg;
          msg << "Conversion not implemented: kind= " << t->kind() << " Implementation: " << IMPLIN << " to: " << IMPLOUT;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLIN,class TIN,class TIN2,ImplType IMPLOUT, class TOUT>
    struct convertToYacsInt
    {
      static inline long convert(const TypeCode *t,TIN o,TIN2 aux)
        {
          stringstream msg;
          msg << "Conversion not implemented: kind= " << t->kind() << " Implementation: " << IMPLIN << " to: " << IMPLOUT;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLIN,class TIN,class TIN2,ImplType IMPLOUT, class TOUT>
    struct convertToYacsString
    {
      static inline std::string convert(const TypeCode *t,TIN o,TIN2 aux)
        {
          stringstream msg;
          msg << "Conversion not implemented: kind= " << t->kind() << " Implementation: " << IMPLIN << " to: " << IMPLOUT;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLIN,class TIN,class TIN2,ImplType IMPLOUT, class TOUT>
    struct convertToYacsBool
    {
      static inline bool convert(const TypeCode *t,TIN o,TIN2 aux)
        {
          stringstream msg;
          msg << "Conversion not implemented: kind= " << t->kind() << " Implementation: " << IMPLIN << " to: " << IMPLOUT;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLIN,class TIN,class TIN2,ImplType IMPLOUT, class TOUT>
    struct convertToYacsObjref
    {
      static inline std::string convert(const TypeCode *t,TIN o,TIN2 aux,int protocol)
        {
          stringstream msg;
          msg << "Conversion not implemented: kind= " << t->kind() << " Implementation: " << IMPLIN << " to: " << IMPLOUT;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLIN,class TIN,class TIN2,ImplType IMPLOUT, class TOUT>
    struct convertToYacsSequence
    {
      static inline void convert(const TypeCode *t,TIN o,TIN2 aux,std::vector<TOUT>& v)
        {
          stringstream msg;
          msg << "Conversion not implemented: kind= " << t->kind() << " Implementation: " << IMPLIN << " to: " << IMPLOUT;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLIN,class TIN,class TIN2,ImplType IMPLOUT, class TOUT>
    struct convertToYacsArray
    {
      static inline void convert(const TypeCode *t,TIN o,TIN2 aux,std::vector<TOUT>& v)
        {
          stringstream msg;
          msg << "Conversion not implemented: kind= " << t->kind() << " Implementation: " << IMPLIN << " to: " << IMPLOUT;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLIN,class TIN,class TIN2,ImplType IMPLOUT, class TOUT>
    struct convertToYacsStruct
    {
      static inline void convert(const TypeCode *t,TIN o,TIN2 aux,std::map<std::string,TOUT>& v)
        {
          stringstream msg;
          msg << "Conversion not implemented: kind= " << t->kind() << " Implementation: " << IMPLIN << " to: " << IMPLOUT;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };

    //! Basic convertor from Yacs<TOUT> type to full TOUT type
    /*!
     *
     */
    template <ImplType IMPLOUT, class TOUT>
    struct convertFromYacsDouble
    {
      static inline TOUT convert(const TypeCode *t,double o)
        {
          stringstream msg;
          msg << "Conversion not implemented: kind= " << t->kind() << " Implementation: " << IMPLOUT;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertFromYacsInt
    {
      static inline TOUT convert(const TypeCode *t,long o)
        {
          stringstream msg;
          msg << "Conversion not implemented: kind= " << t->kind() << " Implementation: " << IMPLOUT;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertFromYacsString
    {
      static inline TOUT convert(const TypeCode *t,std::string o)
        {
          stringstream msg;
          msg << "Conversion not implemented: kind= " << t->kind() << " Implementation: " << IMPLOUT;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertFromYacsBool
    {
      static inline TOUT convert(const TypeCode *t,bool o)
        {
          stringstream msg;
          msg << "Conversion not implemented: kind= " << t->kind() << " Implementation: " << IMPLOUT;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertFromYacsObjref
    {
      static inline TOUT convert(const TypeCode *t,std::string o)
        {
          stringstream msg;
          msg << "Conversion not implemented: kind= " << t->kind() << " Implementation: " << IMPLOUT;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertFromYacsSequence
    {
      static inline TOUT convert(const TypeCode *t,std::vector<TOUT>& v)
        {
          stringstream msg;
          msg << "Conversion not implemented: kind= " << t->kind() << " Implementation: " << IMPLOUT;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertFromYacsArray
    {
      static inline TOUT convert(const TypeCode *t,std::vector<TOUT>& v)
        {
          stringstream msg;
          msg << "Conversion not implemented: kind= " << t->kind() << " Implementation: " << IMPLOUT;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertFromYacsStruct
    {
      static inline TOUT convert(const TypeCode *t,std::map<std::string,TOUT>& v)
        {
          stringstream msg;
          msg << "Conversion not implemented: kind= " << t->kind() << " Implementation: " << IMPLOUT;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLIN,class TIN,class TIN2,ImplType IMPLOUT, class TOUT>
    inline TOUT convertDouble(const TypeCode *t,TIN o,TIN2 aux)
    {
      double d=convertToYacsDouble<IMPLIN,TIN,TIN2,IMPLOUT,TOUT>::convert(t,o,aux);
      DEBTRACE( d );
      TOUT r=convertFromYacsDouble<IMPLOUT,TOUT>::convert(t,d);
      return r;
    }
    template <ImplType IMPLIN,class TIN,class TIN2,ImplType IMPLOUT, class TOUT>
    inline TOUT convertInt(const TypeCode *t,TIN o,TIN2 aux)
    {
      long d=convertToYacsInt<IMPLIN,TIN,TIN2,IMPLOUT,TOUT>::convert(t,o,aux);
      DEBTRACE( d );
      TOUT r=convertFromYacsInt<IMPLOUT,TOUT>::convert(t,d);
      return r;
    }
    template <ImplType IMPLIN,class TIN,class TIN2,ImplType IMPLOUT, class TOUT>
    inline TOUT convertString(const TypeCode *t,TIN o,TIN2 aux)
    {
      std::string d=convertToYacsString<IMPLIN,TIN,TIN2,IMPLOUT,TOUT>::convert(t,o,aux);
      DEBTRACE( d );
      TOUT r=convertFromYacsString<IMPLOUT,TOUT>::convert(t,d);
      return r;
    }
    template <ImplType IMPLIN,class TIN,class TIN2,ImplType IMPLOUT, class TOUT>
    inline TOUT convertBool(const TypeCode *t,TIN o,TIN2 aux)
    {
      double d=convertToYacsBool<IMPLIN,TIN,TIN2,IMPLOUT,TOUT>::convert(t,o,aux);
      DEBTRACE( d );
      TOUT r=convertFromYacsBool<IMPLOUT,TOUT>::convert(t,d);
      return r;
    }
    template <ImplType IMPLIN,class TIN,class TIN2,ImplType IMPLOUT, class TOUT>
    inline TOUT convertObjref(const TypeCode *t,TIN o,TIN2 aux)
    {
      int protocol=-1;
      if(IMPLOUT==XMLImpl || IMPLOUT==NEUTRALImpl)
        protocol=0;
      std::string d=convertToYacsObjref<IMPLIN,TIN,TIN2,IMPLOUT,TOUT>::convert(t,o,aux,protocol);
      DEBTRACE( d );
      TOUT r=convertFromYacsObjref<IMPLOUT,TOUT>::convert(t,d);
      return r;
    }

    template <ImplType IMPLIN,class TIN,class TIN2,ImplType IMPLOUT, class TOUT>
    inline TOUT convertSequence(const TypeCode *t,TIN o,TIN2 aux)
    {
      std::vector<TOUT> v;
      convertToYacsSequence<IMPLIN,TIN,TIN2,IMPLOUT,TOUT>::convert(t,o,aux,v);
      TOUT r=convertFromYacsSequence<IMPLOUT,TOUT>::convert(t,v);
      return r;
    }
    template <ImplType IMPLIN,class TIN,class TIN2,ImplType IMPLOUT, class TOUT>
    inline TOUT convertArray(const TypeCode *t,TIN o,TIN2 aux)
    {
      std::vector<TOUT> v;
      convertToYacsArray<IMPLIN,TIN,TIN2,IMPLOUT,TOUT>::convert(t,o,aux,v);
      TOUT r=convertFromYacsArray<IMPLOUT,TOUT>::convert(t,v);
      return r;
    }
    template <ImplType IMPLIN,class TIN,class TIN2,ImplType IMPLOUT, class TOUT>
    inline TOUT convertStruct(const TypeCode *t,TIN o,TIN2 aux)
    {
      std::map<std::string,TOUT> v;
      convertToYacsStruct<IMPLIN,TIN,TIN2,IMPLOUT,TOUT>::convert(t,o,aux,v);
      TOUT r=convertFromYacsStruct<IMPLOUT,TOUT>::convert(t,v);
      return r;
    }

    template <ImplType IMPLIN,class TIN,class TIN2,ImplType IMPLOUT, class TOUT>
    inline TOUT YacsConvertor(const TypeCode *t,TIN o,TIN2 aux)
      {
         int tk=t->kind();
         switch(t->kind())
           {
           case Double:
             return convertDouble<IMPLIN,TIN,TIN2,IMPLOUT,TOUT>(t,o,aux);
           case Int:
             return convertInt<IMPLIN,TIN,TIN2,IMPLOUT,TOUT>(t,o,aux);
           case String:
             return convertString<IMPLIN,TIN,TIN2,IMPLOUT,TOUT>(t,o,aux);
           case Bool:
             return convertBool<IMPLIN,TIN,TIN2,IMPLOUT,TOUT>(t,o,aux);
           case Objref:
             return convertObjref<IMPLIN,TIN,TIN2,IMPLOUT,TOUT>(t,o,aux);
           case Sequence:
             return convertSequence<IMPLIN,TIN,TIN2,IMPLOUT,TOUT>(t,o,aux);
           case Array:
             return convertArray<IMPLIN,TIN,TIN2,IMPLOUT,TOUT>(t,o,aux);
           case Struct:
             return convertStruct<IMPLIN,TIN,TIN2,IMPLOUT,TOUT>(t,o,aux);
           default:
             break;
           }
         stringstream msg;
         msg << "Conversion not implemented: kind= " << tk << " Implementation: " << IMPLOUT;
         msg << " : " << __FILE__ << ":" << __LINE__;
         throw YACS::ENGINE::ConversionException(msg.str());
      }

    //! ToYacs Convertor for PYTHONImpl
    /*!
     * This convertor converts Python object to YACS<TOUT> types
     * Partial specialization for Python implementation with type PyObject* (PYTHONImpl)
     */
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsDouble<PYTHONImpl,PyObject*,void*,IMPLOUT,TOUT>
    {
      static inline double convert(const TypeCode *t,PyObject* o,void*)
        {
          double x;
          if (PyFloat_Check(o))
            x=PyFloat_AS_DOUBLE(o);
          else if (PyInt_Check(o))
            x=PyInt_AS_LONG(o);
          else if(PyLong_Check(o))
            x=PyLong_AsLong(o);
          else
            {
              stringstream msg;
              msg << "Not a python double. ";
#ifdef _DEVDEBUG_
              msg << "kind=" << t->kind() ;
              msg << " ( " << __FILE__ << ":" << __LINE__ << ")";
#endif
              throw YACS::ENGINE::ConversionException(msg.str());
            }
          return x;
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsInt<PYTHONImpl,PyObject*,void*,IMPLOUT,TOUT>
    {
      static inline long convert(const TypeCode *t,PyObject* o,void*)
        {
          long l;
          if (PyInt_Check(o))
            l=PyInt_AS_LONG(o);
          else if(PyLong_Check(o))
            l=PyLong_AsLong(o);
          else
            {
              stringstream msg;
              msg << "Not a python integer. ";
#ifdef _DEVDEBUG_
              msg << "kind=" << t->kind() ;
              msg << " ( " << __FILE__ << ":" << __LINE__ << ")";
#endif
              throw YACS::ENGINE::ConversionException(msg.str());
            }
          return l;
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsString<PYTHONImpl,PyObject*,void*,IMPLOUT,TOUT>
    {
      static inline std::string convert(const TypeCode *t,PyObject* o,void*)
        {
          std::string s;
          if (PyString_Check(o))
            s= PyString_AS_STRING(o);
          else
            {
              stringstream msg;
              msg << "Not a python string. ";
#ifdef _DEVDEBUG_
              msg << "kind=" << t->kind() ;
              msg << " ( " << __FILE__ << ":" << __LINE__ << ")";
#endif
              throw YACS::ENGINE::ConversionException(msg.str());
            }
          return s;
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsBool<PYTHONImpl,PyObject*,void*,IMPLOUT,TOUT>
    {
      static inline bool convert(const TypeCode *t,PyObject* o,void*)
        {
          bool l;
          if (PyBool_Check(o))
              l=(o==Py_True);
          else if (PyInt_Check(o))
              l=(PyInt_AS_LONG(o)!=0);
          else if(PyLong_Check(o))
              l=(PyLong_AsLong(o)!=0);
          else
            {
              stringstream msg;
              msg << "Not a python boolean. ";
#ifdef _DEVDEBUG_
              msg << "kind=" << t->kind() ;
              msg << " ( " << __FILE__ << ":" << __LINE__ << ")";
#endif
              throw YACS::ENGINE::ConversionException(msg.str());
            }
          return l;
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsObjref<PYTHONImpl,PyObject*,void*,IMPLOUT,TOUT>
    {
      static inline std::string convert(const TypeCode *t,PyObject* o,void*,int protocol)
        {
          if (PyString_Check(o) && strncmp(t->id(),"python",6)!=0)
            {
              // the objref is used by Python as a string (prefix:value) keep it as a string
              return PyString_AS_STRING(o);
            }
          if(strncmp(t->id(),"python",6)==0)
            {
              // It's a native Python object pickle it
              PyObject* mod=PyImport_ImportModule("cPickle");
              PyObject *pickled=PyObject_CallMethod(mod,(char *)"dumps",(char *)"Oi",o,protocol);
              DEBTRACE(PyObject_REPR(pickled) );
              Py_DECREF(mod);
              if(pickled==NULL)
                {
                  PyErr_Print();
                  throw YACS::ENGINE::ConversionException("Problem in convertToYacsObjref<PYTHONImpl");
                }
              std::string mystr(PyString_AsString(pickled),PyString_Size(pickled));
              Py_DECREF(pickled);
              return mystr;
            }
          else if(strncmp(t->id(),"json",4)==0)
            {
              // It's a Python  object convert it to json 
              PyObject* mod=PyImport_ImportModule("simplejson");
              if(mod==NULL)
                {
                  PyErr_Print();
                  throw YACS::ENGINE::ConversionException("Problem in convertToYacsObjref<PYTHONImpl: no simplejson module");
                }
              PyObject *pickled=PyObject_CallMethod(mod,(char *)"dumps",(char *)"O",o);
              Py_DECREF(mod);
              if(pickled==NULL)
                {
                  PyErr_Print();
                  throw YACS::ENGINE::ConversionException("Problem in convertToYacsObjref<PYTHONImpl");
                }
              std::string mystr=PyString_AsString(pickled);
              Py_DECREF(pickled);
              return mystr;
            }
          else
            {
              // It should be a CORBA Object convert it to an IOR string
              PyObject *pystring=PyObject_CallMethod(getSALOMERuntime()->getPyOrb(),(char *)"object_to_string",(char *)"O",o);
              if(pystring==NULL)
                {
                  PyErr_Print();
                  throw YACS::ENGINE::ConversionException("Problem in convertToYacsObjref<PYTHONImpl");
                }
              std::string mystr=PyString_AsString(pystring);
              Py_DECREF(pystring);
              return mystr;
            }
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsSequence<PYTHONImpl,PyObject*,void*,IMPLOUT,TOUT>
    {
      static inline void convert(const TypeCode *t,PyObject* o,void*,std::vector<TOUT>& v)
        {
          if(!PySequence_Check(o))
            {
              stringstream msg;
              msg << "Problem in conversion: the python object is not a sequence " << std::endl;
#ifdef _DEVDEBUG_
              msg << " ( " << __FILE__ << ":" << __LINE__ << ")";
#endif
              throw YACS::ENGINE::ConversionException(msg.str());
            }
          int length=PySequence_Size(o);
          DEBTRACE("length: " << length );
          v.resize(length);
          for(int i=0;i<length;i++)
            {
              PyObject *item=PySequence_ITEM(o,i);
#ifdef _DEVDEBUG_
              std::cerr <<"item[" << i << "]=";
              PyObject_Print(item,stderr,Py_PRINT_RAW);
              std::cerr << std::endl;
#endif
              DEBTRACE( "item refcnt: " << item->ob_refcnt );
              try
                {
                  TOUT ro=YacsConvertor<PYTHONImpl,PyObject*,void*,IMPLOUT,TOUT>(t->contentType(),item,0);
                  v[i]=ro;
                  Py_DECREF(item);
                }
              catch(ConversionException& ex)
                {
                  stringstream msg;
                  msg << ex.what() << " for sequence element " << i;
                  throw YACS::ENGINE::ConversionException(msg.str(),false);
                }
            }
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsStruct<PYTHONImpl,PyObject*,void*,IMPLOUT,TOUT>
    {
      static inline void convert(const TypeCode *t,PyObject* o,void*,std::map<std::string,TOUT>& m)
        {
          DEBTRACE( "o refcnt: " << o->ob_refcnt );
          PyObject *key, *value;
          YACS::ENGINE::TypeCodeStruct* tst=(YACS::ENGINE::TypeCodeStruct*)t;
          int nMember=tst->memberCount();
          DEBTRACE("nMember="<<nMember);
          for(int i=0;i<nMember;i++)
            {
              std::string name=tst->memberName(i);
              DEBTRACE("Member name="<<name);
              TypeCode* tm=tst->memberType(i);
              value=PyDict_GetItemString(o, name.c_str());
              if(value==NULL)
                {
                  //member name not present
                  //TODO delete all allocated objects in m
#ifdef _DEVDEBUG_
                  PyObject_Print(o,stderr,Py_PRINT_RAW);
                  std::cerr << std::endl;
#endif
                  stringstream msg;
                  msg << "member " << name << " not present " ;
                  throw YACS::ENGINE::ConversionException(msg.str());
                }
              DEBTRACE( "value refcnt: " << value->ob_refcnt );
              try
                {
                  TOUT ro=YacsConvertor<PYTHONImpl,PyObject*,void*,IMPLOUT,TOUT>(tm,value,0);
                  m[name]=ro;
                }
              catch(ConversionException& ex)
                {
                  std::string s=" for struct member "+name;
                  throw YACS::ENGINE::ConversionException(ex.what()+s,false);
                }
            }
        }
    };
    /* End of ToYacs Convertor for PYTHONImpl */

    //! FromYacs Convertor for PYTHONImpl
    /*!
     * Convert YACS<PyObject*> intermediate types to PyObject* types (PYTHONImpl)
     */
    template <>
    struct convertFromYacsDouble<PYTHONImpl,PyObject*>
    {
      static inline PyObject* convert(const TypeCode *t,double o)
        {
          PyObject *pyob=PyFloat_FromDouble(o);
          return pyob;
        }
    };
    template <>
    struct convertFromYacsInt<PYTHONImpl,PyObject*>
    {
      static inline PyObject* convert(const TypeCode *t,long o)
        {
          PyObject *pyob=PyLong_FromLong(o);
          return pyob;
        }
    };
    template <>
    struct convertFromYacsString<PYTHONImpl,PyObject*>
    {
      static inline PyObject* convert(const TypeCode *t,std::string& o)
        {
          return PyString_FromString(o.c_str());
        }
    };
    template <>
    struct convertFromYacsBool<PYTHONImpl,PyObject*>
    {
      static inline PyObject* convert(const TypeCode *t,bool o)
        {
          return PyBool_FromLong ((long)o);
        }
    };
    template <>
    struct convertFromYacsObjref<PYTHONImpl,PyObject*>
    {
      static inline PyObject* convert(const TypeCode *t,std::string& o)
        {
          if(o=="")
            {
              Py_INCREF(Py_None);
              return Py_None;
            }
          if(t->isA(Runtime::_tc_file))
            {
              //It's an objref file. Convert it specially
              return PyString_FromString(o.c_str());
            }
          if(strncmp(t->id(),"python",6)==0)
            {
              //It's a python pickled object, unpickled it
              PyObject* mod=PyImport_ImportModule("cPickle");
              PyObject *ob=PyObject_CallMethod(mod,(char *)"loads",(char *)"s#",o.c_str(),o.length());
              DEBTRACE(PyObject_REPR(ob));
              Py_DECREF(mod);
              if(ob==NULL)
                {
                  PyErr_Print();
                  throw YACS::ENGINE::ConversionException("Problem in convertFromYacsObjref<PYTHONImpl");
                }
              return ob;
            }
          if(strncmp(t->id(),"json",4)==0)
            {
              // It's a json object unpack it
              PyObject* mod=PyImport_ImportModule("simplejson");
              if(mod==NULL)
                {
                  PyErr_Print();
                  throw YACS::ENGINE::ConversionException("Problem in convertToYacsObjref<PYTHONImpl: no simplejson module");
                }
              PyObject *ob=PyObject_CallMethod(mod,(char *)"loads",(char *)"s",o.c_str());
              Py_DECREF(mod);
              if(ob==NULL)
                {
                  PyErr_Print();
                  throw YACS::ENGINE::ConversionException("Problem in convertFromYacsObjref<PYTHONImpl");
                }
              return ob;
            }

          /* another way to convert IOR string to CORBA PyObject 
          PyObject* ob= PyObject_CallMethod(getSALOMERuntime()->getPyOrb(),"string_to_object","s",o.c_str());
          DEBTRACE( "Objref python refcnt: " << ob->ob_refcnt );
          return ob;
          */

          //Objref CORBA. prefix=IOR,corbaname,corbaloc
          CORBA::Object_var obref;
          try
            {
              obref = getSALOMERuntime()->getOrb()->string_to_object(o.c_str());
#ifdef REFCNT
              DEBTRACE("obref refCount: " << obref->_PR_getobj()->pd_refCount);
#endif
            }
          catch(CORBA::Exception& ex) 
            {
              DEBTRACE( "Can't get reference to object." );
              throw ConversionException("Can't get reference to object");
            }

          if(obref->_non_existent())
            {
              throw ConversionException("non_existent object");
            }

          if( CORBA::is_nil(obref) )
            {
              DEBTRACE( "Can't get reference to object (or it was nil)." );
              throw ConversionException("Can't get reference to object");
            }

          if(!obref->_is_a(t->id()))
            {
              stringstream msg;
              msg << "Problem in conversion: an objref " << t->id() << " is expected " << endl;
              msg << "An objref of type " << obref->_PD_repoId << " is given " << endl;
              msg << " (" << __FILE__ << ":" << __LINE__ << ")";
              throw YACS::ENGINE::ConversionException(msg.str());
            }
#ifdef REFCNT
          DEBTRACE("obref refCount: " << obref->_PR_getobj()->pd_refCount);
#endif
#ifdef _DEVDEBUG_
          std::cerr << "_PD_repoId: " << obref->_PD_repoId << std::endl;
          std::cerr << "_mostDerivedRepoId: " << obref->_PR_getobj()->_mostDerivedRepoId()  << std::endl;
#endif

          //hold_lock is true: caller is supposed to hold the GIL.
          //omniorb will not take the GIL
          PyObject* ob= getSALOMERuntime()->getApi()->cxxObjRefToPyObjRef(obref, 1);

#ifdef _DEVDEBUG_
          PyObject_Print(ob,stderr,Py_PRINT_RAW);
          std::cerr << std::endl;
          std::cerr << "obref is a generic: " << obref->_is_a("IDL:SALOME/GenericObj:1.0") << std::endl;
          PyObject_Print(getSALOMERuntime()->get_omnipy(),stderr,Py_PRINT_RAW);
          std::cerr << std::endl;
#endif

          //ob is a CORBA::Object. Try to convert it to more specific type SALOME/GenericObj
          if(obref->_is_a("IDL:SALOME/GenericObj:1.0"))
            {
              PyObject *result = PyObject_CallMethod(getSALOMERuntime()->get_omnipy(), (char *)"narrow", (char *)"Osi",ob,"IDL:SALOME/GenericObj:1.0",1);
              if(result==NULL)
                PyErr_Clear();//Exception during narrow. Keep ob
              else if(result==Py_None)
                Py_DECREF(result); //Can't narrow. Keep ob
              else
                {
                  //Can narrow. Keep result
#ifdef _DEVDEBUG_
                  PyObject_Print(result,stderr,Py_PRINT_RAW);
                  std::cerr << std::endl;
#endif
                  Py_DECREF(ob);
                  ob=result;
                }
            }

#ifdef REFCNT
          DEBTRACE("obref refCount: " << obref->_PR_getobj()->pd_refCount);
#endif
          return ob;
        }
    };

    template <>
    struct convertFromYacsSequence<PYTHONImpl,PyObject*>
    {
      static inline PyObject* convert(const TypeCode *t,std::vector<PyObject*>& v)
        {
          std::vector<PyObject*>::const_iterator iter;
          PyObject *pyob = PyList_New(v.size());
          int i=0;
          for(iter=v.begin();iter!=v.end();iter++)
            {
              PyObject* item=*iter;
              DEBTRACE( "item refcnt: " << item->ob_refcnt );
              PyList_SetItem(pyob,i,item);
              DEBTRACE( "item refcnt: " << item->ob_refcnt );
              i++;
            }
          return pyob;
        }
    };
    template <>
    struct convertFromYacsStruct<PYTHONImpl,PyObject*>
    {
      static inline PyObject* convert(const TypeCode *t,std::map<std::string,PyObject*>& m)
        {
          PyObject *pyob = PyDict_New();
          std::map<std::string, PyObject*>::const_iterator pt;
          for(pt=m.begin();pt!=m.end();pt++)
            {
              std::string name=(*pt).first;
              PyObject* item=(*pt).second;
              DEBTRACE( "item refcnt: " << item->ob_refcnt );
              PyDict_SetItemString(pyob,name.c_str(),item);
              Py_DECREF(item);
              DEBTRACE( "item refcnt: " << item->ob_refcnt );
            }
          DEBTRACE( "pyob refcnt: " << pyob->ob_refcnt );
          return pyob;
        }
    };
    /* End of FromYacs Convertor for PYTHONImpl */

    //! ToYacs Convertor for XMLImpl
    /*!
     * Partial specialization for XML implementation (XMLImpl)
     * This convertor converts xml object to YACS<TOUT> types
     */
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsDouble<XMLImpl,xmlDocPtr,xmlNodePtr,IMPLOUT,TOUT>
    {
      static inline double convert(const TypeCode *t,xmlDocPtr doc,xmlNodePtr cur)
        {
          double d=0;
          cur = cur->xmlChildrenNode;
          while (cur != NULL)
            {
              if ((!xmlStrcmp(cur->name, (const xmlChar *)"double")))
                {
                  //wait a double, got a double
                  xmlChar * s = NULL;
                  s = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                  if (s)
                    {
                      DEBTRACE( "convertToYacsDouble " << (const char *)s );
                      d=Cstr2d((const char *)s);
                      xmlFree(s);
                    }
                  else
                    {
                      DEBTRACE("############### workaround to improve...");
                    }
                  return d;
                }
              else if ((!xmlStrcmp(cur->name, (const xmlChar *)"int")))
                {
                  //wait a double, got an int
                  xmlChar * s = NULL;
                  s = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                  if (s)
                    {
                      DEBTRACE( "convertToYacsDouble " << (const char *)s );
                      d=Cstr2d((const char *)s);
                      xmlFree(s);
                    }
                  else
                    {
                      DEBTRACE("############### workaround to improve...");
                    }
                  return d;
                }
              cur = cur->next;
            }
          stringstream msg;
          msg << "Problem in conversion from Xml to " << getImplName(IMPLOUT) << " with type:  " << t->id() ;
          msg << " (" << __FILE__ << ":" << __LINE__ << ")";
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsInt<XMLImpl,xmlDocPtr,xmlNodePtr,IMPLOUT,TOUT>
    {
      static inline long convert(const TypeCode *t,xmlDocPtr doc,xmlNodePtr cur)
        {
          long d=0;
          cur = cur->xmlChildrenNode;
          while (cur != NULL)
            {
              if ((!xmlStrcmp(cur->name, (const xmlChar *)"int")))
                {
                  xmlChar * s = NULL;
                  s = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                  if (s)
                    {
                      DEBTRACE( "convertToYacsInt " << (const char *)s );
                      d=atol((const char *)s);
                      xmlFree(s);
                    }
                  else
                    {
                      DEBTRACE("############### workaround to improve...");
                    }
                  return d;
                }
              cur = cur->next;
            }
          stringstream msg;
          msg << "Problem in conversion from Xml to " << getImplName(IMPLOUT) << " with type:  " << t->id() ;
          msg << " (" << __FILE__ << ":" << __LINE__ << ")";
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsString<XMLImpl,xmlDocPtr,xmlNodePtr,IMPLOUT,TOUT>
    {
      static inline std::string convert(const TypeCode *t,xmlDocPtr doc,xmlNodePtr cur)
        {
          cur = cur->xmlChildrenNode;
          while (cur != NULL)
            {
              if ((!xmlStrcmp(cur->name, (const xmlChar *)"string")))
                {
                  //wait a string, got a string
                  xmlChar * s = NULL;
                  s = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                  if(s==0)return "";
                  DEBTRACE( "convertToYacsString " << (const char *)s );
                  std::string mystr=std::string((const char *)s);
                  xmlFree(s);
                  return mystr;
                }
              cur = cur->next;
            }
          stringstream msg;
          msg << "Problem in conversion from Xml to " << getImplName(IMPLOUT) << " with type:  " << t->id() ;
          msg << " (" << __FILE__ << ":" << __LINE__ << ")";
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsBool<XMLImpl,xmlDocPtr,xmlNodePtr,IMPLOUT,TOUT>
    {
      static inline bool convert(const TypeCode *t,xmlDocPtr doc,xmlNodePtr cur)
        {
          cur = cur->xmlChildrenNode;
          while (cur != NULL)
            {
              if ((!xmlStrcmp(cur->name, (const xmlChar *)"boolean")))
                {
                  //wait a boolean, got a boolean
                  xmlChar * s = NULL;
                  s = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                  bool ob =false;
                  if (s)
                    {
                      DEBTRACE( "convertToYacsBool " << (const char *)s );
                      ob=atoi((const char*)s)!=0;
                      xmlFree(s);
                    }
                  else
                    {
                      DEBTRACE("############### workaround to improve...");
                    }
                  return ob;
                }
              cur = cur->next;
            }
          stringstream msg;
          msg << "Problem in conversion from Xml to " << getImplName(IMPLOUT) << " with type:  " << t->id() ;
          msg << " (" << __FILE__ << ":" << __LINE__ << ")";
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsObjref<XMLImpl,xmlDocPtr,xmlNodePtr,IMPLOUT,TOUT>
    {
      static inline std::string convert(const TypeCode *t,xmlDocPtr doc,xmlNodePtr cur,int protocol)
        {
          cur = cur->xmlChildrenNode;
          while (cur != NULL)
            {
              if ((!xmlStrcmp(cur->name, (const xmlChar *)"objref")))
                {
                  //we wait a objref, we have got a objref
                  xmlChar * s = NULL;
                  std::string mystr = "";
                  s = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                  if (s)
                    {
                      DEBTRACE( "convertToYacsObjref " << (const char *)s );
                      mystr = (const char *)s;
                      xmlFree(s);
                    }
                  else
                    {
                      DEBTRACE("############### workaround to improve...");
                    }
                  return mystr;
                }
              else if ((!xmlStrcmp(cur->name, (const xmlChar *)"string")))// <- here case where pyobj value has been stored in XML. pyobj has kind==ObjRef. And the stored format is String ! EDF11027
                {
                  //wait a string, got a string
                  xmlChar * s = NULL;
                  s = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                  if(s==0)return "";
                  DEBTRACE( "convertToYacsString " << (const char *)s );
                  std::string mystr=std::string((const char *)s);
                  xmlFree(s);
                  return mystr;
                }
              cur = cur->next;
            }
          stringstream msg;
          msg << "Problem in conversion from Xml to " << getImplName(IMPLOUT) << " with type:  " << t->id() ;
          msg << " (" << __FILE__ << ":" << __LINE__ << ")";
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsSequence<XMLImpl,xmlDocPtr,xmlNodePtr,IMPLOUT,TOUT>
    {
      static inline void convert(const TypeCode *t,xmlDocPtr doc,xmlNodePtr cur,std::vector<TOUT>& v)
        {
          cur = cur->xmlChildrenNode;
          while (cur != NULL)
            {
              if ((!xmlStrcmp(cur->name, (const xmlChar *)"array")))
                {
                  DEBTRACE( "parse sequence " );
                  xmlNodePtr cur1=cur->xmlChildrenNode;
                  while (cur1 != NULL)
                    {
                      if ((!xmlStrcmp(cur1->name, (const xmlChar *)"data")))
                        {
                          DEBTRACE( "parse data " );
                          xmlNodePtr cur2=cur1->xmlChildrenNode;
                          while (cur2 != NULL)
                            {
                              //collect all values
                              if ((!xmlStrcmp(cur2->name, (const xmlChar *)"value")))
                                {
                                  TOUT ro=YacsConvertor<XMLImpl,xmlDocPtr,xmlNodePtr,IMPLOUT,TOUT>(t->contentType(),doc,cur2);
                                  v.push_back(ro);
                                }
                              cur2 = cur2->next;
                            } // end while value
                          break;
                        }
                      cur1 = cur1->next;
                    } // end while data
                  break;
                }
              cur = cur->next;
            } // end while array
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsStruct<XMLImpl,xmlDocPtr,xmlNodePtr,IMPLOUT,TOUT>
    {
      static inline void convert(const TypeCode *t,xmlDocPtr doc,xmlNodePtr cur,std::map<std::string,TOUT>& m)
        {
          YACS::ENGINE::TypeCodeStruct* tst=(YACS::ENGINE::TypeCodeStruct*)t;
          int nMember=tst->memberCount();
          DEBTRACE("nMember="<<nMember);
          std::map<std::string,TypeCode*> mtc;
          for(int i=0;i<nMember;i++)
            {
              mtc[tst->memberName(i)]=tst->memberType(i);
            }

          cur = cur->xmlChildrenNode;
          while (cur != NULL)
            {
              if ((!xmlStrcmp(cur->name, (const xmlChar *)"struct")))
                {
                  DEBTRACE( "parse struct " );
                  xmlNodePtr cur1=cur->xmlChildrenNode;
                  while (cur1 != NULL)
                    {
                      if ((!xmlStrcmp(cur1->name, (const xmlChar *)"member")))
                        {
                          DEBTRACE( "parse member " );
                          xmlNodePtr cur2=cur1->xmlChildrenNode;
                          while (cur2 != NULL)
                            {
                              //member name
                              if ((!xmlStrcmp(cur2->name, (const xmlChar *)"name")))
                                {
                                  xmlChar * s = NULL;
                                  s = xmlNodeListGetString(doc, cur2->xmlChildrenNode, 1);
                                  std::string name= (char *)s;
                                  cur2 = cur2->next;
                                  while (cur2 != NULL)
                                    {
                                      if ((!xmlStrcmp(cur2->name, (const xmlChar *)"value")))
                                        {
                                          TOUT ro=YacsConvertor<XMLImpl,xmlDocPtr,xmlNodePtr,IMPLOUT,TOUT>(mtc[name],doc,cur2);
                                          m[name]=ro;
                                          break;
                                        }
                                      cur2 = cur2->next;
                                    }
                                  xmlFree(s);
                                  break;
                                }
                              cur2 = cur2->next;
                            } // end while member/value
                        }
                      cur1 = cur1->next;
                    } // end while member
                  break;
                }
              cur = cur->next;
            } // end while struct
        }
    };
    /* End of ToYacs Convertor for XMLImpl */

    //! FromYacs Convertor for XMLImpl
    /*!
     * Convert YACS<std::string> intermediate types to std::string types (XMLImpl)
     */
    template <>
    struct convertFromYacsDouble<XMLImpl,std::string>
    {
      static inline std::string convert(const TypeCode *t,double o)
        {
          stringstream msg ;
          msg << "<value><double>" << setprecision(16) << o << "</double></value>\n";
          return msg.str();
        }
    };
    template <>
    struct convertFromYacsInt<XMLImpl,std::string>
    {
      static inline std::string convert(const TypeCode *t,long o)
        {
          stringstream msg ;
          msg << "<value><int>" << o << "</int></value>\n";
          return msg.str();
        }
    };
    template <>
    struct convertFromYacsString<XMLImpl,std::string>
    {
      static inline std::string convert(const TypeCode *t,std::string& o)
        {
          std::string msg="<value><string>";
          return msg+o+"</string></value>\n";
        }
    };
    template <>
    struct convertFromYacsBool<XMLImpl,std::string>
    {
      static inline std::string convert(const TypeCode *t,bool o)
        {
          stringstream msg ;
          msg << "<value><boolean>" << o << "</boolean></value>\n";
          return msg.str();
        }
    };
    template <>
    struct convertFromYacsObjref<XMLImpl,std::string>
    {
      static inline std::string convert(const TypeCode *t,std::string& o)
        {
          if(strncmp(t->id(),"python",6)==0 )
            return "<value><objref><![CDATA[" + o + "]]></objref></value>\n";
          else if(strncmp(t->id(),"json",4)==0)
            return "<value><objref><![CDATA[" + o + "]]></objref></value>\n";
          else
            return "<value><objref>" + o + "</objref></value>\n";
        }
    };

    template <>
    struct convertFromYacsSequence<XMLImpl,std::string>
    {
      static inline std::string convert(const TypeCode *t,std::vector<std::string>& v)
        {
          std::vector<std::string>::const_iterator iter;
          stringstream xmlob;
          xmlob << "<value><array><data>\n";
          for(iter=v.begin();iter!=v.end();iter++)
            {
              xmlob << *iter;
            }
          xmlob << "</data></array></value>\n";
          DEBTRACE("Sequence= " << xmlob);
          return xmlob.str();
        }
    };
    template <>
    struct convertFromYacsStruct<XMLImpl,std::string>
    {
      static inline std::string convert(const TypeCode *t,std::map<std::string,std::string>& m)
        {
          std::string result="<value><struct>\n";
          std::map<std::string, std::string>::const_iterator pt;
          for(pt=m.begin();pt!=m.end();pt++)
            {
              std::string name=(*pt).first;
              std::string item=(*pt).second;
              result=result+"<member>\n";
              result=result+"<name>"+name+"</name>\n";
              result=result+item;
              result=result+"</member>\n";
            }
          result=result+"</struct></value>\n";
          return result;
        }
    };

    /* End of FromYacs Convertor for XMLImpl */

    //! ToYacs Convertor for NEUTRALImpl
    /*!
     * This convertor converts Neutral objects to intermediate YACS<TOUT> types
     * Template : Partial specialization for Neutral implementation with types YACS::ENGINE::Any*
     */
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsDouble<NEUTRALImpl,YACS::ENGINE::Any*,void*,IMPLOUT,TOUT>
    {
      static inline double convert(const TypeCode *t,YACS::ENGINE::Any* o,void*)
        {
          if(o->getType()->kind()==Double)
            return o->getDoubleValue();
          else if(o->getType()->kind()==Int)
            return o->getIntValue();

          stringstream msg;
          msg << "Problem in conversion: a double or int is expected " ;
          msg << " (" << __FILE__ << ":" << __LINE__ << ")";
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsInt<NEUTRALImpl,YACS::ENGINE::Any*,void*,IMPLOUT,TOUT>
    {
      static inline long convert(const TypeCode *t,YACS::ENGINE::Any* o,void*)
        {
          if(o->getType()->kind()==Int)
            return o->getIntValue();
          stringstream msg;
          msg << "Problem in conversion: a int is expected " ;
          msg << " (" << __FILE__ << ":" << __LINE__ << ")";
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsString<NEUTRALImpl,YACS::ENGINE::Any*,void*,IMPLOUT,TOUT>
    {
      static inline std::string convert(const TypeCode *t,YACS::ENGINE::Any* o,void*)
        {
          if(o->getType()->kind()==String)
            return o->getStringValue();
          stringstream msg;
          msg << "Problem in conversion: a string is expected " ;
          msg << " (" << __FILE__ << ":" << __LINE__ << ")";
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsBool<NEUTRALImpl,YACS::ENGINE::Any*,void*,IMPLOUT,TOUT>
    {
      static inline bool convert(const TypeCode *t,YACS::ENGINE::Any* o,void*)
        {
          if(o->getType()->kind()==Bool)
            return o->getBoolValue();
          else if(o->getType()->kind()==Int)
            return o->getIntValue() != 0;
          stringstream msg;
          msg << "Problem in conversion: a bool or int is expected " ;
          msg << " (" << __FILE__ << ":" << __LINE__ << ")";
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsObjref<NEUTRALImpl,YACS::ENGINE::Any*,void*,IMPLOUT,TOUT>
    {
      static inline std::string convert(const TypeCode *t,YACS::ENGINE::Any* o,void*,int protocol)
        {
          if(o->getType()->kind()==String)
            return o->getStringValue();
          stringstream msg;
          msg << "Problem in conversion: a objref(string) is expected " ;
          msg << " (" << __FILE__ << ":" << __LINE__ << ")";
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsSequence<NEUTRALImpl,YACS::ENGINE::Any*,void*,IMPLOUT,TOUT>
    {
      static inline void convert(const TypeCode *t,YACS::ENGINE::Any* o,void*,std::vector<TOUT>& v)
        {
          SequenceAny* sdata= (SequenceAny*)o;
          int length=sdata->size();
          v.resize(length);
          for(int i=0;i<length;i++)
            {
              TOUT ro=YacsConvertor<NEUTRALImpl,YACS::ENGINE::Any*,void*,IMPLOUT,TOUT>(t->contentType(),(*sdata)[i],0);
              v[i]=ro;
            }
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsStruct<NEUTRALImpl,YACS::ENGINE::Any*,void*,IMPLOUT,TOUT>
    {
      static inline void convert(const TypeCode *t,YACS::ENGINE::Any* o,void*,std::map<std::string,TOUT>& m)
        {
          StructAny * sdata = dynamic_cast<StructAny *>(o);
          YASSERT(sdata != NULL);
          const TypeCodeStruct * tst = dynamic_cast<const TypeCodeStruct *>(t);
          YASSERT(tst != NULL);
          for (int i=0 ; i<tst->memberCount() ; i++)
            {
              string name = tst->memberName(i);
              TOUT ro=YacsConvertor<NEUTRALImpl,YACS::ENGINE::Any*,void*,IMPLOUT,TOUT>(tst->memberType(i),(*sdata)[name.c_str()],0);
              m[name]=ro;
            }
        }
    };
    /* End of ToYacs Convertor for NEUTRALImpl */

    //! FromYacs Convertor for NEUTRALImpl
    /*!
     * Convert YACS<YACS::ENGINE::Any*> intermediate types to YACS::ENGINE::Any* types (NEUTRALImpl)
     */
    template <>
    struct convertFromYacsDouble<NEUTRALImpl,YACS::ENGINE::Any*>
    {
      static inline YACS::ENGINE::Any* convert(const TypeCode *t,double o)
        {
          YACS::ENGINE::Any *ob=YACS::ENGINE::AtomAny::New(o);
          return ob;
        }
    };
    template <>
    struct convertFromYacsInt<NEUTRALImpl,YACS::ENGINE::Any*>
    {
      static inline YACS::ENGINE::Any* convert(const TypeCode *t,long o)
        {
          return YACS::ENGINE::AtomAny::New((int)o);
        }
    };
    template <>
    struct convertFromYacsString<NEUTRALImpl,YACS::ENGINE::Any*>
    {
      static inline YACS::ENGINE::Any* convert(const TypeCode *t,std::string& o)
        {
          return YACS::ENGINE::AtomAny::New(o);
        }
    };
    template <>
    struct convertFromYacsBool<NEUTRALImpl,YACS::ENGINE::Any*>
    {
      static inline YACS::ENGINE::Any* convert(const TypeCode *t,bool o)
        {
          return YACS::ENGINE::AtomAny::New(o);
        }
    };
    template <>
    struct convertFromYacsObjref<NEUTRALImpl,YACS::ENGINE::Any*>
    {
      static inline YACS::ENGINE::Any* convert(const TypeCode *t,std::string& o)
        {
          //Check if objref is a GenericObj and register it if it is the case (workaround for bad management of GenericObj)
          if(o=="" || (t->isA(Runtime::_tc_file)) || (strncmp(t->id(),"python",6)==0) || (strncmp(t->id(),"json",4)==0))
            return YACS::ENGINE::AtomAny::New(o);

          //Objref CORBA. prefix=IOR,corbaname,corbaloc
          CORBA::Object_var obref;
          try
            {
              obref = getSALOMERuntime()->getOrb()->string_to_object(o.c_str());
            }
          catch(CORBA::Exception& ex)
            {
              throw ConversionException("Can't get reference to object");
            }
          if(obref->_non_existent())
            throw ConversionException("non_existent object");
          if( CORBA::is_nil(obref) )
            throw ConversionException("Can't get reference to object");
          if(!obref->_is_a(t->id()))
            {
              stringstream msg;
              msg << "Problem in conversion: an objref " << t->id() << " is expected " << endl;
              msg << "An objref of type " << obref->_PD_repoId << " is given " << endl;
              msg << " (" << __FILE__ << ":" << __LINE__ << ")";
              throw YACS::ENGINE::ConversionException(msg.str());
            }

          SALOME::GenericObj_var gobj=SALOME::GenericObj::_narrow(obref);
          if(!CORBA::is_nil(gobj))
            {
              DEBTRACE("It's a SALOME::GenericObj register it");
              gobj->Register();
            }
          else
              DEBTRACE("It's a CORBA::Object but not a SALOME::GenericObj");

          return YACS::ENGINE::AtomAny::New(o);
        }
    };

    template <>
    struct convertFromYacsSequence<NEUTRALImpl,YACS::ENGINE::Any*>
    {
      static inline YACS::ENGINE::Any* convert(const TypeCode *t,std::vector<YACS::ENGINE::Any*>& v)
        {
          std::vector<YACS::ENGINE::Any*>::const_iterator iter;
          //Objref are managed as string within YACS::ENGINE::Any objs
          SequenceAny* any;
          any=SequenceAny::New(t->contentType());
          for(iter=v.begin();iter!=v.end();iter++)
            {
              any->pushBack(*iter);
              (*iter)->decrRef();
            }
          DEBTRACE( "refcnt: " << any->getRefCnt() );
          return any;
        }
    };

    template <>
    struct convertFromYacsStruct<NEUTRALImpl,YACS::ENGINE::Any*>
    {
      static inline YACS::ENGINE::Any* convert(const TypeCode *t,std::map<std::string,YACS::ENGINE::Any*>& m)
        {
          StructAny * any = StructAny::New((TypeCodeStruct *)t);
          std::map<std::string,YACS::ENGINE::Any*>::const_iterator it;
          for (it=m.begin() ; it!=m.end() ; it++)
            {
              any->setEltAtRank(it->first.c_str(), it->second);
              it->second->decrRef();
            }
          return any;
        }
    };
    /* End of FromYacs Convertor for NEUTRALImpl */

    //! ToYacs Convertor for CORBAImpl
    /*!
     * This convertor converts Corba objects to intermediate YACS<TOUT> types
     * Template : Partial specialization for CORBA implementation with types CORBA::Any*
     */
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsDouble<CORBAImpl,CORBA::Any*,void*,IMPLOUT,TOUT>
    {
      static inline double convert(const TypeCode *t,CORBA::Any* o,void*)
        {
          CORBA::TypeCode_var tc = o->type();
          if (tc->equivalent(CORBA::_tc_double))
            {
              CORBA::Double d;
              *o >>= d;
              return d;
            }
          if (tc->equivalent(CORBA::_tc_long))
            {
              CORBA::Long d;
              *o >>= d;
              return d;
            }
          stringstream msg;
          msg << "Problem in CORBA to TOUT conversion: kind= " << t->kind() ;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsInt<CORBAImpl,CORBA::Any*,void*,IMPLOUT,TOUT>
    {
      static inline long convert(const TypeCode *t,CORBA::Any* o,void*)
        {
          CORBA::Long d;
          if(*o >>= d)
            return d;
          stringstream msg;
          msg << "Problem in CORBA to TOUT conversion: kind= " << t->kind() ;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsString<CORBAImpl,CORBA::Any*,void*,IMPLOUT,TOUT>
    {
      static inline std::string convert(const TypeCode *t,CORBA::Any* o,void*)
        {
          const char *s;
          if(*o >>=s)
            return s;
          stringstream msg;
          msg << "Problem in CORBA to TOUT conversion: kind= " << t->kind() ;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsBool<CORBAImpl,CORBA::Any*,void*,IMPLOUT,TOUT>
    {
      static inline bool convert(const TypeCode *t,CORBA::Any* o,void*)
        {
          CORBA::Boolean b;
          if(*o >>= CORBA::Any::to_boolean(b))
            return b;
          stringstream msg;
          msg << "Problem in Corba to TOUT conversion: kind= " << t->kind() ;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsObjref<CORBAImpl,CORBA::Any*,void*,IMPLOUT,TOUT>
    {
      static inline std::string convert(const TypeCode *t,CORBA::Any* o,void*,int protocol)
        {
          char file[]="/tmp/XXXXXX";
          if(t->isA(Runtime::_tc_file))
            {
              Engines::Salome_file_ptr sf;
              *o >>= sf;
              Salome_file_i* f=new Salome_file_i();
              mkstemp(file);
              f->setDistributedFile(file);
              f->connect(sf);
              f->recvFiles();
              delete f;
              return file;
            }
          else if(strncmp(t->id(),"python",6)==0)
            {
              const char *s;
              Engines::fileBlock * buffer;
              if(*o >>=buffer)
                {
                  s=(const char*)buffer->get_buffer();

                  if(protocol !=0)
                    {
                      std::string mystr(s,buffer->length());
                      return mystr;
                    }

                  PyGILState_STATE gstate = PyGILState_Ensure(); 
                  PyObject* mod=PyImport_ImportModule("cPickle");
                  PyObject *ob=PyObject_CallMethod(mod,(char *)"loads",(char *)"s#",s,buffer->length());
                  PyObject *pickled=PyObject_CallMethod(mod,(char *)"dumps",(char *)"Oi",ob,protocol);
                  DEBTRACE(PyObject_REPR(pickled));
                  std::string mystr=PyString_AsString(pickled);
                  Py_DECREF(mod);
                  Py_DECREF(ob);
                  Py_DECREF(pickled);
                  PyGILState_Release(gstate);

                  return mystr;
                }
              stringstream msg;
              msg << "Problem in CORBA (protocol python) to TOUT conversion: kind= " << t->kind() ;
              msg << " : " << __FILE__ << ":" << __LINE__;
              throw YACS::ENGINE::ConversionException(msg.str());
            }
          else if(strncmp(t->id(),"json",4)==0)
            {
              const char *s;
              if(*o >>=s)
                {
                  return s;
                }
              stringstream msg;
              msg << "Problem in CORBA (protocol json) to TOUT conversion: kind= " << t->kind() ;
              msg << " : " << __FILE__ << ":" << __LINE__;
              throw YACS::ENGINE::ConversionException(msg.str());
            }
          else
            {
              CORBA::Object_var ObjRef ;
              *o >>= CORBA::Any::to_object(ObjRef) ;
              CORBA::String_var objref = getSALOMERuntime()->getOrb()->object_to_string(ObjRef);
              return (char *)objref;
            }
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsSequence<CORBAImpl,CORBA::Any*,void*,IMPLOUT,TOUT>
    {
      static inline void convert(const TypeCode *t,CORBA::Any* o,void*,std::vector<TOUT>& v)
        {
          CORBA::TypeCode_var tc=o->type();
          if (tc->kind() != CORBA::tk_sequence)
            {
              stringstream msg;
              msg << "Not a sequence corba type " << tc->kind();
              msg << " : " << __FILE__ << ":" << __LINE__;
              throw YACS::ENGINE::ConversionException(msg.str());
            }
          DynamicAny::DynAny_ptr dynany=getSALOMERuntime()->getDynFactory()->create_dyn_any(*o);
          DynamicAny::DynSequence_ptr ds=DynamicAny::DynSequence::_narrow(dynany);
          CORBA::release(dynany);
          DynamicAny::AnySeq_var as=ds->get_elements();
          int len=as->length();
          v.resize(len);
          for(int i=0;i<len;i++)
            {
#ifdef REFCNT
              DEBTRACE("refcount CORBA as[i]: " << ((omni::TypeCode_base*)as[i].pd_tc.in())->pd_ref_count);
#endif
              TOUT ro=YacsConvertor<CORBAImpl,CORBA::Any*,void*,IMPLOUT,TOUT>(t->contentType(),&as[i],0);
              v[i]=ro;
            }
          ds->destroy();
          CORBA::release(ds);
          for(int i=0;i<len;i++)
            {
#ifdef REFCNT
              DEBTRACE("refcount CORBA as[i]: " << ((omni::TypeCode_base*)as[i].pd_tc.in())->pd_ref_count);
#endif
            }
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsStruct<CORBAImpl,CORBA::Any*,void*,IMPLOUT,TOUT>
    {
      static inline void convert(const TypeCode *t,CORBA::Any* o,void*,std::map<std::string,TOUT>& m)
        {
          CORBA::TypeCode_var tc=o->type();
          DEBTRACE(tc->kind());
          if (tc->kind() != CORBA::tk_struct)
            {
              stringstream msg;
              msg << "Not a struct corba type " << tc->kind();
              msg << " : " << __FILE__ << ":" << __LINE__;
              throw YACS::ENGINE::ConversionException(msg.str());
            }
          YACS::ENGINE::TypeCodeStruct* tst=(YACS::ENGINE::TypeCodeStruct*)t;
          DynamicAny::DynAny_ptr dynany=getSALOMERuntime()->getDynFactory()->create_dyn_any(*o);
          DynamicAny::DynStruct_ptr ds=DynamicAny::DynStruct::_narrow(dynany);
          CORBA::release(dynany);
          DynamicAny::NameValuePairSeq_var as=ds->get_members();
          int len=as->length();
          for(int i=0;i<len;i++)
            {
              std::string name=as[i].id.in();
              DEBTRACE(name);
              CORBA::Any value=as[i].value;
#ifdef REFCNT
              DEBTRACE("refcount CORBA value: " << ((omni::TypeCode_base*)value.pd_tc.in())->pd_ref_count);
#endif
              TOUT ro=YacsConvertor<CORBAImpl,CORBA::Any*,void*,IMPLOUT,TOUT>(tst->memberType(i),&value,0);
              m[name]=ro;
            }
          ds->destroy();
          CORBA::release(ds);
        }
    };
    /* End of ToYacs Convertor for CORBAImpl */

    //! FromYacs Convertor for CORBAImpl
    /*!
     * Convert YACS<CORBA::Any*> intermediate types to CORBA::Any* types (CORBAImpl)
     */
    template <>
    struct convertFromYacsDouble<CORBAImpl,CORBA::Any*>
    {
      static inline CORBA::Any* convert(const TypeCode *t,double o)
        {
          CORBA::Any *any = new CORBA::Any();
          *any <<= (CORBA::Double)o;
          return any;
        }
    };
    template <>
    struct convertFromYacsInt<CORBAImpl,CORBA::Any*>
    {
      static inline CORBA::Any* convert(const TypeCode *t,long o)
        {
          CORBA::Any *any = new CORBA::Any();
          *any <<= (CORBA::Long)o;
          return any;
        }
    };
    template <>
    struct convertFromYacsString<CORBAImpl,CORBA::Any*>
    {
      static inline CORBA::Any* convert(const TypeCode *t,std::string& o)
        {
          CORBA::Any *any = new CORBA::Any();
          *any <<= o.c_str();
          return any;
        }
    };
    template <>
    struct convertFromYacsBool<CORBAImpl,CORBA::Any*>
    {
      static inline CORBA::Any* convert(const TypeCode *t,bool o)
        {
          CORBA::Any *any = new CORBA::Any();
          *any <<= CORBA::Any::from_boolean(o);
          return any;
        }
    };
    template <>
    struct convertFromYacsObjref<CORBAImpl,CORBA::Any*>
    {
      static inline CORBA::Any* convert(const TypeCode *t,std::string& o)
        {
          CORBA::Object_var obref;

          if(t->isA(Runtime::_tc_file))
            {
              //It's an objref file. Convert it specially
              Salome_file_i* aSalome_file = new Salome_file_i();
              try
                {
                  aSalome_file->setLocalFile(o.c_str());
                  obref = aSalome_file->_this();
                  aSalome_file->_remove_ref();
                }
              catch (const SALOME::SALOME_Exception& e)
                {
                  stringstream msg;
                  msg << e.details.text;
                  msg << " : " << __FILE__ << ":" << __LINE__;
                  throw YACS::ENGINE::ConversionException(msg.str());
                }
            }
          else if(strncmp(t->id(),"python",6)==0 )
            {
              CORBA::Any *any = new CORBA::Any();
              Engines::fileBlock * buffer=new Engines::fileBlock();
              buffer->length(o.length());
              CORBA::Octet *buf=buffer->get_buffer();
              memcpy(buf,o.c_str(),o.length());
              *any <<= buffer;
              return any;
            }
          else if(strncmp(t->id(),"json",4)==0)
            {
              CORBA::Any *any = new CORBA::Any();
              *any <<= o.c_str();
              return any;
            }
          else
            {
              try
                {
                  obref=getSALOMERuntime()->getOrb()->string_to_object(o.c_str());
                }
              catch(CORBA::Exception& ex)
                {
                  throw ConversionException("Can't get reference to object");
                }
              if( CORBA::is_nil(obref) )
                {
                  throw ConversionException("Can't get reference to object");
                }
            }
#ifdef REFCNT
          DEBTRACE("ObjRef refCount: " << obref->_PR_getobj()->pd_refCount);
#endif
          CORBA::Any *any = new CORBA::Any();
          *any <<= obref;
#ifdef REFCNT
          DEBTRACE("ObjRef refCount: " << obref->_PR_getobj()->pd_refCount);
#endif
          return any;
        }
    };

    template <>
    struct convertFromYacsSequence<CORBAImpl,CORBA::Any*>
    {
      static inline CORBA::Any* convert(const TypeCode *t,std::vector<CORBA::Any*>& v)
        {
          CORBA::ORB_ptr orb=getSALOMERuntime()->getOrb();
          std::vector<CORBA::Any*>::const_iterator iter;

          // Build an Any from vector v
          int isObjref=0;
          if(t->contentType()->kind() == Objref)
            isObjref=1;

          CORBA::TypeCode_var tc=getCorbaTC(t);

          DynamicAny::DynAny_var dynany=getSALOMERuntime()->getDynFactory()->create_dyn_any_from_type_code(tc);
          DynamicAny::DynSequence_var ds = DynamicAny::DynSequence::_narrow(dynany);
          ds->set_length(v.size());

          for(iter=v.begin();iter!=v.end();iter++)
            {
              DynamicAny::DynAny_var temp=ds->current_component();
              CORBA::Any* a=*iter;
              //It seems that from_any does not support inherited objref: convert to CORBA::Object and insert reference
              if(isObjref)
                {
                  CORBA::Object_var zzobj ;
                  *a >>= CORBA::Any::to_object(zzobj) ;
                  temp->insert_reference(zzobj);
                }
              else
                temp->from_any(*a);

              //delete intermediate any
              delete a;
              ds->next();
            }

          CORBA::Any *any=ds->to_any();
          ds->destroy();
          return any;
        }
    };
    template <>
    struct convertFromYacsStruct<CORBAImpl,CORBA::Any*>
    {
      static inline CORBA::Any* convert(const TypeCode *t,std::map<std::string,CORBA::Any*>& m)
        {
          CORBA::ORB_ptr orb=getSALOMERuntime()->getOrb();

          YACS::ENGINE::TypeCodeStruct* tst=(YACS::ENGINE::TypeCodeStruct*)t;
          int nMember=tst->memberCount();
          DEBTRACE("nMember="<<nMember);

          CORBA::TypeCode_var tc=getCorbaTC(t);
          DynamicAny::DynAny_var dynany=getSALOMERuntime()->getDynFactory()->create_dyn_any_from_type_code(tc);
          DynamicAny::DynStruct_var ds = DynamicAny::DynStruct::_narrow(dynany);

          for(int i=0;i<nMember;i++)
            {
              DynamicAny::DynAny_var temp=ds->current_component();
              const char * name=tst->memberName(i);
              DEBTRACE("Member name="<<name);
              //do not test member presence : test has been done in ToYacs convertor
              CORBA::Any* a=m[name];
              //It seems that from_any does not support inherited objref: convert to CORBA::Object and insert reference
              CORBA::TypeCode_var atc = tc->member_type(i);
              if(atc->kind()==CORBA::tk_objref)
                {
                  //special treatment for objref
                  CORBA::Object_var zzobj ;
                  *a >>= CORBA::Any::to_object(zzobj) ;
                  temp->insert_reference(zzobj);
                }
              else
                {
                  temp->from_any(*a);
                }
              //delete intermediate any
              delete a;
              ds->next();
            }
          CORBA::Any *any=ds->to_any();
          ds->destroy();

          return any;
        }
    };
    /* End of FromYacs Convertor for CORBAImpl */

    /* Some shortcuts for CORBA to CORBA conversion */
    template <>
    inline CORBA::Any* convertDouble<CORBAImpl,CORBA::Any*,void*,CORBAImpl,CORBA::Any*>(const TypeCode *t,CORBA::Any* o,void* aux)
    {
      CORBA::TypeCode_var tc = o->type();
      if (tc->equivalent(CORBA::_tc_double))
        {
          return o;
        }
      if (tc->equivalent(CORBA::_tc_long))
        {
          CORBA::Long d;
          *o >>= d;
          CORBA::Any *any = new CORBA::Any();
          *any <<= (CORBA::Double)d;
          return any;
        }
      stringstream msg;
      msg << "Not a double or long corba type " << tc->kind();
      msg << " : " << __FILE__ << ":" << __LINE__;
      throw YACS::ENGINE::ConversionException(msg.str());
    }
    template <>
    inline CORBA::Any* convertInt<CORBAImpl,CORBA::Any*,void*,CORBAImpl,CORBA::Any*>(const TypeCode *t,CORBA::Any* o,void* aux)
    {
      return o;
    }
    template <>
    inline CORBA::Any* convertString<CORBAImpl,CORBA::Any*,void*,CORBAImpl,CORBA::Any*>(const TypeCode *t,CORBA::Any* o,void* aux)
    {
      return o;
    }
    template <>
    inline CORBA::Any* convertBool<CORBAImpl,CORBA::Any*,void*,CORBAImpl,CORBA::Any*>(const TypeCode *t,CORBA::Any* o,void* aux)
    {
      return o;
    }
    template <>
    inline CORBA::Any* convertObjref<CORBAImpl,CORBA::Any*,void*,CORBAImpl,CORBA::Any*>(const TypeCode *t,CORBA::Any* o,void* aux)
    {
      return o;
    }
    template <>
    inline CORBA::Any* convertStruct<CORBAImpl,CORBA::Any*,void*,CORBAImpl,CORBA::Any*>(const TypeCode *t,CORBA::Any* o,void* aux)
    {
      return o;
    }
    /* End of shortcuts for CORBA to CORBA conversion */

    //! ToYacs Convertor for CPPImpl
    /*!
     * This convertor converts Python object to YACS<TOUT> types
     * Partial specialization for Python implementation with type PyObject* (PYTHONImpl)
     */
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsDouble<CPPImpl,void*,const TypeCode*,IMPLOUT,TOUT>
    {
      static inline double convert(const TypeCode *t,void* o,const TypeCode* intype)
        {
          if(intype->kind()==YACS::ENGINE::Double)
            {
              return *(double*)o;
            }
          else if(intype->kind()==YACS::ENGINE::Int)
            {
              return *(long*)o;
            }
          stringstream msg;
          msg << "Problem in Cpp to TOUT conversion: kind= " << t->kind() ;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    template <ImplType IMPLOUT, class TOUT>
    struct convertToYacsInt<CPPImpl,void*,const TypeCode*,IMPLOUT,TOUT>
    {
      static inline long convert(const TypeCode *t,void* o,const TypeCode* intype)
        {
          if(intype->kind()==YACS::ENGINE::Int)
            {
              return *(long*)o;
            }
          stringstream msg;
          msg << "Problem in Cpp to TOUT conversion: kind= " << t->kind() ;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    };
    /* End of ToYacs Convertor for CPPImpl */

    //Python conversions
    std::string convertPyObjectXml(const TypeCode *t,PyObject *data)
      {
        return YacsConvertor<PYTHONImpl,PyObject*,void*,XMLImpl,std::string>(t,data,0);
      }
    YACS::ENGINE::Any* convertPyObjectNeutral(const TypeCode *t,PyObject *data)
      {
        return YacsConvertor<PYTHONImpl,PyObject*,void*,NEUTRALImpl,YACS::ENGINE::Any*>(t,data,0);
      }
    CORBA::Any* convertPyObjectCorba(const TypeCode *t,PyObject *data)
      {
        return YacsConvertor<PYTHONImpl,PyObject*,void*,CORBAImpl,CORBA::Any*>(t,data,0);
      }
    PyObject* convertPyObjectPyObject(const TypeCode *t,PyObject *data)
      {
        return YacsConvertor<PYTHONImpl,PyObject*,void*,PYTHONImpl,PyObject*>(t,data,0);
      }

    std::string convertPyObjectToString(PyObject* ob)
    {
      PyObject *s;
      PyGILState_STATE gstate = PyGILState_Ensure(); 
      s=PyObject_Str(ob);
      std::string ss(PyString_AsString(s),PyString_Size(s));
      Py_DECREF(s);
      PyGILState_Release(gstate);
      return ss;
    }

    //XML conversions
    PyObject* convertXmlPyObject(const TypeCode *t,xmlDocPtr doc,xmlNodePtr cur)
      {
        return YacsConvertor<XMLImpl,xmlDocPtr,xmlNodePtr,PYTHONImpl,PyObject*>(t,doc,cur);
      }
    YACS::ENGINE::Any* convertXmlNeutral(const TypeCode *t,xmlDocPtr doc,xmlNodePtr cur)
      {
        return YacsConvertor<XMLImpl,xmlDocPtr,xmlNodePtr,NEUTRALImpl,YACS::ENGINE::Any*>(t,doc,cur);
      }
    CORBA::Any* convertXmlCorba(const TypeCode *t,xmlDocPtr doc,xmlNodePtr cur)
      {
        return YacsConvertor<XMLImpl,xmlDocPtr,xmlNodePtr,CORBAImpl,CORBA::Any*>(t,doc,cur);
      }
    PyObject* convertXmlStrPyObject(const TypeCode *t,std::string data)
      {
        xmlDocPtr doc;
        xmlNodePtr cur;
        PyObject *ob=NULL;
        doc = xmlParseMemory(data.c_str(), strlen(data.c_str()));
        if (doc == NULL )
        {
          std::stringstream msg;
          msg << "Problem in conversion: XML Document not parsed successfully ";
          msg << " (" << __FILE__ << ":" << __LINE__ << ")";
          throw YACS::ENGINE::ConversionException(msg.str());
        }
        cur = xmlDocGetRootElement(doc);
        if (cur == NULL)
        {
          xmlFreeDoc(doc);
          std::stringstream msg;
          msg << "Problem in conversion: empty XML Document";
          msg << " (" << __FILE__ << ":" << __LINE__ << ")";
          throw YACS::ENGINE::ConversionException(msg.str());
        }
        while (cur != NULL)
        {
          if ((!xmlStrcmp(cur->name, (const xmlChar *)"value")))
          {
            ob=convertXmlPyObject(t,doc,cur);
            break;
          }
          cur = cur->next;
        }
        xmlFreeDoc(doc);
        if(ob==NULL)
        {
          std::stringstream msg;
          msg << "Problem in conversion: incorrect XML value";
          msg << " (" << __FILE__ << ":" << __LINE__ << ")";
          throw YACS::ENGINE::ConversionException(msg.str());
        }
        return ob;
      }
    //NEUTRAL conversions
    PyObject* convertNeutralPyObject(const TypeCode *t,YACS::ENGINE::Any* data)
      {
        return YacsConvertor<NEUTRALImpl,YACS::ENGINE::Any*,void*,PYTHONImpl,PyObject*>(t,data,0);
      }
    std::string convertNeutralXml(const TypeCode *t,YACS::ENGINE::Any* data)
      {
        return YacsConvertor<NEUTRALImpl,YACS::ENGINE::Any*,void*,XMLImpl,std::string>(t,data,0);
      }
    CORBA::Any* convertNeutralCorba(const TypeCode *t,YACS::ENGINE::Any* data)
      {
        return YacsConvertor<NEUTRALImpl,YACS::ENGINE::Any*,void*,CORBAImpl,CORBA::Any*>(t,data,0);
      }
    YACS::ENGINE::Any *convertNeutralNeutral(const TypeCode *t, YACS::ENGINE::Any* data)
      {
        data->incrRef();
        return data;
      }

    //CORBA conversions
    PyObject* convertCorbaPyObject(const TypeCode *t,CORBA::Any* data)
      {
        return YacsConvertor<CORBAImpl,CORBA::Any*,void*,PYTHONImpl,PyObject*>(t,data,0);
      }
    std::string convertCorbaXml(const TypeCode *t,CORBA::Any* data)
      {
        return YacsConvertor<CORBAImpl,CORBA::Any*,void*,XMLImpl,std::string>(t,data,0);
      }
    YACS::ENGINE::Any* convertCorbaNeutral(const TypeCode *t,CORBA::Any* data)
      {
        return YacsConvertor<CORBAImpl,CORBA::Any*,void*,NEUTRALImpl,YACS::ENGINE::Any*>(t,data,0);
      }
    CORBA::Any *convertCorbaCorba(const TypeCode *t,CORBA::Any *data)
      {
        return YacsConvertor<CORBAImpl,CORBA::Any*,void*,CORBAImpl,CORBA::Any*>(t,data,0);
      }

    //! Basic template checker from type TIN 
    /*!
     * This checker does nothing : throws exception
     * It must be partially specialize for a specific type (TIN)
     */
    template <ImplType IMPLIN,class TIN,class TIN2>
    static inline bool checkDouble(const TypeCode *t,TIN o,TIN2 aux)
        {
          stringstream msg;
          msg << "Check not implemented for Implementation: " << IMPLIN ;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    template <ImplType IMPLIN,class TIN,class TIN2>
    static inline bool checkInt(const TypeCode *t,TIN o,TIN2 aux)
        {
          stringstream msg;
          msg << "Check not implemented for Implementation: " << IMPLIN ;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    template <ImplType IMPLIN,class TIN,class TIN2>
    static inline bool checkBool(const TypeCode *t,TIN o,TIN2 aux)
        {
          stringstream msg;
          msg << "Check not implemented for Implementation: " << IMPLIN ;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    template <ImplType IMPLIN,class TIN,class TIN2>
    static inline bool checkString(const TypeCode *t,TIN o,TIN2 aux)
        {
          stringstream msg;
          msg << "Check not implemented for Implementation: " << IMPLIN ;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    template <ImplType IMPLIN,class TIN,class TIN2>
    static inline bool checkObjref(const TypeCode *t,TIN o,TIN2 aux)
        {
          stringstream msg;
          msg << "Check not implemented for Implementation: " << IMPLIN ;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    template <ImplType IMPLIN,class TIN,class TIN2>
    static inline bool checkSequence(const TypeCode *t,TIN o,TIN2 aux)
        {
          stringstream msg;
          msg << "Check not implemented for Implementation: " << IMPLIN ;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    template <ImplType IMPLIN,class TIN,class TIN2>
    static inline bool checkStruct(const TypeCode *t,TIN o,TIN2 aux)
        {
          stringstream msg;
          msg << "Check not implemented for Implementation: " << IMPLIN ;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }
    template <ImplType IMPLIN,class TIN,class TIN2>
    static inline bool checkArray(const TypeCode *t,TIN o,TIN2 aux)
        {
          stringstream msg;
          msg << "Check not implemented for Implementation: " << IMPLIN ;
          msg << " : " << __FILE__ << ":" << __LINE__;
          throw YACS::ENGINE::ConversionException(msg.str());
        }

    template <ImplType IMPLIN,class TIN,class TIN2>
    inline bool YacsChecker(const TypeCode *t,TIN o,TIN2 aux)
      {
         int tk=t->kind();
         switch(t->kind())
           {
           case Double:
             return checkDouble<IMPLIN,TIN,TIN2>(t,o,aux);
           case Int:
             return checkInt<IMPLIN,TIN,TIN2>(t,o,aux);
           case String:
             return checkString<IMPLIN,TIN,TIN2>(t,o,aux);
           case Bool:
             return checkBool<IMPLIN,TIN,TIN2>(t,o,aux);
           case Objref:
             return checkObjref<IMPLIN,TIN,TIN2>(t,o,aux);
           case Sequence:
             return checkSequence<IMPLIN,TIN,TIN2>(t,o,aux);
           case Array:
             return checkArray<IMPLIN,TIN,TIN2>(t,o,aux);
           case Struct:
             return checkStruct<IMPLIN,TIN,TIN2>(t,o,aux);
           default:
             break;
           }
         stringstream msg;
         msg << "Check not implemented for kind= " << tk ;
         msg << " : " << __FILE__ << ":" << __LINE__;
         throw YACS::ENGINE::ConversionException(msg.str());
      }
    template<>
    inline bool checkDouble<PYTHONImpl,PyObject*,void*>(const TypeCode *t,PyObject* o,void* aux)
      {
        if (PyFloat_Check(o))
          return true;
        else if (PyInt_Check(o))
          return true;
        else if(PyLong_Check(o))
          return true;
        else
          {
            stringstream msg;
            msg << "Not a python double ";
            throw YACS::ENGINE::ConversionException(msg.str());
          }
      }
    template<>
    inline bool checkInt<PYTHONImpl,PyObject*,void*>(const TypeCode *t,PyObject* o,void* aux)
      {
          if (PyInt_Check(o) || PyLong_Check(o))
            return true;
          else
            {
              stringstream msg;
              msg << "Not a python integer ";
              throw YACS::ENGINE::ConversionException(msg.str());
            }
      }
    template<>
    inline bool checkBool<PYTHONImpl,PyObject*,void*>(const TypeCode *t,PyObject* o,void* aux)
      {
          if (PyBool_Check(o))
              return true;
          else if (PyInt_Check(o))
              return true;
          else if(PyLong_Check(o))
              return true;
          else
            {
              stringstream msg;
              msg << "Not a python boolean " ;
              throw YACS::ENGINE::ConversionException(msg.str());
            }

      }
    template<>
    inline bool checkString<PYTHONImpl,PyObject*,void*>(const TypeCode *t,PyObject* o,void* aux)
      {
          if (PyString_Check(o))
            return true;
          else
            {
              stringstream msg;
              msg << "Not a python string " ;
              throw YACS::ENGINE::ConversionException(msg.str());
            }
      }
    template<>
    inline bool checkObjref<PYTHONImpl,PyObject*,void*>(const TypeCode *t,PyObject* o,void* aux)
      {
          if (PyString_Check(o))
            return true;
          if(strncmp(t->id(),"python",6)==0) // a Python object is expected (it's always true)
            return true;
          else if(strncmp(t->id(),"json",4)==0) // The python object must be json pickable
            {
               // The python object should be json compliant (to improve)
               return true;
            }
          else
            {
              // The python object should be a CORBA obj (to improve)
               return true;
            }
      }
    template<>
    inline bool checkSequence<PYTHONImpl,PyObject*,void*>(const TypeCode *t,PyObject* o,void* aux)
      {
        if(!PySequence_Check(o))
          {
            stringstream msg;
            msg << "python object is not a sequence " ;
            throw YACS::ENGINE::ConversionException(msg.str());
          }
        int length=PySequence_Size(o);
        for(int i=0;i<length;i++)
          {
            PyObject *item=PySequence_ITEM(o,i);
            try
              {
                YacsChecker<PYTHONImpl,PyObject*,void*>(t->contentType(),item,0);
              }
            catch(ConversionException& ex)
              {
                stringstream msg;
                msg << ex.what() << " for sequence element " << i;
                throw YACS::ENGINE::ConversionException(msg.str(),false);
              }
            Py_DECREF(item);
          }
        return true;
      }
    template<>
    inline bool checkStruct<PYTHONImpl,PyObject*,void*>(const TypeCode *t,PyObject* o,void* aux)
      {
        PyObject *value;
        if(!PyDict_Check(o))
          {
            stringstream msg;
            msg << "python object is not a dict " ;
            throw YACS::ENGINE::ConversionException(msg.str());
          }
        YACS::ENGINE::TypeCodeStruct* tst=(YACS::ENGINE::TypeCodeStruct*)t;
        int nMember=tst->memberCount();
        for(int i=0;i<nMember;i++)
          {
            std::string name=tst->memberName(i);
            TypeCode* tm=tst->memberType(i);
            value=PyDict_GetItemString(o, name.c_str());
            if(value==NULL)
              {
                stringstream msg;
                msg << "member " << name << " not present " ;
                throw YACS::ENGINE::ConversionException(msg.str());
              }
            try
              {
                YacsChecker<PYTHONImpl,PyObject*,void*>(tm,value,0);
              }
            catch(ConversionException& ex)
              {
                std::string s=" for struct member "+name;
                throw YACS::ENGINE::ConversionException(ex.what()+s,false);
              }
          }
        return true;
      }

    bool checkPyObject(const TypeCode *t,PyObject* ob)
      {
        return YacsChecker<PYTHONImpl,PyObject*,void*>(t,ob,0);
      }
  }
}
