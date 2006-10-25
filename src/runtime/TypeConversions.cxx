
#include "TypeConversions.hxx"
#include "Exception.hxx"
#include "RuntimeSALOME.hxx"

#include <iostream>
#include <sstream>

using namespace std;

namespace YACS
{
  namespace ENGINE
  {

    /*
     * Fonctions qui retournent un TypeCode CORBA equivalent a un TypeCode Superviseur
     */

    CORBA::TypeCode_ptr getCorbaTCNull(TypeCode *t)
    {
      stringstream msg;
      msg << "Conversion not implemented: kind= " << t->kind() << " : " << __FILE__ << ":" << __LINE__;
      throw YACS::Exception(msg.str());
    }

    CORBA::TypeCode_ptr getCorbaTCDouble(TypeCode *t)
    {
      return CORBA::_tc_double;
    }

    CORBA::TypeCode_ptr getCorbaTCInt(TypeCode *t)
    {
      return CORBA::_tc_long;
    }

    CORBA::TypeCode_ptr getCorbaTCString(TypeCode *t)
    {
      return CORBA::_tc_string;
    }

    CORBA::TypeCode_ptr getCorbaTCObjref(TypeCode *t)
    {
      return CORBA::_tc_Object;
    }

    CORBA::TypeCode_ptr getCorbaTCSequence(TypeCode *t)
    {
      return getSALOMERuntime()->getOrb()->create_sequence_tc(0,getCorbaTC(t->content_type()));
    }

    getCorbaTCFn getCorbaTCFns[]=
      {
	getCorbaTCNull,
	getCorbaTCDouble,
	getCorbaTCInt,
	getCorbaTCString,
	getCorbaTCNull,
	getCorbaTCObjref,
	getCorbaTCSequence,
	getCorbaTCNull,
      };

    CORBA::TypeCode_ptr getCorbaTC(TypeCode *t)
    {
      int tk=t->kind();
      return getCorbaTCFns[tk](t);
    }

    /*
     * Fin des Fonctions qui retournent un TypeCode CORBA equivalent a un TypeCode Superviseur
     */

    /*
     * Fonctions de conversion d'un PyObject * quelconque en CORBA::Any *
     * du type donné par le TypeCode passé en argument
     */

    //Le TypeCode passé en argument est celui du port qui recoit la donnée : InputCorbaPort

    //CORBA::Any *convertCorbaPyObject(TypeCode* t,PyObject* ob);

    CORBA::Any *convertCorbaPyObjectNull(TypeCode *t,PyObject *ob)
    {
      stringstream msg;
      msg << "Conversion not implemented: kind= " << t->kind() << " : " << __FILE__ << ":" << __LINE__;
      throw YACS::Exception(msg.str());
    }

    //kind = 1
    //Conversion d'un objet Python "equivalent double" en CORBA::Any double

    CORBA::Any *convertCorbaPyObjectDouble(TypeCode *t,PyObject *ob)
    {
      PyObject_Print(ob,stdout,Py_PRINT_RAW);
      cerr << endl;
      CORBA::Double d = 0;
      if (PyFloat_Check(ob))
	d = (CORBA::Double)PyFloat_AS_DOUBLE(ob);
      else if (PyInt_Check(ob))
	d = (CORBA::Double)PyInt_AS_LONG(ob);
      else
	d = (CORBA::Double)PyLong_AsDouble(ob);
      CORBA::Any *any = new CORBA::Any();
      *any <<= d;
      return any;
    }

    //kind = 2

    CORBA::Any *convertCorbaPyObjectInt(TypeCode *t,PyObject *ob)
    {
      PyObject_Print(ob,stdout,Py_PRINT_RAW);
      cerr << endl;
      CORBA::Long l;
      if (PyInt_Check(ob))
	l = PyInt_AS_LONG(ob);
      else
	l = PyLong_AsLong(ob);
      CORBA::Any *any = new CORBA::Any();
      *any <<= l;
      return any;
    }

    //kind = 3

    CORBA::Any *convertCorbaPyObjectString(TypeCode *t,PyObject *ob)
    {
      PyObject_Print(ob,stdout,Py_PRINT_RAW);
      cerr << endl;
      char * s=PyString_AsString(ob);
      CORBA::Any *any = new CORBA::Any();
      *any <<= s;
      return any;
    }

    //kind = 5

