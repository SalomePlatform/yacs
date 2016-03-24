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

#ifndef _RUNTIMESALOME_HXX_
#define _RUNTIMESALOME_HXX_

#include "YACSRuntimeSALOMEExport.hxx"

// rnv: avoid compilation warning on Linux : "_POSIX_C_SOURCE" and "_XOPEN_SOURCE" are redefined
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

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

      PyObject* (*handleCxxSystemException)(const CORBA::SystemException& ex);
      // Sets the Python exception state to reflect the given C++ system
      // exception. Always returns NULL. The caller must hold the Python
      // interpreter lock.
};
//--- end of from omniORBpy.h (not present on Debian Sarge packages)
      
namespace YACS
{
  namespace ENGINE
  {

    class RuntimeSALOME;
    YACSRUNTIMESALOME_EXPORT RuntimeSALOME* getSALOMERuntime();

    class InputCorbaPort;
    class InputPyPort;
    class InputXmlPort;
    class InputCppPort;

    class YACSRUNTIMESALOME_EXPORT RuntimeSALOME: public Runtime
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

      // singleton creation
      static void setRuntime(long flags = UsePython+UseCorba+UseXml+UseCpp+UseSalome,
                             int argc = 0, char* argv[] = NULL);
      
      friend RuntimeSALOME* getSALOMERuntime();
      
      virtual std::string getVersion() const;

      virtual void init(long flags, int argc, char* argv[]);
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
      virtual OptimizerLoop* createOptimizerLoop(const std::string& name,const std::string& algLib,
                                                 const std::string& factoryName,bool algInitOnFile,
                                                 const std::string& kind="", Proc * procForTypes = NULL);
      virtual Bloc* createBloc(const std::string& name);
      virtual Proc* createProc(const std::string& name);

      virtual TypeCode * createInterfaceTc(const std::string& id, const std::string& name,
                                            std::list<TypeCodeObjref *> ltc);
      virtual TypeCode * createSequenceTc(const std::string& id, const std::string& name, TypeCode *content);
      virtual TypeCodeStruct * createStructTc(const std::string& id, const std::string& name);

      virtual InputPort* adapt(InputPort* source,
                               const std::string& impl,
                               TypeCode * type,bool init=false) throw (ConversionException);

      virtual InputPort* adapt(InPropertyPort* source,
                               const std::string& impl,
                               TypeCode * type,bool init=false) throw (ConversionException);

      virtual InputPort* adaptNeutral(InputPort* source,
                                      const std::string& impl,
                                      TypeCode * type,bool init) throw (ConversionException);

      virtual InputPort* adapt(InputCorbaPort* source,
                               const std::string& impl,
                               TypeCode * type,bool init) throw (ConversionException);

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
                               TypeCode * type,bool init) throw (ConversionException);

      virtual InputPort* adaptPythonToCorba(InputPyPort* source,
                                            TypeCode * type) throw (ConversionException);
      
      virtual InputPort* adaptPythonToNeutral(InputPyPort* source,
                                              TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptPythonToPython(InputPyPort* source,
                                             TypeCode * type,bool init) throw (ConversionException);
      
      virtual InputPort* adaptPythonToXml(InputPyPort* source,
                                          TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptPythonToCpp(InputPyPort* source,
                                          TypeCode * type) throw (ConversionException);

      virtual InputPort* adapt(InputCppPort* source,
                               const std::string& impl,
                               TypeCode * type,bool init) throw (ConversionException);
                               
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
                               TypeCode * type,bool init) throw (ConversionException);

      virtual InputPort* adaptXmlToCorba(InputXmlPort* source,
                                         TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptXmlToPython(InputXmlPort* inport,
                                          TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptXmlToCpp(InputXmlPort* inport,
                                          TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptXmlToNeutral(InputXmlPort* inport,
                                          TypeCode * type) throw (ConversionException);
      virtual InputPort* adaptXmlToXml(InputXmlPort* inport,
                                TypeCode * type,bool init) throw (ConversionException);


      virtual InputPort* adaptNeutralToXml(InputPort* inport,
                                           TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptNeutralToPython(InputPort* inport,
                                              TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptNeutralToCorba(InputPort* inport,
                                             TypeCode * type) throw (ConversionException);

      virtual InputPort* adaptNeutralToCpp(InputPort* inport,
                                             TypeCode * type) throw (ConversionException);

      virtual void* convertNeutral(TypeCode * type, Any *data);
      virtual std::string convertNeutralAsString(TypeCode * type, Any *data);
      virtual std::string convertPyObjectToString(PyObject* ob);
      virtual PyObject* convertStringToPyObject(const std::string& s);

      virtual ~RuntimeSALOME(); 

      CORBA::ORB_ptr getOrb();
      PyObject * getPyOrb();
      PyObject * getBuiltins();
      DynamicAny::DynAnyFactory_ptr getDynFactory();
      omniORBpyAPI* getApi();
      PyObject * get_omnipy();

    protected:
      RuntimeSALOME();  // singleton
      RuntimeSALOME(long flags, int argc, char* argv[]);  // singleton
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
