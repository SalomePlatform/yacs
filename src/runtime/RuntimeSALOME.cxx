
#include "RuntimeSALOME.hxx"
#include "PythonNode.hxx"
#include "CORBANode.hxx"
#include "XMLNode.hxx"
#include "CppNode.hxx"
#include "TypeConversions.hxx"
#include "CORBACORBAConv.hxx"
#include "PythonCORBAConv.hxx"
#include "CORBAPythonConv.hxx"
#include "XMLCORBAConv.hxx"

#include <omniORB4/CORBA.h>
#include <iostream>
#include <sstream>
#include <cassert>

using namespace std;
using namespace YACS::ENGINE;



void RuntimeSALOME::setRuntime() // singleton creation (not thread safe!)
{
  if (! Runtime::_singleton) Runtime::_singleton = new RuntimeSALOME();
}

RuntimeSALOME* YACS::ENGINE::getSALOMERuntime()
{
  assert(Runtime::_singleton);
  return dynamic_cast< RuntimeSALOME* >(Runtime::_singleton);
}

/**
 *  Singleton creation, initialize converter map
 */
  
RuntimeSALOME::RuntimeSALOME()
{
  _setOfImplementation.insert("Cpp");
  _setOfImplementation.insert("Python");
  _setOfImplementation.insert("CORBA");
  init();
}


void RuntimeSALOME::init()
{
  int nbargs = 0; char **args = 0;
  _orb = CORBA::ORB_init (nbargs, args);
  CORBA::Object_var obj = _orb->resolve_initial_references("DynAnyFactory");
  _dynFactory = DynamicAny::DynAnyFactory::_narrow(obj);

  PyObject *mainmod ;
  cerr << "RuntimeSALOME::init" << endl;
  Py_Initialize();

  mainmod = PyImport_AddModule("__main__");
  PyObject *globals;
  globals = PyModule_GetDict(mainmod);

  /* globals is a borrowed reference */
  Py_INCREF(globals);
  /* globals is a new reference */
  
  _bltins = PyEval_GetBuiltins();  /* borrowed ref */
  
  //init section
  PyObject* omnipy = PyImport_ImportModule((char*)"_omnipy");
  if (!omnipy)
    {
      PyErr_SetString(PyExc_ImportError, (char*)"Cannot import _omnipy");
      return;
    }
  PyObject* pyapi = PyObject_GetAttrString(omnipy, (char*)"API");
  _api = (omniORBpyAPI*)PyCObject_AsVoidPtr(pyapi);
  Py_DECREF(pyapi);
  PyObject *res=PyRun_String("\n"
			     "import sys\n"
			     "sys.path.insert(0,'.')\n"
			     "import CORBA\n"
			     "from omniORB import any\n"
			     "orb = CORBA.ORB_init([], CORBA.ORB_ID)\n"
			     "print sys.getrefcount(orb)\n"
			     "\n",
			     Py_file_input,globals,globals );
  if(res == NULL)
    {
      PyErr_Print();
      return;
    }
  Py_DECREF(res);
  _pyorb = PyDict_GetItemString(globals,"orb");
  cerr << "refcnt: " << _pyorb->ob_refcnt << endl;
  PyObject_Print(_pyorb,stdout,Py_PRINT_RAW);
  cerr << endl;
  /* pyorb is a borrowed reference */
  //Py_INCREF(pyorb); pas nécessaire

  PyObject *pyany;
  pyany = PyDict_GetItemString(globals,"any");
  cerr << "pyany refcnt: " << pyany->ob_refcnt << endl;
  /* pyany is a borrowed reference */
}


void RuntimeSALOME::fini()
{
  cerr << "RuntimeSALOME::fini" << endl;
  Py_Finalize();
}


ElementaryNode* RuntimeSALOME::createNode(string implementation,
					  string name) throw(Exception)
{
  ElementaryNode* node = 0;
  if (implementation == "Python")
    node = new PythonNode(name);
  else if (implementation == "CORBA")
    node = new CORBANode(name);
  else if (implementation == "XML")
    node = new XmlNode(name);
  else if (implementation == "Cpp")
    node = new CppNode(name);
  else 
    {
      string what ="RuntimeSALOME does not handle this implementation: " + implementation;
      throw Exception(what);
    }
  return node;
}

InputPort * RuntimeSALOME::createInputPort(const string& name,
					   const string& impl,
					   Node * node,
					   TypeCode * type)
{
  if(impl == "CPP")
    {
      throw Exception("Cannot create InputCppPort ");
    }
  else if(impl == "Python")
    {
      return new InputPyPort(name, node, type);
    }
  else if(impl == "CORBA")
    {
      return new InputCorbaPort(name, node, type);
    }
  else if(impl == "XML")
    {
      return new InputXmlPort(name, node, type);
    }
  else
    {
      stringstream msg;
      msg << "Cannot create " << impl << " InputPort" ;
      msg << " ("__FILE__ << ":" << __LINE__ << ")";
      throw Exception(msg.str());
    }
}

