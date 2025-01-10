// Copyright (C) 2006-2025  CEA, EDF
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

#ifndef _TYPECONVERSIONS_HXX_
#define _TYPECONVERSIONS_HXX_

#include "YACSRuntimeSALOMEExport.hxx"

#define  PY_SSIZE_T_CLEAN

#include <Python.h>
#include <omniORB4/CORBA.h>
#include <libxml/parser.h>
#include <string>

#if PY_VERSION_HEX < 0x03050000
static char*
Py_EncodeLocale(const wchar_t *arg, size_t *size)
{
	return _Py_wchar2char(arg, size);
}
static wchar_t*
Py_DecodeLocale(const char *arg, size_t *size)
{
	return _Py_char2wchar(arg, size);
}
#endif

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

    YACSRUNTIMESALOME_EXPORT CORBA::TypeCode_ptr getCorbaTC(const TypeCode *t);

    YACSRUNTIMESALOME_EXPORT int isAdaptableCorbaPyObject(const TypeCode * t1, const TypeCode * t2);
    YACSRUNTIMESALOME_EXPORT int isAdaptableCorbaNeutral(const TypeCode * t1, const TypeCode * t2);
    YACSRUNTIMESALOME_EXPORT int isAdaptableCorbaCorba(const TypeCode * t1, const TypeCode * t2);

    YACSRUNTIMESALOME_EXPORT int isAdaptableNeutralCorba(const TypeCode * t1, const TypeCode * t2);
    YACSRUNTIMESALOME_EXPORT int isAdaptableNeutralNeutral(const TypeCode * t1, const TypeCode * t2);
    YACSRUNTIMESALOME_EXPORT int isAdaptableNeutralXml(const TypeCode * t1, const TypeCode * t2);
    YACSRUNTIMESALOME_EXPORT int isAdaptableNeutralPyObject(const TypeCode * t1, const TypeCode * t2);

    YACSRUNTIMESALOME_EXPORT int isAdaptablePyObjectPyObject(const TypeCode * t1, const TypeCode * t2);
    YACSRUNTIMESALOME_EXPORT int isAdaptablePyObjectCorba(const TypeCode * t1, const TypeCode * t2);
    YACSRUNTIMESALOME_EXPORT int isAdaptablePyObjectNeutral(const TypeCode * t1, const TypeCode * t2);

    YACSRUNTIMESALOME_EXPORT int isAdaptableXmlNeutral(const TypeCode *t1,const TypeCode *t2);
    YACSRUNTIMESALOME_EXPORT int isAdaptableXmlCorba(const TypeCode *t1, const TypeCode *t2);

    YACSRUNTIMESALOME_EXPORT PyObject *convertCorbaPyObject(const TypeCode * t,CORBA::Any* ob);
    YACSRUNTIMESALOME_EXPORT PyObject *convertCorbaPyObject(const TypeCode * t,CORBA::Any* ob);
    YACSRUNTIMESALOME_EXPORT PyObject *convertCorbaPyObject(const TypeCode * t,CORBA::Any* ob);
    YACSRUNTIMESALOME_EXPORT CORBA::Any *convertCorbaCorba(const TypeCode * t,CORBA::Any* ob);
    YACSRUNTIMESALOME_EXPORT YACS::ENGINE::Any *convertCorbaNeutral(const TypeCode *t,CORBA::Any* ob);
    YACSRUNTIMESALOME_EXPORT std::string convertCorbaXml(const TypeCode * t,CORBA::Any* ob);

    YACSRUNTIMESALOME_EXPORT CORBA::Any *convertPyObjectCorba(const TypeCode *t,PyObject *ob);
    YACSRUNTIMESALOME_EXPORT std::string convertPyObjectXml(const TypeCode * t,PyObject* ob);
    YACSRUNTIMESALOME_EXPORT YACS::ENGINE::Any *convertPyObjectNeutral(const TypeCode *t,PyObject* ob);
    YACSRUNTIMESALOME_EXPORT PyObject* convertPyObjectPyObject(const TypeCode *t,PyObject *ob);
    YACSRUNTIMESALOME_EXPORT std::string convertPyObjectToString(PyObject* ob);
    YACSRUNTIMESALOME_EXPORT bool checkPyObject(const TypeCode *t,PyObject* ob);

    YACSRUNTIMESALOME_EXPORT PyObject *convertXmlPyObject(const TypeCode * t,xmlDocPtr doc,xmlNodePtr cur );
    YACSRUNTIMESALOME_EXPORT PyObject *convertXmlStrPyObject(const TypeCode * t,std::string data );
    YACSRUNTIMESALOME_EXPORT CORBA::Any *convertXmlCorba(const TypeCode * t,xmlDocPtr doc,xmlNodePtr cur );
    YACSRUNTIMESALOME_EXPORT YACS::ENGINE::Any *convertXmlNeutral(const TypeCode * t,xmlDocPtr doc,xmlNodePtr cur );

    YACSRUNTIMESALOME_EXPORT PyObject *convertNeutralPyObject(const TypeCode * t,YACS::ENGINE::Any* ob);
    YACSRUNTIMESALOME_EXPORT std::string convertNeutralXml(const TypeCode * t,YACS::ENGINE::Any* ob);
    YACSRUNTIMESALOME_EXPORT CORBA::Any *convertNeutralCorba(const TypeCode *t,YACS::ENGINE::Any *ob);
    YACSRUNTIMESALOME_EXPORT YACS::ENGINE::Any *convertNeutralNeutral(const TypeCode *t, YACS::ENGINE::Any* ob);
  }

}

#endif
