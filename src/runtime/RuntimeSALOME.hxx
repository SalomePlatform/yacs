
#ifndef _RUNTIMESALOME_HXX_
#define _RUNTIMESALOME_HXX_

#include <Python.h>
#include <omniORB4/CORBA.h>
#include "Runtime.hxx"

#include<string>
#include<set>

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
};
//--- end of from omniORBpy.h (not present on Debian Sarge packages)
      
namespace YACS
{
  namespace ENGINE
  {

    class RuntimeSALOME;
    RuntimeSALOME* getSALOMERuntime();

    class InputCorbaPort;
    class InputPyPort;
    class InputXmlPort;
    class InputCppPort;

    class RuntimeSALOME: public Runtime
    {
    public:
      
      enum 
      {
	      IsPyExt = 1,
        UsePython = 2,
	      UseCorba = 4,
	      UseXml = 8,
	      UseCpp = 16,
	      UseSalome = 32
      } FLAGS;

      static void setRuntime(long flags = UsePython+UseCorba+UseXml+UseCpp+UseSalome); // singleton creation
      
      friend RuntimeSALOME* getSALOMERuntime();

      virtual void init(long flags);
      virtual void fini();

      virtual InputPort* createInputPort(const std::string& name,
                                         const std::string& impl,
                                         Node * node,
                                         TypeCode * type);

      virtual OutputPort* createOutputPort(const std::string& name,
                                           const std::string& impl,
                                           Node * node,
                                           TypeCode * type);
      virtual InputDataStreamPort* createInputDataStreamPort(const std::string& name,
                                                             Node *node,TypeCode *type);

      virtual OutputDataStreamPort* createOutputDataStreamPort(const std::string& name,
                                                               Node *node,TypeCode *type);

      virtual DataNode* createInDataNode(const std::string& kind,const std::string& name);
      virtual DataNode* createOutDataNode(const std::string& kind,const std::string& name);
      virtual InlineFuncNode* createFuncNode(const std::string& kind,const std::string& name);
      virtual InlineNode* createScriptNode(const std::string& kind,const std::string& name);

      virtual ServiceNode* createRefNode(const std::string& kind,const std::string& name);
      virtual ServiceNode* createCompoNode(const std::string& kind,const std::string& name);
      virtual ServiceInlineNode *createSInlineNode(const std::string& kind, const std::string& name);
      virtual ComponentInstance* createComponentInstance(const std::string& name,
                                                         const std::string& kind="");
      virtual Container *createContainer(const std::string& kind="");
      virtual WhileLoop* createWhileLoop(const std::string& name);
      virtual ForLoop* createForLoop(const std::string& name);
      virtual Bloc* createBloc(const std::string& name);
      virtual Proc* createProc(const std::string& name);

      virtual InputPort* adapt(InputPort* source,
                               const std::string& impl,
                               TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptNeutral(InputPort* source,
                                      const std::string& impl,
                                      TypeCode * type) throw (ConversionException);

      virtual InputPort* adapt(InputCorbaPort* source,
                               const std::string& impl,
                               TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptCorbaToCorba(InputCorbaPort* source,
                                           TypeCode * type) throw (ConversionException);
  
      virtual InputPort* adaptCorbaToNeutral(InputCorbaPort* source,
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
      
      virtual InputPort* adaptPythonToNeutral(InputPyPort* source,
                                              TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptPythonToPython(InputPyPort* source,
                                             TypeCode * type) throw (ConversionException);
      
      virtual InputPort* adaptPythonToXml(InputPyPort* source,
                                          TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptPythonToCpp(InputPyPort* source,
                                          TypeCode * type) throw (ConversionException);

      virtual InputPort* adapt(InputCppPort* source,
                               const std::string& impl,
                               TypeCode * type) throw (ConversionException);
                               
      virtual InputPort* adaptCppToCorba(InputCppPort* source,
                                            TypeCode * type) throw (ConversionException);
      
      virtual InputPort* adaptCppToNeutral(InputCppPort* source,
                                              TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptCppToPython(InputCppPort* source,
                                             TypeCode * type) throw (ConversionException);
      
      virtual InputPort* adaptCppToXml(InputCppPort* source,
                                          TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptCppToCpp(InputCppPort* source,
                                          TypeCode * type) throw (ConversionException);

      virtual InputPort* adapt(InputXmlPort* source,
                               const std::string& impl,
                               TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptXmlToCorba(InputXmlPort* source,
                                         TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptXmlToPython(InputXmlPort* inport,
                                          TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptXmlToCpp(InputXmlPort* inport,
                                          TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptXmlToNeutral(InputXmlPort* inport,
                                          TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptNeutralToXml(InputPort* inport,
                                           TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptNeutralToPython(InputPort* inport,
                                              TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptNeutralToCorba(InputPort* inport,
                                             TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptNeutralToCpp(InputPort* inport,
                                             TypeCode * type) throw (ConversionException);
      virtual ~RuntimeSALOME(); 

      CORBA::ORB_ptr getOrb();
      PyObject * getPyOrb();
      PyObject * getBuiltins();
      DynamicAny::DynAnyFactory_ptr getDynFactory();
      omniORBpyAPI* getApi();
      PyObject * get_omnipy();

    protected:
      RuntimeSALOME();  // singleton
      RuntimeSALOME(long flags);  // singleton
      void initBuiltins();
      CORBA::ORB_var _orb;
      PyObject * _pyorb;
      PyObject * _bltins;
      DynamicAny::DynAnyFactory_var _dynFactory;
      omniORBpyAPI* _api;
      PyObject* _omnipy;
      long _flags;
      bool _usePython, _useCorba, _useCpp, _useXml;

    };
  }
}

#endif