OutputPort * RuntimeSALOME::createOutputPort(const string& name,
					     const string& impl,
					     Node * node,
					     TypeCode * type)
{
  if(impl == "CPP")
    {
      throw Exception("Cannot create OutputCppPort ");
    }
  else if(impl == "Python")
    {
      return new OutputPyPort(name, node, type);
    }
  else if(impl == "CORBA")
    {
      return new OutputCorbaPort(name, node, type);
    }
  else if(impl == "XML")
    {
      return new OutputXmlPort(name, node, type);
    }
  else
    {
      stringstream msg;
      msg << "Cannot create " << impl << " OutputPort" ;
      msg << " ("__FILE__ << ":" << __LINE__ << ")";
      throw Exception(msg.str());
    }
}

InputPort* RuntimeSALOME::adapt(const string& imp_source,
				InputPort* source,
				const string& impl,
				TypeCode * type) throw (ConversionException)
{
  cerr<<"RuntimeSALOME::adapt(InputPort* source" << endl;
  if(imp_source == "Python")
    {
      return adapt((InputPyPort*)source,impl,type);
    }
  else if(imp_source == "CORBA")
    {
      return adapt((InputCorbaPort*)source,impl,type);
    }
  else if(imp_source == "XML")
    {
      return adapt((InputXmlPort*)source,impl,type);
    }
  else
    {
      stringstream msg;
      msg << "Cannot adapt " << imp_source << " InputPort to " << impl;
      msg << " ("__FILE__ << ":" << __LINE__ << ")";
      throw ConversionException(msg.str());
    }
}

//! Retourne un adaptateur d'un port entrant Xml pour un port sortant dont l'implémentation est donnée par impl
/*!
 *   \param source : input port to adapt to implementation impl and type type
 *   \param impl : output port implementation (C++, Python or Corba)
 *   \param type : le type supporté par le port sortant
 *   \return input port adapté à l'implémentation
 */

InputPort* RuntimeSALOME::adapt(InputXmlPort* source,
				const string& impl,
				TypeCode * type) throw (ConversionException)
{
  cerr<<"RuntimeSALOME::adapt(InputXmlPort* source" << endl;
  if(impl == "CORBA")
    {
      return adaptXmlToCorba(source,type);
    }
  else
    {
      stringstream msg;
      msg << "Cannot connect InputXmlPort to " << impl << " implementation";
      msg << " ("__FILE__ << ":" << __LINE__ << ")";
      throw ConversionException(msg.str());
    }
}

//! Retourne un adaptateur d'un port entrant XML pour un port sortant CORBA
/*!
 *   \param inport : input port to adapt to CORBA type type
 *   \param type : le type supporté par le port sortant
 *   \return a InputCorbaPort port
 */

InputPort* RuntimeSALOME::adaptXmlToCorba(InputXmlPort* inport,
					  TypeCode * type) throw (ConversionException)
{
  cerr <<"RuntimeSALOME::adaptXmlToCorba(InputXmlPort* inport" << endl;
  if(isAdaptableXmlCorba(type,inport->type()))
    {
      //les types sont convertibles
      return new CorbaXml(inport);
    }
  //les types sont non convertibles
  stringstream msg;
  msg << "Cannot connect InputXmlPort to Corba output port " ;
  msg << type->id() << " != " << inport->type()->id();
  msg << " ("__FILE__ << ":" << __LINE__ << ")";
  throw ConversionException(msg.str());
}

//! Retourne un adaptateur d'un port entrant CORBA pour un port sortant Xml
/*!
 *   \param inport : input port to adapt to Xml type type
 *   \param type : le type supporté par le port sortant
 *   \return an input port of Python type InputXmlPort
 */

InputPort* RuntimeSALOME::adaptCorbaToXml(InputCorbaPort* inport,
					  TypeCode * type) throw (ConversionException)
{
  //ATTENTION : on utilise isAdaptableCorbaPyObject (meme fonction)
  cerr << "RuntimeSALOME::adaptCorbaToXml(InputCorbaPort* inport" << endl;
  if(isAdaptableCorbaPyObject(type,inport->type()))
    {
      //les types sont convertibles
      return new XmlCorba(inport);
    }
  //les types sont non convertibles
  stringstream msg;
  msg << "Cannot connect InputCorbaPort with OutputXmlPort : " ;
  msg << __FILE__ << ":" <<__LINE__;
  throw ConversionException(msg.str());
}


//! Retourne un adaptateur d'un port entrant CORBA pour un port sortant CORBA
/*!
 *   \param inport : input port to adapt to CORBA type type
 *   \param type : le type supporté par le port sortant
 */

