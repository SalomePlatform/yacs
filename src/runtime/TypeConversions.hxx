
#ifndef _TYPECONVERSIONS_HXX_
#define _TYPECONVERSIONS_HXX_

#include <Python.h>
#include <omniORB4/CORBA.h>
#include <libxml/parser.h>

#include "TypeCode.hxx"

namespace YACS
{
  namespace ENGINE
  {

    typedef CORBA::TypeCode_ptr (*getCorbaTCFn)(TypeCode *);
    CORBA::TypeCode_ptr getCorbaTC(TypeCode *t);

    typedef CORBA::Any* (*convertCorbaPyObjectFn)(TypeCode *,PyObject* );
    CORBA::Any *convertCorbaPyObject(TypeCode *t,PyObject *ob);

    typedef int (*isAdaptableCorbaPyObjectFn)(TypeCode *,TypeCode* );
    int isAdaptableCorbaPyObject(TypeCode* t1,TypeCode* t2);

    typedef int (*isAdaptableXmlCorbaFn)(TypeCode *,TypeCode* );
    int isAdaptableXmlCorba(TypeCode *t1,TypeCode *t2);

    typedef int (*isAdaptableCorbaCorbaFn)(TypeCode *,TypeCode* );
    int isAdaptableCorbaCorba(TypeCode* t1,TypeCode* t2);

    typedef int (*isAdaptablePyObjectPyObjectFn)(TypeCode *,TypeCode* );
    int isAdaptablePyObjectPyObject(TypeCode* t1,TypeCode* t2);

    typedef int (*isAdaptablePyObjectCorbaFn)(TypeCode *,TypeCode* );
    int isAdaptablePyObjectCorba(TypeCode* t1,TypeCode* t2);

    typedef PyObject* (*convertPyObjectCorbaFn)(TypeCode *,CORBA::Any* );
    PyObject *convertPyObjectCorba(TypeCode* t,CORBA::Any* ob);

    typedef char* (*convertXmlCorbaFn)(TypeCode *,CORBA::Any* );
    char *convertXmlCorba(TypeCode* t,CORBA::Any* ob);

    typedef CORBA::Any* (*convertCorbaCorbaFn)(TypeCode *,CORBA::Any* );
    CORBA::Any *convertCorbaCorba(TypeCode* t,CORBA::Any* ob);

    typedef CORBA::Any* (*convertCorbaXmlFn)(TypeCode *,xmlDocPtr doc,xmlNodePtr cur);
    CORBA::Any *convertCorbaXml(TypeCode* t,xmlDocPtr doc,xmlNodePtr cur );

  }

}

#endif