    CORBA::Any *convertCorbaPyObjectObjref(TypeCode *t,PyObject *ob)
    {
      PyObject_Print(ob,stdout,Py_PRINT_RAW);
      cerr << endl;
      PyObject *pystring=PyObject_CallMethod(getSALOMERuntime()->getPyOrb(),
					     "object_to_string", "O", ob);
      CORBA::Object_ptr obref =
	getSALOMERuntime()->getOrb()->string_to_object(PyString_AsString(pystring));
      Py_DECREF(pystring);
      CORBA::Any *any = new CORBA::Any();
      *any <<= obref;
      cerr << "typecode: " << any->type()->id() << endl;
      return any;
    };

    //kind = 6

    CORBA::Any *convertCorbaPyObjectSequence(TypeCode *t,PyObject *ob)
    {
      PyObject_Print(ob,stdout,Py_PRINT_RAW);
      cerr << endl;
      int length=PySequence_Size(ob);
      cerr <<"length: " << length << endl;
      CORBA::TypeCode_var tc_content;
      DynamicAny::AnySeq as ;
      as.length(length);
      for(int i=0;i<length;i++)
	{
	  PyObject *o=PySequence_ITEM(ob,i);
	  cerr <<"item[" << i << "]=";
	  PyObject_Print(o,stdout,Py_PRINT_RAW);
	  cerr << endl;
	  cerr << "o refcnt: " << o->ob_refcnt << endl;
	  CORBA::Any *a= convertCorbaPyObject(t->content_type(),o);
	  //ici on fait une copie. Peut-on l'éviter ?
	  as[i]=*a;
	  tc_content=a->type();
	  //delete a;
	  Py_DECREF(o);
	}

      CORBA::TypeCode_var tc = 
	getSALOMERuntime()->getOrb()->create_sequence_tc(0,tc_content);
      DynamicAny::DynAny_var dynany =
	getSALOMERuntime()->getDynFactory()->create_dyn_any_from_type_code(tc);
      DynamicAny::DynSequence_var ds =
	DynamicAny::DynSequence::_narrow(dynany);

      try
	{
	  ds->set_elements(as);
	}
      catch(DynamicAny::DynAny::TypeMismatch& ex)
	{
	  throw YACS::Exception("type mismatch");
	}
      catch(DynamicAny::DynAny::InvalidValue& ex)
	{
	  throw YACS::Exception("invalid value");
	}
      CORBA::Any *any=ds->to_any();
      return any;
    }

    convertCorbaPyObjectFn convertCorbaPyObjectFns[] =
      {
	convertCorbaPyObjectNull,
	convertCorbaPyObjectDouble,
	convertCorbaPyObjectInt,
	convertCorbaPyObjectString,
	convertCorbaPyObjectNull,
	convertCorbaPyObjectObjref,
	convertCorbaPyObjectSequence,
      };

    CORBA::Any *convertCorbaPyObject(TypeCode *t,PyObject *ob)
    {
      int tk=t->kind();
      return convertCorbaPyObjectFns[tk](t,ob);
    }

    /*
     * Fin des fonctions de conversion PyObject * -> CORBA::Any *
     */

    /*
     * Fonctions de test d'adaptation pour conversion PyObject * (t1) -> CORBA::Any * (t2)
     */
    //t1 est le type du port output Python
    //t2 est le type du port input Corba

    int isAdaptableCorbaPyObjectNull(TypeCode *t1,TypeCode* t2)
    {
      return 0;
    }

    int isAdaptableCorbaPyObjectDouble(TypeCode *t1,TypeCode* t2)
    {
      if (t1->kind() == Double) return 1;
      if (t1->kind() == Int) return 1;
      return 0;
    }

    int isAdaptableCorbaPyObjectInt(TypeCode *t1,TypeCode* t2)
    {
      if (t1->kind() == Int) return 1;
      return 0;
    }

    int isAdaptableCorbaPyObjectString(TypeCode *t1,TypeCode* t2)
    {
      if (t1->kind() == String)return 1;
      return 0;
    }

    int isAdaptableCorbaPyObjectObjref(TypeCode *t1,TypeCode* t2)
    {
      if(t1->kind() == Objref)
	{
	  //Il faut que le type du inport soit plus général que celui du outport
	  if ( t1->is_a(t2->id()) ) return 1;
	}
      return 0;
    }

    int isAdaptableCorbaPyObjectSequence(TypeCode *t1,TypeCode* t2)
    {
      if(t1->kind() == Sequence)
	{
	  if(isAdaptableCorbaPyObject(t1->content_type(),t2->content_type()))
	    {
	      return 1;
	    }
	}
      return 0;
    }