InputPort* RuntimeSALOME::adaptCorbaToCorba(InputCorbaPort* inport,
					    TypeCode * type) throw (ConversionException)
{
  if(type->is_a(inport->type()))
    {
      //les types sont compatibles : pas de conversion
      return inport;
    }
  else if(isAdaptableCorbaCorba(type,inport->type()))
    {
      //les types sont convertibles
      return new CorbaCorba(inport);
    }
  //les types sont non convertibles
  stringstream msg;
  msg << "Cannot connect 2 CorbaPort with non convertible types: " ;
  msg << type->id() << " != " << inport->type()->id();
  throw ConversionException(msg.str());
}

//! Retourne un adaptateur d'un port entrant CORBA pour un port sortant Python
/*!
 *   \param inport : input port to adapt to Python type type
 *   \param type : le type supporté par le port sortant
 *   \return an input port of Python type InputPyPort
 */

InputPort* RuntimeSALOME::adaptCorbaToPython(InputCorbaPort* inport,
					     TypeCode * type) throw (ConversionException)
{
  if(inport->type()->kind() == Double)
    {
      if(isAdaptableCorbaPyObject(type,inport->type()))return new PyCorbaDouble(inport);
    }
  else if(inport->type()->kind() == Int)
    {
      if(isAdaptableCorbaPyObject(type,inport->type()))return new PyCorbaInt(inport);
    }
  else if(inport->type()->kind() == String)
    {
      if(isAdaptableCorbaPyObject(type,inport->type()))return new PyCorbaString(inport);
    }
  else if(inport->type()->kind() == Objref )
    {
      if(isAdaptableCorbaPyObject(type,inport->type()))
	{
	  return new PyCorbaObjref(inport);
        }
      else
	{
	  stringstream msg;
	  msg << "Cannot connect InputPyPort : incompatible objref types ";
	  msg << __FILE__ << ":" <<__LINE__;
	  throw ConversionException(msg.str());
        }
    }
  else if(inport->type()->kind() == Sequence)
    {
      if(isAdaptableCorbaPyObject(type,inport->type()))
	{
	  return new PyCorbaSequence(inport);
        }
      else
	{
	  stringstream msg;
	  msg << "Cannot convert this sequence type " ;
	  msg << __FILE__ << ":" <<__LINE__;
	  throw ConversionException(msg.str());
        }
    }
  // Adaptation not found
  stringstream msg;
  msg << "Cannot connect InputCorbaPort to Python output " ;
  msg << __FILE__ << ":" <<__LINE__;
  throw ConversionException(msg.str());
}

//! Retourne un adaptateur d'un port entrant CORBA pour un port sortant C++
/*!
 *   \param inport : input port to adapt to C++ type type
 *   \param type : le type supporté par le port sortant
 */

InputPort* RuntimeSALOME::adaptCorbaToCpp(InputCorbaPort* inport,
					  TypeCode * type) throw (ConversionException)
{
  throw ConversionException("Cannot connect InputCorbaPort to C++ ");
}

//! Retourne un adaptateur d'un port entrant CORBA pour un port sortant dont l'implémentation est donnée par impl
/*!
 *   \param source : input port to adapt to implementation impl and type type
 *   \param impl : output port implementation (C++, Python or Corba)
 *   \param type : le type supporté par le port sortant
 */

InputPort* RuntimeSALOME::adapt(InputCorbaPort* source,
				const string& impl,
				TypeCode * type) throw (ConversionException)
{
  cerr<<"RuntimeSALOME::adapt(InputPyPort* source" << endl;
  if(impl == "CPP")
    {
      return adaptCorbaToCpp(source,type);
    }
  else if(impl == "Python")
    {
      return adaptCorbaToPython(source,type);
    }
  else if(impl == "CORBA")
    {
      return adaptCorbaToCorba(source,type);
    }
  else if(impl == "XML")
    {
      return adaptCorbaToXml(source,type);
    }
   else
    {
      stringstream msg;
      msg << "Cannot connect InputCorbaPort : unknown implementation " ;
      msg << __FILE__ << ":" <<__LINE__;
      throw ConversionException(msg.str());
    }
  return source;
}

//! Retourne un adaptateur d'un port entrant Python pour un port sortant Python
/*!
 * Dans ce cas, on ne fait pas de conversion ni de cast (int->double, par ex).
 * On vérifie simplement que la connexion est autorisée.
 *   \param inport : InputPort to adapt to Python type type
 *   \param type : le TypeCode supporté par le port sortant
 *   \return       InputPort de type Python (InputPyPort)
 */

