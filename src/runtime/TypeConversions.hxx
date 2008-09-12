
#ifndef _TYPECONVERSIONS_HXX_
#define _TYPECONVERSIONS_HXX_

#include <Python.h>
#include <omniORB4/CORBA.h>
#include <libxml/parser.h>
#include <string>

namespace YACS
{
  namespace ENGINE
  {

    typedef enum
      {
        CORBAImpl    = 1,
        PYTHONImpl   = 2,
        NEUTRALImpl  = 3,
        XMLImpl      = 4,
        CPPImpl      = 5,
      } ImplType;

    class TypeCode;
    class Any;

    CORBA::TypeCode_ptr getCorbaTC(const TypeCode *t);

    int isAdaptableCorbaPyObject(const TypeCode * t1, const TypeCode * t2);
    int isAdaptableCorbaNeutral(const TypeCode * t1, const TypeCode * t2);
    int isAdaptableCorbaCorba(const TypeCode * t1, const TypeCode * t2);

    int isAdaptableNeutralCorba(const TypeCode * t1, const TypeCode * t2);
    int isAdaptableNeutralNeutral(const TypeCode * t1, const TypeCode * t2);
    int isAdaptableNeutralXml(const TypeCode * t1, const TypeCode * t2);
    int isAdaptableNeutralPyObject(const TypeCode * t1, const TypeCode * t2);

    int isAdaptablePyObjectPyObject(const TypeCode * t1, const TypeCode * t2);
    int isAdaptablePyObjectCorba(const TypeCode * t1, const TypeCode * t2);
    int isAdaptablePyObjectNeutral(const TypeCode * t1, const TypeCode * t2);

    int isAdaptableXmlNeutral(const TypeCode *t1,const TypeCode *t2);
    int isAdaptableXmlCorba(const TypeCode *t1, const TypeCode *t2);

    PyObject *convertCorbaPyObject(const TypeCode * t,CORBA::Any* ob);
    CORBA::Any *convertCorbaCorba(const TypeCode * t,CORBA::Any* ob);
    YACS::ENGINE::Any *convertCorbaNeutral(const TypeCode *t,CORBA::Any* ob);
    std::string convertCorbaXml(const TypeCode * t,CORBA::Any* ob);

    CORBA::Any *convertPyObjectCorba(const TypeCode *t,PyObject *ob);
    std::string convertPyObjectXml(const TypeCode * t,PyObject* ob);
    YACS::ENGINE::Any *convertPyObjectNeutral(const TypeCode *t,PyObject* ob);

    PyObject *convertXmlPyObject(const TypeCode * t,xmlDocPtr doc,xmlNodePtr cur );
    PyObject *convertXmlStrPyObject(const TypeCode * t,std::string data );
    CORBA::Any *convertXmlCorba(const TypeCode * t,xmlDocPtr doc,xmlNodePtr cur );
    YACS::ENGINE::Any *convertXmlNeutral(const TypeCode * t,xmlDocPtr doc,xmlNodePtr cur );

    PyObject *convertNeutralPyObject(const TypeCode * t,YACS::ENGINE::Any* ob);
    std::string convertNeutralXml(const TypeCode * t,YACS::ENGINE::Any* ob);
    CORBA::Any *convertNeutralCorba(const TypeCode *t,YACS::ENGINE::Any *ob);
    YACS::ENGINE::Any *convertNeutralNeutral(const TypeCode *t, YACS::ENGINE::Any* ob);
  }

}

#endif