    isAdaptableCorbaPyObjectFn isAdaptableCorbaPyObjectFns[]=
      {
	isAdaptableCorbaPyObjectNull,
	isAdaptableCorbaPyObjectDouble,
	isAdaptableCorbaPyObjectInt,
	isAdaptableCorbaPyObjectString,
	isAdaptableCorbaPyObjectNull,
	isAdaptableCorbaPyObjectObjref,
	isAdaptableCorbaPyObjectSequence,
      };

    int isAdaptableCorbaPyObject(TypeCode *t1,TypeCode *t2)
    {
      int tk=t2->kind();
      return isAdaptableCorbaPyObjectFns[tk](t1,t2);
    }

    /*
     * Fin des fonctions d'adaptation pour conversion PyObject * -> CORBA::Any *
     */

    /*
     * Fonctions de test d'adaptation pour conversion CORBA::Any * (t1) -> Xml::char * (t2)
     */
    //t1 est le type du port output Corba
    //t2 est le type du port input Xml

    int isAdaptableXmlCorbaNull(TypeCode *t1,TypeCode* t2)
    {
      return 0;
    }

    int isAdaptableXmlCorbaDouble(TypeCode *t1,TypeCode* t2)
    {
      if(t1->kind() == Double)return 1;
      if(t1->kind() == Int)return 1;
      return 0;
    }

    int isAdaptableXmlCorbaInt(TypeCode *t1,TypeCode* t2)
    {
      if(t1->kind() == Int)return 1;
      return 0;
    }

    int isAdaptableXmlCorbaString(TypeCode *t1,TypeCode* t2)
    {
      if(t1->kind() == String)return 1;
      return 0;
    }

    int isAdaptableXmlCorbaObjref(TypeCode *t1,TypeCode* t2)
    {
      if(t1->kind() == Objref)
	{
	  //Il faut que le type du inport soit plus général que celui du outport
	  if( t1->is_a(t2->id()) )return 1;
	}
      return 0;
    }

    int isAdaptableXmlCorbaSequence(TypeCode *t1,TypeCode* t2)
    {
      if(t1->kind() == Sequence)
	{
	  if(isAdaptableXmlCorba(t1->content_type(),t2->content_type()))
	    {
	      return 1;
	    }
	}
      return 0;
    }

    isAdaptableXmlCorbaFn isAdaptableXmlCorbaFns[]=
      {
	isAdaptableXmlCorbaNull,
	isAdaptableXmlCorbaDouble,
	isAdaptableXmlCorbaInt,
	isAdaptableXmlCorbaString,
	isAdaptableXmlCorbaNull,
	isAdaptableXmlCorbaObjref,
	isAdaptableXmlCorbaSequence,
      };

    int isAdaptableXmlCorba(TypeCode *t1,TypeCode *t2)
    {
      int tk=t2->kind();
      return isAdaptableXmlCorbaFns[tk](t1,t2);
    }

    /*
     * Fin des fonctions d'adaptation pour conversion CORBA::Any * (t1) -> Xml::char * (t2)
     */

    /*
     * Fonctions de test d'adaptation pour conversion CORBA::Any * (t1) -> CORBA::Any * (t2)
     */
    //t1 est le type du port output Corba
    //t2 est le type du port input Corba

    int isAdaptableCorbaCorbaNull(TypeCode *t1,TypeCode* t2)
    {
      return 0;
    }

    int isAdaptableCorbaCorbaDouble(TypeCode *t1,TypeCode* t2)
    {
      if(t1->kind() == Double)return 1;
      if(t1->kind() == Int)return 1;
      return 0;
    }

    int isAdaptableCorbaCorbaInt(TypeCode *t1,TypeCode* t2)
    {
      if(t1->kind() == Int)return 1;
      return 0;
    }

    int isAdaptableCorbaCorbaString(TypeCode *t1,TypeCode* t2)
    {
      if(t1->kind() == String)return 1;
      return 0;
    }

    int isAdaptableCorbaCorbaObjref(TypeCode *t1,TypeCode* t2)
    {
      if(t1->kind() == Objref){
	//Il faut que le type du inport soit plus général que celui du outport
	if( t1->is_a(t2->id()) )return 1;
      }
      return 0;
    }

    int isAdaptableCorbaCorbaSequence(TypeCode *t1,TypeCode* t2)
    {
      if(t1->kind() == Sequence)
	{
	  if(isAdaptableCorbaCorba(t1->content_type(),t2->content_type()))
	    {
	      return 1;
	    }
	}
      return 0;
    }