InputPort* RuntimeSALOME::adaptPythonToPython(InputPyPort* inport,
					      TypeCode * type) throw (ConversionException)
{
  if(isAdaptablePyObjectPyObject(type,inport->type()))
    {
      //les types sont convertibles
      //En Python, il n'est pas nécessaire de convertir. La conversion
      //sera faite à la volée dans l'interpréteur
      return inport;
    }
  //les types sont non convertibles
  stringstream msg;
  msg << "Cannot connect 2 Python Port with non convertible types: " ;
  msg << type->id() << " != " << inport->type()->id();
  throw ConversionException(msg.str());
}

//! Retourne un adaptateur d'un port entrant Python pour un port sortant C++
/*!
 * Pas encore implémenté
 *   \param inport : InputPort to adapt to C++ type type
 *   \param type : le TypeCode supporté par le port sortant
 *   \return       InputPort de type C++ (InputCppPort)
 */

InputPort* RuntimeSALOME::adaptPythonToCpp(InputPyPort* inport,
					   TypeCode * type) throw (ConversionException)
{
  throw ConversionException("Cannot connect InputPyPort to C++ ");
}

//! Retourne un adaptateur d'un port entrant Python pour un port sortant Corba
/*!
 * On convertit dans tous les cas
 *   \param inport : InputPort to adapt to Corba type type
 *   \param type : le TypeCode supporté par le port sortant
 *   \return       InputPort de type Corba (InputCorbaPort)
 */

InputPort* RuntimeSALOME::adaptPythonToCorba(InputPyPort* inport,
					     TypeCode * type) throw (ConversionException)
{
  cerr << "RuntimeSALOME::adaptPythonToCorba:" ;
  cerr << inport->type()->kind() << ":" << type->kind()<< endl;

  if(inport->type()->kind() == Double)
    {
      if(isAdaptablePyObjectCorba(type,inport->type()))return new CorbaPyDouble(inport);
    }
  else if(inport->type()->kind() == Int)
    {
      if(isAdaptablePyObjectCorba(type,inport->type()))return new CorbaPyInt(inport);
    }
  else if(inport->type()->kind() == String)
    {
      if(isAdaptablePyObjectCorba(type,inport->type()))return new CorbaPyString(inport);
    }
  else if(inport->type()->kind() == Objref)
    {
      if(isAdaptablePyObjectCorba(type,inport->type()))
	{
	  return new CorbaPyObjref(inport);
        }
      else
	{
	  stringstream msg;
	  msg << "Cannot connect InputCorbaPort : incompatible objref types ";
	  msg << __FILE__ << ":" <<__LINE__;
	  throw ConversionException(msg.str());
        }
    }
  else if(inport->type()->kind() == Sequence)
    {
      if(isAdaptablePyObjectCorba(type,inport->type()))
	{
	  return new CorbaPySequence(inport);
        }
      else
	{
	  stringstream msg;
	  msg << "Cannot convert this sequence type " ;
	  msg << __FILE__ << ":" <<__LINE__;
	  throw ConversionException(msg.str());
        }
    }
  // Adaptation not found
  stringstream msg;
  msg << "Cannot connect InputPyPort to Corba output " ;
  msg <<  __FILE__ << ":" << __LINE__;
  throw ConversionException(msg.str());
}

//! Retourne un adaptateur d'un port entrant Python pour un port sortant dont l'implémentation est donnée par impl
/*!
 *   \param source : input port to adapt to implementation impl and type type
 *   \param impl : output port implementation (C++, Python or Corba)
 *   \param type : le type supporté par le port sortant
 *   \return input port adapté à l'implémentation
 */

InputPort* RuntimeSALOME::adapt(InputPyPort* source,
				const string& impl,
				TypeCode * type) throw (ConversionException)
{
  cerr<<"RuntimeSALOME::adapt(InputPyPort* source" << endl;
  if(impl == "CPP")
    {
      return adaptPythonToCpp(source,type);
    }
  else if(impl == "Python")
    {
      return adaptPythonToPython(source,type);
    }
  else if(impl == "CORBA")
    {
      return adaptPythonToCorba(source,type);
    }
  else
    {
      throw ConversionException("Cannot connect InputPyPort : unknown implementation  ");
    }
}

// bool RuntimeSALOME::isCompatible(const OutputPort* outputPort, 
// 				 const InputPort*  inputPort)
// {
//   bool result=true;
//   return result;
// }

CORBA::ORB_ptr RuntimeSALOME::getOrb()
{
  return _orb;
}

PyObject * RuntimeSALOME::getPyOrb()
{
  return _pyorb;
}

PyObject * RuntimeSALOME::getBuiltins()
{
  return _bltins;
}

DynamicAny::DynAnyFactory_ptr RuntimeSALOME::getDynFactory()
{
  return _dynFactory;
}

omniORBpyAPI* RuntimeSALOME::getApi()
{
  return _api;
}

