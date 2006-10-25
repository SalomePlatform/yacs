
#ifndef _RUNTIMESALOME_HXX_
#define _RUNTIMESALOME_HXX_

#include "Runtime.hxx"
#include "CORBAPorts.hxx"
#include "PythonPorts.hxx"
#include "XMLPorts.hxx"
#include "CORBAXMLConv.hxx"

#include<string>
#include<set>

namespace YACS
{
  namespace ENGINE
  {

    //--- from omniORBpy.h (not present on Debian Sarge packages)
    
    struct omniORBpyAPI
    {
      
      PyObject* (*cxxObjRefToPyObjRef)(const CORBA::Object_ptr cxx_obj,
				       CORBA::Boolean hold_lock);
      // Convert a C++ object reference to a Python object reference.
      // If <hold_lock> is true, caller holds the Python interpreter lock.
      
      CORBA::Object_ptr (*pyObjRefToCxxObjRef)(PyObject* py_obj,
					       CORBA::Boolean hold_lock);
      // Convert a Python object reference to a C++ object reference.
      // Raises BAD_PARAM if the Python object is not an object reference.
      // If <hold_lock> is true, caller holds the Python interpreter lock.
      
      
      omniORBpyAPI();
      // Constructor for the singleton. Sets up the function pointers.
    };
    class RuntimeSALOME;
    RuntimeSALOME* getSALOMERuntime();

    class RuntimeSALOME: public Runtime
    {
    public:
      
      static void setRuntime(); // singleton creation
      
      friend RuntimeSALOME* getSALOMERuntime();

      virtual void init();
      virtual void fini();

      virtual InputPort* createInputPort(const std::string& name,
					 const std::string& impl,
					 Node * node,
					 TypeCode * type);

      virtual OutputPort* createOutputPort(const std::string& name,
					   const std::string& impl,
					   Node * node,
					   TypeCode * type);

      virtual ElementaryNode* createNode(std::string implementation,
					 std::string name ) throw(Exception);

      virtual InputPort* adapt(const std::string& imp_source,
			       InputPort* source,
			       const std::string& impl,
			       TypeCode * type) throw (ConversionException);

      virtual InputPort* adapt(InputCorbaPort* source,
			       const std::string& impl,
			       TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptCorbaToCorba(InputCorbaPort* source,
					   TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptCorbaToPython(InputCorbaPort* source,
					    TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptCorbaToCpp(InputCorbaPort* source,
					 TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptCorbaToXml(InputCorbaPort* source,
					 TypeCode * type) throw (ConversionException);

      virtual InputPort* adapt(InputPyPort* source,
			       const std::string& impl,
			       TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptPythonToCorba(InputPyPort* source,
					    TypeCode * type) throw (ConversionException);
      
      virtual InputPort* adaptPythonToPython(InputPyPort* source,
					     TypeCode * type) throw (ConversionException);
      
      virtual InputPort* adaptPythonToCpp(InputPyPort* source,
					  TypeCode * type) throw (ConversionException);

      virtual InputPort* adapt(InputXmlPort* source,
			       const std::string& impl,
			       TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptXmlToCorba(InputXmlPort* source,
					 TypeCode * type) throw (ConversionException);

      CORBA::ORB_ptr getOrb();
      PyObject * getPyOrb();
      PyObject * getBuiltins();
      DynamicAny::DynAnyFactory_ptr getDynFactory();
      omniORBpyAPI* getApi();

    protected:
      RuntimeSALOME();  // singleton
      CORBA::ORB_var _orb;
      PyObject * _pyorb;
      PyObject * _bltins;
      DynamicAny::DynAnyFactory_var _dynFactory;
      omniORBpyAPI* _api;
    };
  }
}

#endif