    isAdaptableCorbaCorbaFn isAdaptableCorbaCorbaFns[]=
      {
	isAdaptableCorbaCorbaNull,
	isAdaptableCorbaCorbaDouble,
	isAdaptableCorbaCorbaInt,
	isAdaptableCorbaCorbaString,
	isAdaptableCorbaCorbaNull,
	isAdaptableCorbaCorbaObjref,
	isAdaptableCorbaCorbaSequence,
      };

    int isAdaptableCorbaCorba(TypeCode *t1,TypeCode *t2)
    {
      int tk=t2->kind();
      return isAdaptableCorbaCorbaFns[tk](t1,t2);
    }

    /*
     * Fin des fonctions d'adaptation pour conversion CORBA::Any * -> CORBA::Any *
     */

    /*
     * Fonctions de test d'adaptation pour conversion PyObject * (t1) -> PyObject * (t2)
     */
    //t1 est le type du port output Python
    //t2 est le type du port input Python

    int isAdaptablePyObjectPyObjectNull(TypeCode *t1,TypeCode* t2)
    {
      return 0;
    }

    int isAdaptablePyObjectPyObjectDouble(TypeCode *t1,TypeCode* t2)
    {
      if(t1->kind() == Double)return 1;
      if(t1->kind() == Int)return 1;
      return 0;
    }

    int isAdaptablePyObjectPyObjectInt(TypeCode *t1,TypeCode* t2)
    {
      if(t1->kind() == Int)return 1;
      return 0;
    }

    int isAdaptablePyObjectPyObjectString(TypeCode *t1,TypeCode* t2)
    {
      if(t1->kind() == String)return 1;
      return 0;
    }

    int isAdaptablePyObjectPyObjectObjref(TypeCode *t1,TypeCode* t2)
    {
      if(t1->kind() == Objref)
	{
	  //Il faut que le type du inport soit plus général que celui du outport
	  if( t1->is_a(t2->id()) )return 1;
	}
      return 0;
    }

    int isAdaptablePyObjectPyObjectSequence(TypeCode *t1,TypeCode* t2)
    {
      if(t1->kind() == Sequence)
	{
	  if(isAdaptablePyObjectPyObject(t1->content_type(),t2->content_type()))
	    {
	      return 1;
	    }
	}
      return 0;
    }

    isAdaptablePyObjectPyObjectFn isAdaptablePyObjectPyObjectFns[]=
      {
	isAdaptablePyObjectPyObjectNull,
	isAdaptablePyObjectPyObjectDouble,
	isAdaptablePyObjectPyObjectInt,
	isAdaptablePyObjectPyObjectString,
	isAdaptablePyObjectPyObjectNull,
	isAdaptablePyObjectPyObjectObjref,
	isAdaptablePyObjectPyObjectSequence,
      };

    int isAdaptablePyObjectPyObject(TypeCode *t1,TypeCode *t2)
    {
      int tk=t2->kind();
      return isAdaptablePyObjectPyObjectFns[tk](t1,t2);
    }

    /*
     * Fin des fonctions d'adaptation pour conversion PyObject * -> PyObject *
     */

    /*
     * Fonctions de test d'adaptation pour conversion CORBA::Any * (t1) -> PyObject * (t2)
     */
    //t1 est le type du port output Corba
    //t2 est le type du port input Python

    int isAdaptablePyObjectCorbaNull(TypeCode *t1,TypeCode* t2)
    {
      return 0;
    }

    //on peut convertir un double ou un int en CORBA double
    int isAdaptablePyObjectCorbaDouble(TypeCode *t1,TypeCode* t2)
    {
      if(t1->kind() == Double)return 1;
      if(t1->kind() == Int)return 1;
      return 0;
    }

    int isAdaptablePyObjectCorbaInt(TypeCode *t1,TypeCode* t2)
    {
      if(t1->kind() == Int)return 1;
      return 0;
    }

    int isAdaptablePyObjectCorbaString(TypeCode *t1,TypeCode* t2)
    {
      if(t1->kind() == String)return 1;
      return 0;
    }

    int isAdaptablePyObjectCorbaObjref(TypeCode *t1,TypeCode* t2)
    {
      if(t1->kind() == Objref){
	//Il faut que le type du inport soit plus général que celui du outport
	if( t1->is_a(t2->id()) )return 1;
      }
      return 0;
    }

