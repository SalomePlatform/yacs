// Copyright (C) 2006-2024  CEA, EDF
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
#include <Python.h>

#include "YACSRuntimeSALOMEExport.hxx"
#include "ConnectionManager.hxx"
// rnv: avoid compilation warning on Linux : "_POSIX_C_SOURCE" and "_XOPEN_SOURCE" are redefined
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <omniORB4/CORBA.h>
#include <omniORBpy.h>
#include "Runtime.hxx"

#include <memory>
#include <string>
#include <set>

class SALOME_NamingService_Container_Abstract;

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
      static Runtime* getSingleton() { return Runtime::_singleton; }

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

#ifndef SWIG
      std::unique_ptr<SALOME_NamingService_Container_Abstract> getNS();
#endif
      // singleton creation
      static void setRuntime(long flags = UsePython+UseCorba+UseXml+UseCpp+UseSalome,
                             int argc = 0, char* argv[] = NULL);
      
      friend RuntimeSALOME* getSALOMERuntime();
      
      virtual std::string getVersion() const;

      virtual void init(long flags, int argc, char* argv[]);
      virtual void fini();
      PyObject *launchSubProcess(const std::vector<std::string>& cmds);
      virtual std::vector< std::pair<std::string,int> > getCatalogOfComputeNodes() const;
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
#ifndef SWIG
      virtual Container *createContainer(const std::string& kind="");
#endif
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
                               TypeCode * type,bool init=false);

      virtual InputPort* adapt(InPropertyPort* source,
                               const std::string& impl,
                               TypeCode * type,bool init=false);

      virtual InputPort* adaptNeutral(InputPort* source,
                                      const std::string& impl,
                                      TypeCode * type,bool init);

      virtual InputPort* adapt(InputCorbaPort* source,
                               const std::string& impl,
                               TypeCode * type,bool init);

      virtual InputPort* adaptCorbaToCorba(InputCorbaPort* source,
                                           TypeCode * type);
  
      virtual InputPort* adaptCorbaToNeutral(InputCorbaPort* source,
                                             TypeCode * type);

      virtual InputPort* adaptCorbaToPython(InputCorbaPort* source,
                                            TypeCode * type);

      virtual InputPort* adaptCorbaToCpp(InputCorbaPort* source,
                                         TypeCode * type);

      virtual InputPort* adaptCorbaToXml(InputCorbaPort* source,
                                         TypeCode * type);

      virtual InputPort* adapt(InputPyPort* source,
                               const std::string& impl,
                               TypeCode * type,bool init);

      virtual InputPort* adaptPythonToCorba(InputPyPort* source,
                                            TypeCode * type);
      
      virtual InputPort* adaptPythonToNeutral(InputPyPort* source,
                                              TypeCode * type);

      virtual InputPort* adaptPythonToPython(InputPyPort* source,
                                             TypeCode * type,bool init);
      
      virtual InputPort* adaptPythonToXml(InputPyPort* source,
                                          TypeCode * type);

      virtual InputPort* adaptPythonToCpp(InputPyPort* source,
                                          TypeCode * type);

      virtual InputPort* adapt(InputCppPort* source,
                               const std::string& impl,
                               TypeCode * type,bool init);
                               
      virtual InputPort* adaptCppToCorba(InputCppPort* source,
                                            TypeCode * type);
      
      virtual InputPort* adaptCppToNeutral(InputCppPort* source,
                                              TypeCode * type);

      virtual InputPort* adaptCppToPython(InputCppPort* source,
                                             TypeCode * type);
      
      virtual InputPort* adaptCppToXml(InputCppPort* source,
                                          TypeCode * type);

      virtual InputPort* adaptCppToCpp(InputCppPort* source,
                                          TypeCode * type);

      virtual InputPort* adapt(InputXmlPort* source,
                               const std::string& impl,
                               TypeCode * type,bool init);

      virtual InputPort* adaptXmlToCorba(InputXmlPort* source,
                                         TypeCode * type);

      virtual InputPort* adaptXmlToPython(InputXmlPort* inport,
                                          TypeCode * type);

      virtual InputPort* adaptXmlToCpp(InputXmlPort* inport,
                                          TypeCode * type);

      virtual InputPort* adaptXmlToNeutral(InputXmlPort* inport,
                                          TypeCode * type);
      virtual InputPort* adaptXmlToXml(InputXmlPort* inport,
                                TypeCode * type,bool init);


      virtual InputPort* adaptNeutralToXml(InputPort* inport,
                                           TypeCode * type);

      virtual InputPort* adaptNeutralToPython(InputPort* inport,
                                              TypeCode * type);

      virtual InputPort* adaptNeutralToCorba(InputPort* inport,
                                             TypeCode * type);

      virtual InputPort* adaptNeutralToCpp(InputPort* inport,
                                             TypeCode * type);

      virtual void* convertNeutral(TypeCode * type, Any *data);
      virtual std::string convertNeutralAsString(TypeCode * type, Any *data);
      virtual std::string convertPyObjectToString(PyObject* ob);
      virtual PyObject* convertStringToPyObject(const std::string& s);

      virtual ~RuntimeSALOME(); 

      void loadModulCatalog();

      CORBA::ORB_ptr getOrb() const;
      CORBA::Object_var getFromNS(const char *entry) const;
      PyObject * getPyOrb() const;
      PyObject * getBuiltins() const;
      DynamicAny::DynAnyFactory_ptr getDynFactory() const;
      omniORBpyAPI* getApi();
      PyObject * get_omnipy();
      ConnectionManager& getConnectionManager(){return _connectionManager;}

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

    private:
      ConnectionManager _connectionManager;
    };
  }
}

#endif