    int isAdaptablePyObjectCorbaSequence(TypeCode *t1,TypeCode* t2)
    {
      if(t1->kind() == Sequence)
	{
	  if(isAdaptablePyObjectCorba(t1->content_type(),t2->content_type()))
	    {
	      return 1;
	    }
	}
      return 0;
    }

    isAdaptablePyObjectCorbaFn isAdaptablePyObjectCorbaFns[]={
      isAdaptablePyObjectCorbaNull,
      isAdaptablePyObjectCorbaDouble,
      isAdaptablePyObjectCorbaInt,
      isAdaptablePyObjectCorbaString,
      isAdaptablePyObjectCorbaNull,
      isAdaptablePyObjectCorbaObjref,
      isAdaptablePyObjectCorbaSequence,
    };

    int isAdaptablePyObjectCorba(TypeCode *t1,TypeCode *t2)
    {
      int tk=t2->kind();
      return isAdaptablePyObjectCorbaFns[tk](t1,t2);
    }

    /*
     * Fin des fonctions d'adaptation pour conversion CORBA::Any * -> PyObject *
     */

    /*
     * Fonctions de conversion CORBA::Any * -> PyObject *
     */
    //Le TypeCode t est celui vers lequel on convertit (celui de l'InputPort)
    //On a le type Corba de l'objet sortant par ob->type()

    PyObject *convertPyObjectCorbaNull(TypeCode *t,CORBA::Any *ob)
    {
      stringstream msg;
      msg << "Conversion not implemented: kind= " << t->kind() ;
      msg << " : " << __FILE__ << ":" << __LINE__;
      throw YACS::Exception(msg.str());
    }

    //kind=1
    //Convertit un CORBA::Any "equivalent double" en Python double

    PyObject *convertPyObjectCorbaDouble(TypeCode *t,CORBA::Any *ob)
    {
      CORBA::TypeCode_var tc = ob->type();
      if (tc->equivalent(CORBA::_tc_double)) 
	{
	  CORBA::Double d;
	  *ob >>= d;
	  PyObject *pyob=PyFloat_FromDouble(d);
	  cerr << "pyob refcnt: " << pyob->ob_refcnt << endl;
	  return pyob;
	}
      if (tc->equivalent(CORBA::_tc_long))
	{
	  CORBA::Long d;
	  *ob >>= d;
	  //Faudrait-il convertir en PyFloat ??
	  PyObject *pyob=PyInt_FromLong(d);
	  cerr << "pyob refcnt: " << pyob->ob_refcnt << endl;
	  return pyob;
	}
      stringstream msg;
      msg << "Internal error " ;
      msg << " : " << __FILE__ << ":" << __LINE__;
      throw YACS::Exception(msg.str());
    }

    PyObject *convertPyObjectCorbaInt(TypeCode *t,CORBA::Any *ob)
    {
      CORBA::Long l;
      *ob >>= l;
      return PyInt_FromLong(l);
    }

    PyObject *convertPyObjectCorbaString(TypeCode *t,CORBA::Any *ob)
    {
      char *s;
      *ob >>=s;
      PyObject *pyob=PyString_FromString(s);
      cerr << "pyob refcnt: " << pyob->ob_refcnt << endl;
      return pyob;
    }

    PyObject *convertPyObjectCorbaObjref(TypeCode *t,CORBA::Any *ob)
    {
      CORBA::Object_ptr ObjRef ;
      *ob >>= (CORBA::Any::to_object ) ObjRef ;
      //hold_lock is true: caller is supposed to hold the GIL.
      //omniorb will not take the GIL
      PyObject *pyob = getSALOMERuntime()->getApi()->cxxObjRefToPyObjRef(ObjRef, 1);
      cerr << "pyob refcnt: " << pyob->ob_refcnt << endl;
      return pyob;
    }

    PyObject *convertPyObjectCorbaSequence(TypeCode *t,CORBA::Any *ob)
    {
      cerr << "convertPyObjectCorbaSequence" << endl;
      DynamicAny::DynAny_var dynany= getSALOMERuntime()->getDynFactory()->create_dyn_any(*ob);
      DynamicAny::DynSequence_var ds=DynamicAny::DynSequence::_narrow(dynany);
      DynamicAny::AnySeq_var as=ds->get_elements();
      int len=as->length();
      PyObject *pyob = PyList_New(len);
      for(int i=0;i<len;i++)
	{
	  PyObject *e=convertPyObjectCorba(t->content_type(),&as[i]);
	  cerr << "e refcnt: " << e->ob_refcnt << endl;
	  PyList_SetItem(pyob,i,e);
	  cerr << "e refcnt: " << e->ob_refcnt << endl;
	}
      cerr << "pyob refcnt: " << pyob->ob_refcnt << endl;
      cerr << "Sequence= ";
      PyObject_Print(pyob,stdout,Py_PRINT_RAW);
      cerr << endl;
      return pyob;
    }


    convertPyObjectCorbaFn convertPyObjectCorbaFns[]=
      {
	convertPyObjectCorbaNull,
	convertPyObjectCorbaDouble,
	convertPyObjectCorbaInt,
	convertPyObjectCorbaString,
	convertPyObjectCorbaNull,
	convertPyObjectCorbaObjref,
	convertPyObjectCorbaSequence,
      };

    PyObject *convertPyObjectCorba(TypeCode *t,CORBA::Any *ob)
    {
      int tk=t->kind();
      return convertPyObjectCorbaFns[tk](t,ob);
    }

    /*
     * Fin des fonctions de conversion CORBA::Any * -> PyObject *
     */

    /*
     * Fonctions de conversion CORBA::Any * -> Xml char *
     */
    //Le TypeCode t est celui vers lequel on convertit (celui de l'InputPort)
    //On a le type Corba de l'objet sortant par ob->type()

    char *convertXmlCorbaNull(TypeCode *t,CORBA::Any *ob)
    {
      stringstream msg;
      msg << "Conversion not implemented: kind= " << t->kind() ;
      msg << " : " << __FILE__ << ":" << __LINE__;
      throw YACS::Exception(msg.str());
    }

    //kind=1
    //Convertit un CORBA::Any "equivalent double" en Python double

    char *convertXmlCorbaDouble(TypeCode *t,CORBA::Any *ob)
    {
      CORBA::TypeCode_var tc = ob->type();
      if (tc->equivalent(CORBA::_tc_double))
	{
	  CORBA::Double d;
	  *ob >>= d;
	  stringstream msg ;
	  msg << "<value><double>" << d << "</double></value>\n";
	  return (char *)msg.str().c_str();
	}
      if (tc->equivalent(CORBA::_tc_long))
	{
	  CORBA::Long d;
	  *ob >>= d;
	  stringstream msg;
	  msg << "<value><double>" << d << "</double></value>\n";
	  return (char *)msg.str().c_str();
	}
      stringstream msg;
      msg << "Internal error " ;
      msg << " : " << __FILE__ << ":" << __LINE__;
      throw YACS::Exception(msg.str());
    }

    char *convertXmlCorbaInt(TypeCode *t,CORBA::Any *ob)
    {
      CORBA::Long l;
      *ob >>= l;
      stringstream msg ;
      msg << "<value><int>" << l << "</int></value>\n";
      return (char *)msg.str().c_str();
    }

    char *convertXmlCorbaString(TypeCode *t,CORBA::Any *ob)
    {
      char *s;
      *ob >>=s;
      stringstream msg ;
      msg << "<value><string>" << s << "</string></value>\n";
      return (char *)msg.str().c_str();
    }

    char *convertXmlCorbaObjref(TypeCode *t,CORBA::Any *ob)
    {
      CORBA::Object_ptr ObjRef ;
      *ob >>= (CORBA::Any::to_object ) ObjRef ;
      stringstream msg ;
      msg << "<value><objref>" << getSALOMERuntime()->getOrb()->object_to_string(ObjRef) << "</objref></value>\n";
      return (char *)msg.str().c_str();
    }

    char *convertXmlCorbaSequence(TypeCode *t,CORBA::Any *ob)
    {
      cerr << "convertXmlCorbaSequence" << endl;
      DynamicAny::DynAny_var dynany=getSALOMERuntime()->getDynFactory()->create_dyn_any(*ob);
      DynamicAny::DynSequence_var ds=DynamicAny::DynSequence::_narrow(dynany);
      DynamicAny::AnySeq_var as=ds->get_elements();
      int len=as->length();
      stringstream xmlob;
      xmlob << "<value><array><data>\n";
      for(int i=0;i<len;i++)
	{
	  xmlob << convertXmlCorba(t->content_type(),&as[i]);
	}
      xmlob << "</data></array></value>\n";
      cerr << "Sequence= ";
      cerr << xmlob;
      cerr << endl;
      return (char *)xmlob.str().c_str();
    }

    convertXmlCorbaFn convertXmlCorbaFns[]=
      {
	convertXmlCorbaNull,
	convertXmlCorbaDouble,
	convertXmlCorbaInt,
	convertXmlCorbaString,
	convertXmlCorbaNull,
	convertXmlCorbaObjref,
	convertXmlCorbaSequence,
      };

    char *convertXmlCorba(TypeCode *t,CORBA::Any *ob)
    {
      int tk=t->kind();
      return convertXmlCorbaFns[tk](t,ob);
    }

    /*
     * Fin des fonctions de conversion CORBA::Any * -> Xml char *
     */

    /*
     * Fonctions de conversion CORBA::Any * -> CORBA::Any *
     */
    //Le TypeCode t est celui vers lequel on convertit (celui de l'InputPort)
    //On a le type Corba de l'objet sortant par ob->type()

    CORBA::Any *convertCorbaCorbaNull(TypeCode *t,CORBA::Any *ob)
    {
      stringstream msg;
      msg << "Conversion not implemented: kind= " << t->kind() ;
      msg << " : " << __FILE__ << ":" << __LINE__;
      throw YACS::Exception(msg.str());
    };

    //kind=1
    //Convertit un CORBA::Any "equivalent double" en Python double

    CORBA::Any *convertCorbaCorbaDouble(TypeCode *t,CORBA::Any *ob)
    {
      CORBA::TypeCode_var tc = ob->type();
      if (tc->equivalent(CORBA::_tc_double)) 
	{
	  return ob;
	}
      if (tc->equivalent(CORBA::_tc_long))
	{
	  CORBA::Long d;
	  *ob >>= d;
	  CORBA::Any *any = new CORBA::Any();
	  *any <<= (CORBA::Double)d;
	  return any;
	}
      stringstream msg;
      msg << "Internal error " ;
      msg << " : " << __FILE__ << ":" << __LINE__;
      throw YACS::Exception(msg.str());
    }

    CORBA::Any *convertCorbaCorbaInt(TypeCode *t,CORBA::Any *ob)
    {
      return ob;
    }

    CORBA::Any *convertCorbaCorbaString(TypeCode *t,CORBA::Any *ob)
    {
      return ob;
    }

    CORBA::Any *convertCorbaCorbaObjref(TypeCode *t,CORBA::Any *ob)
    {
      return ob;
    }

    CORBA::Any *convertCorbaCorbaSequence(TypeCode *t,CORBA::Any *ob)
    {
      cerr << "convertCorbaCorbaSequence" << endl;
      DynamicAny::DynAny_var dynany= getSALOMERuntime()->getDynFactory()->create_dyn_any(*ob);
      DynamicAny::DynSequence_var ds=DynamicAny::DynSequence::_narrow(dynany);
      DynamicAny::AnySeq_var as=ds->get_elements();
      int length=as->length();
      CORBA::TypeCode_var tc_content;
      DynamicAny::AnySeq asout ;
      asout.length(length);
      for(int i=0;i<length;i++)
	{
	  CORBA::Any *a=convertCorbaCorba(t->content_type(),&as[i]);
	  //ici on fait une copie. Peut-on l'éviter ?
	  asout[i]=*a;
	  tc_content=a->type();
	  //delete a;
	}
      CORBA::TypeCode_var tc= getSALOMERuntime()->getOrb()->create_sequence_tc(0,tc_content);
      DynamicAny::DynAny_var dynanyout=getSALOMERuntime()->getDynFactory()->create_dyn_any_from_type_code(tc);
      DynamicAny::DynSequence_var dsout=DynamicAny::DynSequence::_narrow(dynanyout);
      try
	{
	  dsout->set_elements(asout);
	}
      catch(DynamicAny::DynAny::TypeMismatch& ex)
	{
	  throw YACS::Exception("type mismatch");
	}
      catch(DynamicAny::DynAny::InvalidValue& ex)
	{
	  throw YACS::Exception("invalid value");
	}
      CORBA::Any *any=dsout->to_any();
      return any;
    }


    convertCorbaCorbaFn convertCorbaCorbaFns[]=
      {
	convertCorbaCorbaNull,
	convertCorbaCorbaDouble,
	convertCorbaCorbaInt,
	convertCorbaCorbaString,
	convertCorbaCorbaNull,
	convertCorbaCorbaObjref,
	convertCorbaCorbaSequence,
      };

    CORBA::Any *convertCorbaCorba(TypeCode *t,CORBA::Any *ob)
    {
      int tk=t->kind();
      return convertCorbaCorbaFns[tk](t,ob);
    }

    /*
     * Fin des fonctions de conversion CORBA::Any * -> PyObject *
     */

    /*
     * Fonctions de conversion Xml char * -> CORBA::Any *
     */

    //Le TypeCode t est celui vers lequel on convertit (celui de l'InputPort)
    
    CORBA::Any *convertCorbaXmlNull(TypeCode *t, xmlDocPtr doc, xmlNodePtr cur)
    {
      stringstream msg;
      msg << "Conversion not implemented: kind= " << t->kind() ;
      msg << " : " << __FILE__ << ":" << __LINE__;
      throw YACS::Exception(msg.str());
    }
    
    CORBA::Any *convertCorbaXmlDouble(TypeCode *t,xmlDocPtr doc,xmlNodePtr cur)
    {
      cur = cur->xmlChildrenNode;
      while (cur != NULL)
	{
	  if ((!xmlStrcmp(cur->name, (const xmlChar *)"double")))
	    {
	      //on attend un double, on a bien un double
	      xmlChar * s = NULL;
	      s = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
	      CORBA::Any *any = new CORBA::Any();
	      cerr << "convertCorbaXmlDouble " << (const char *)s << endl;
	      *any <<= (CORBA::Double)atof((const char *)s);
	      xmlFree(s);
	      return any;
	    }
	  else if ((!xmlStrcmp(cur->name, (const xmlChar *)"int")))
	    {
	      //on attend un double, on a un int
	      xmlChar * s = NULL;
	      s = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
	      CORBA::Any *any = new CORBA::Any();
	      cerr << "convertCorbaXmlDouble " << (const char *)s << endl;
	      *any <<= (CORBA::Double)atof((const char *)s);
	      xmlFree(s);
	      return any;
	    }
	  cur = cur->next;
	}
      stringstream msg;
      msg << "Problem in conversion: kind= " << t->kind() ;
      msg << " : " << __FILE__ << ":" << __LINE__;
      throw YACS::Exception(msg.str());
    }
    
    CORBA::Any *convertCorbaXmlSequence(TypeCode *t,xmlDocPtr doc,xmlNodePtr cur)
    {
      CORBA::TypeCode_var tc_content;
      DynamicAny::AnySeq as ;
      int len=0;
      cur = cur->xmlChildrenNode;
      while (cur != NULL)
	{
	  if ((!xmlStrcmp(cur->name, (const xmlChar *)"array"))) 
	    {
	      cerr << "parse sequence " << endl;
	      xmlNodePtr cur1=cur->xmlChildrenNode;
	      while (cur1 != NULL)
		{
		  if ((!xmlStrcmp(cur1->name, (const xmlChar *)"data")))
		    {
		      cerr << "parse data " << endl;
		      xmlNodePtr cur2=cur1->xmlChildrenNode;
                    while (cur2 != NULL)
		      {
                        //on recupere toutes les values
                        if ((!xmlStrcmp(cur2->name, (const xmlChar *)"value")))
			  {
                            cerr << "parse value " << endl;
                            CORBA::Any *a=convertCorbaXml(t->content_type(),doc,cur2);
                            as.length(len+1);
                            as[len++]=*a;
                            tc_content=a->type();
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

      CORBA::TypeCode_var tc=getSALOMERuntime()->getOrb()->create_sequence_tc(0,tc_content);
      DynamicAny::DynAny_var dynanyout=getSALOMERuntime()->getDynFactory()->create_dyn_any_from_type_code(tc);
      DynamicAny::DynSequence_var dsout=DynamicAny::DynSequence::_narrow(dynanyout);
    try
      {
        dsout->set_elements(as);
      }
    catch(DynamicAny::DynAny::TypeMismatch& ex)
      {
        throw YACS::Exception("type mismatch");
      }
    catch(DynamicAny::DynAny::InvalidValue& ex)
      {
        throw YACS::Exception("invalid value");
      }
    CORBA::Any *any=dsout->to_any();
    return any;
    }

    convertCorbaXmlFn convertCorbaXmlFns[]=
      {
	convertCorbaXmlNull,
	convertCorbaXmlDouble,
	convertCorbaXmlNull,
	convertCorbaXmlNull,
	convertCorbaXmlNull,
	convertCorbaXmlNull,
	convertCorbaXmlSequence,
      };

    CORBA::Any *convertCorbaXml(TypeCode *t,xmlDocPtr doc,xmlNodePtr cur)
    {
      int tk=t->kind();
      return convertCorbaXmlFns[tk](t,doc,cur);
    }

    /*
     * Fin des fonctions de conversion Xml char * -> CORBA::Any *
     */

  }
}
