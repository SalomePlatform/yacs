//#define REFCNT
#ifdef REFCNT
#define private public
#define protected public
#include <omniORB4/CORBA.h>
#include <omniORB4/internal/typecode.h>
#include <omniORB4/internal/corbaOrb.h>
#endif

#include "yacsconfig.h"
#include "RuntimeSALOME.hxx"
#include "SALOMEDispatcher.hxx"
#include "Proc.hxx"
#include "WhileLoop.hxx"
#include "ForLoop.hxx"
#include "Bloc.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "PresetPorts.hxx"
#include "InputDataStreamPort.hxx"
#include "OutputDataStreamPort.hxx"
#include "SalomeProc.hxx"
//Catalog Loaders
#include "SessionCataLoader.hxx"

//Components
#include "CORBAComponent.hxx"
#include "SalomeComponent.hxx"
#include "SalomePythonComponent.hxx"
#include "CppComponent.hxx"

#include "SalomeContainer.hxx"
#include "CppContainer.hxx"

//Nodes
#include "PythonNode.hxx"
#include "CORBANode.hxx"
#include "XMLNode.hxx"
#include "CppNode.hxx"
#include "PresetNode.hxx"
#include "OutNode.hxx"
#include "SalomePythonNode.hxx"

//CORBA proxy ports
#include "CORBACORBAConv.hxx"
#include "CORBAPythonConv.hxx"
#include "CORBAXMLConv.hxx"
#include "CORBACppConv.hxx"
#include "CORBANeutralConv.hxx"

#include "TypeConversions.hxx"
//Python proxy ports
#include "PythonCORBAConv.hxx"
#include "PythonXMLConv.hxx"
#include "PythonCppConv.hxx"
#include "PythonNeutralConv.hxx"

//Neutral proxy ports
#include "NeutralCORBAConv.hxx"
#include "NeutralPythonConv.hxx"
#include "NeutralXMLConv.hxx"
#include "NeutralCppConv.hxx"

//C++ proxy ports
#include "CppCORBAConv.hxx"
#include "CppPythonConv.hxx"
#include "CppXMLConv.hxx"
#include "CppCppConv.hxx"
#include "CppNeutralConv.hxx"

//XML proxy ports
#include "XMLCORBAConv.hxx"
#include "XMLPythonConv.hxx"
#include "XMLCppConv.hxx"
#include "XMLNeutralConv.hxx"

//Calcium specific ports
#include "CalStreamPort.hxx"

#ifdef SALOME_KERNEL
#include "SALOME_NamingService.hxx"
#include "SALOME_LifeCycleCORBA.hxx"
#endif
  
#include <libxml/parser.h>
#include <omniORB4/CORBA.h>
#include <iostream>
#include <sstream>
#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;

void RuntimeSALOME::setRuntime(long flags) // singleton creation (not thread safe!)
{
  if (! Runtime::_singleton)
    {
      RuntimeSALOME* r=new RuntimeSALOME(flags);
      Runtime::_singleton = r;
      r->initBuiltins();
    }
  DEBTRACE("RuntimeSALOME::setRuntime() done !");
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
  assert(0);
}

void RuntimeSALOME::initBuiltins()
{
  //Fill the builtin catalog with nodes specific to the runtime
  std::map<std::string,TypeCode*>& typeMap=_builtinCatalog->_typeMap;
  std::map<std::string,Node*>& nodeMap=_builtinCatalog->_nodeMap;
  std::map<std::string,ComposedNode*>& composednodeMap=_builtinCatalog->_composednodeMap;
  std::map<std::string,ComponentDefinition*>& componentMap=_builtinCatalog->_componentMap;
  nodeMap["PyFunction"]=new PyFuncNode("PyFunction");
  nodeMap["PyScript"]=new PythonNode("PyScript");
  nodeMap["CORBANode"]=new CORBANode("CORBANode");
  nodeMap["XmlNode"]=new XmlNode("XmlNode");
  nodeMap["SalomeNode"]=new SalomeNode("SalomeNode");
  nodeMap["CppNode"]=new CppNode("CppNode");
  nodeMap["SalomePythonNode"]=new SalomePythonNode("SalomePythonNode");
}

RuntimeSALOME::RuntimeSALOME(long flags)
{
  // If all flags (apart the IsPyExt flags) are unset, force them to true
  if ((flags - flags & RuntimeSALOME::IsPyExt) == 0)
    flags += RuntimeSALOME::UseCorba + RuntimeSALOME::UsePython
          +  RuntimeSALOME::UseCpp + RuntimeSALOME::UseXml;

  // Salome Nodes implies Corba Nodes
  if (flags & RuntimeSALOME::UseSalome)
    flags |= RuntimeSALOME::UseCorba;

  // Corba Nodes implies Python Nodes
  if (flags & RuntimeSALOME::UseCorba)
    flags |= RuntimeSALOME::UsePython;

  _useCorba = flags & RuntimeSALOME::UseCorba;
  _usePython = flags & RuntimeSALOME::UsePython;
  _useCpp = flags & RuntimeSALOME::UseCpp;
  _useXml = flags & RuntimeSALOME::UseXml;

  /* Init libxml */
  xmlInitParser();

  if (_useCpp)    _setOfImplementation.insert(CppNode::IMPL_NAME);
  if (_usePython) _setOfImplementation.insert(PythonNode::IMPL_NAME);
  if (_useCorba)  _setOfImplementation.insert(CORBANode::IMPL_NAME);
  if (_useXml)    _setOfImplementation.insert(XmlNode::IMPL_NAME);
  init(flags);
}

RuntimeSALOME::~RuntimeSALOME()
{
  DEBTRACE("RuntimeSALOME::~RuntimeSALOME");
  // destroy catalog loader prototypes
  std::map<std::string, CatalogLoader*>::const_iterator pt;
  for(pt=_catalogLoaderFactoryMap.begin();pt!=_catalogLoaderFactoryMap.end();pt++)
    {
      delete (*pt).second;
    }
}

//! CORBA and Python initialization
/*!
 *  \param flags contains several bits
 *            bit0 (ispyext) true when method is called from Python
 *                           (Python initialization must not be done!)
 *            bit1 (UsePython) true if python nodes are needed
 *            bit1 (UseCorba)  true if CORBA nodes are needed
 *            bit1 (UseXml)    true if python nodes are needed
 *            bit1 (UseCpp)    true if C++ nodes are needed
 *            bit1 (UseSalome) true if Salome nodes are needed
 *
 */

void RuntimeSALOME::init(long flags)
{
  bool ispyext = flags & RuntimeSALOME::IsPyExt;
  if (_useCorba)
    {
      PortableServer::POA_var root_poa;
      PortableServer::POAManager_var pman;
      CORBA::Object_var obj;
      int nbargs = 0; char **args = 0;
      _orb = CORBA::ORB_init (nbargs, args);
      obj = _orb->resolve_initial_references("RootPOA");
      root_poa = PortableServer::POA::_narrow(obj);
      pman = root_poa->the_POAManager();
      pman->activate();

#ifdef REFCNT
      DEBTRACE("_orb refCount: " << ((omniOrbORB*)_orb.in())->pd_refCount);
#endif
      obj = _orb->resolve_initial_references("DynAnyFactory");
      _dynFactory = DynamicAny::DynAnyFactory::_narrow(obj);
    }

  if (_usePython)
    {
      DEBTRACE("RuntimeSALOME::init, is python extension = " << ispyext);

      // Initialize Python interpreter in embedded mode
      if (!Py_IsInitialized())
        {
          Py_InitializeEx(0); // do not install signal handlers
          PyEval_InitThreads(); /* Create (and acquire) the interpreter lock (for threads)*/
          PyEval_SaveThread(); /* Release the thread state */
          //here we do not have the Global Interpreter Lock
        }

      PyObject *mainmod,*pyapi,*res ;
      PyObject *globals;
      PyGILState_STATE gstate;
      gstate = PyGILState_Ensure(); // acquire the Global Interpreter Lock
    
      mainmod = PyImport_AddModule("__main__");
      globals = PyModule_GetDict(mainmod);
      /* globals is a borrowed reference */
  
      if (PyDict_GetItemString(globals, "__builtins__") == NULL) 
        {
          PyObject *bimod = PyImport_ImportModule("__builtin__");
          if (bimod == NULL || PyDict_SetItemString(globals, "__builtins__", bimod) != 0)
            Py_FatalError("can't add __builtins__ to __main__");
          Py_DECREF(bimod);
        }

      _bltins = PyEval_GetBuiltins();  /* borrowed ref */
  
      if (_useCorba)
        {

          //init section
          PyObject* omnipy = PyImport_ImportModule((char*)"_omnipy");
          if (!omnipy)
            {
              PyErr_Print();
              PyErr_SetString(PyExc_ImportError, (char*)"Cannot import _omnipy");
              goto out;
            }
          pyapi = PyObject_GetAttrString(omnipy, (char*)"API");
          if (!pyapi)
            {
              goto out;
            }
          _api = (omniORBpyAPI*)PyCObject_AsVoidPtr(pyapi);
          Py_DECREF(pyapi);

          res=PyRun_String("\n"
                             "import sys\n"
                             "sys.path.insert(0,'.')\n"
                             "from omniORB import CORBA\n"
                             "from omniORB import any\n"
                             "orb = CORBA.ORB_init([], CORBA.ORB_ID)\n"
                             "#print sys.getrefcount(orb)\n"
                             "\n",
                             Py_file_input,globals,globals );
          if(res == NULL)
            {
              PyErr_Print();
              goto out;
            }
          Py_DECREF(res);

          _pyorb = PyDict_GetItemString(globals,"orb");
          /* PyDict_GetItemString returns a borrowed reference. There is no need to decref _pyorb */

          PyObject *pyany;
          pyany = PyDict_GetItemString(globals,"any");
          /* PyDict_GetItemString returns a borrowed reference. There is no need to decref pyany */

#ifdef REFCNT
          DEBTRACE("_orb refCount: " << ((omniOrbORB*)_orb.in())->pd_refCount);
#endif
        }
      out:
        PyGILState_Release(gstate); // Release the Global Interpreter Lock
    }
  if (_useCorba)
    {
      // initialize the catalogLoaderFactory map with the session one
      _catalogLoaderFactoryMap["session"]=new SessionCataLoader;
    }
}

void RuntimeSALOME::fini()
{
  if (_usePython)
    {
      PyGILState_STATE gstate = PyGILState_Ensure();
#ifdef REFCNT
      DEBTRACE("_orb refCount: " << ((omniOrbORB*)_orb.in())->pd_refCount);
#endif
      PyObject *mainmod, *globals;
      mainmod = PyImport_AddModule("__main__");
      globals = PyModule_GetDict(mainmod);
      if (_useCorba)
        {
          PyObject* res;
          res=PyRun_String("orb.destroy()\n"
                           "\n",
                           Py_file_input,globals,globals );
          if(res == NULL)
            PyErr_Print();
          else
            Py_DECREF(res);
        }
      std::map<std::string,Node*>& nodeMap=_builtinCatalog->_nodeMap;
      delete nodeMap["PyFunction"];
      delete nodeMap["PyScript"];
      delete nodeMap["SalomePythonNode"];
      nodeMap.erase("PyFunction");
      nodeMap.erase("PyScript");
      nodeMap.erase("SalomePythonNode");

      Py_Finalize();
#ifdef REFCNT
      DEBTRACE("_orb refCount: " << ((omniOrbORB*)_orb.in())->pd_refCount);
#endif
    }
  else
    {
      if (_useCorba)
        {
#ifdef REFCNT
          DEBTRACE("_orb refCount: " << ((omniOrbORB*)_orb.in())->pd_refCount);
#endif
          _orb->destroy();
        }
    }
}

Proc* RuntimeSALOME::createProc(const std::string& name)
{
  return new SalomeProc(name);
}

Bloc* RuntimeSALOME::createBloc(const std::string& name)
{
  return new Bloc(name);
}

WhileLoop* RuntimeSALOME::createWhileLoop(const std::string& name)
{
  return new WhileLoop(name);
}

ForLoop* RuntimeSALOME::createForLoop(const std::string& name)
{
  return new ForLoop(name);
}

DataNode* RuntimeSALOME::createDataNode(const std::string& kind,const std::string& name)
{
  DataNode* node;
  if(kind == "" )
    {
      node = new PresetNode(name);
      return node;
    }
  else if(kind == "outnode" )
    {
      return new OutNode(name);
    }
  std::string msg="DataNode kind ("+kind+") unknown";
  throw Exception(msg);
}

InlineFuncNode* RuntimeSALOME::createFuncNode(const std::string& kind,const std::string& name)
{
  InlineFuncNode* node;
  if(kind == "" || kind == SalomeNode::KIND || kind == PythonNode::KIND)
    {
      node = new PyFuncNode(name);
      return node;
    }
  std::string msg="FuncNode kind ("+kind+") unknown";
  throw Exception(msg);
}

InlineNode* RuntimeSALOME::createScriptNode(const std::string& kind,const std::string& name)
{
  InlineNode* node;
  if(kind == "" || kind == SalomeNode::KIND || kind == PythonNode::KIND)
    {
      node = new PythonNode(name);
      return node;
    }
  std::string msg="ScriptNode kind ("+kind+") unknown";
  throw Exception(msg);
}

ServiceNode* RuntimeSALOME::createRefNode(const std::string& kind,const std::string& name)
{
  ServiceNode* node;
  if(kind == "" || kind == SalomeNode::KIND || kind == CORBANode::KIND)
    {
      node = new CORBANode(name);
      return node;
    }
  else if(kind == XmlNode::KIND)
    {
      node = new XmlNode(name);
      return node;
    }
  std::string msg="RefNode kind ("+kind+") unknown";
  throw Exception(msg);
}

ServiceNode* RuntimeSALOME::createCompoNode(const std::string& kind,const std::string& name)
{
  ServiceNode* node;
  if(kind == "" || kind == SalomeNode::KIND )
    {
      node=new SalomeNode(name);
      return node;
    }
  else if (kind == CppNode::KIND) 
    {
      node = new CppNode(name);
      return node;
    }
  std::string msg="CompoNode kind ("+kind+") unknown";
  throw Exception(msg);
}

ServiceInlineNode *RuntimeSALOME::createSInlineNode(const std::string& kind, const std::string& name)
{
  if(kind == "" || kind == SalomeNode::KIND )
    return new SalomePythonNode(name);
  std::string msg="CompoNode kind ("+kind+") unknown";
  throw Exception(msg);
}

ComponentInstance* RuntimeSALOME::createComponentInstance(const std::string& name,
                                                          const std::string& kind)
{
  ComponentInstance* compo;
  if(kind == "" || kind == SalomeComponent::KIND) 
    return new SalomeComponent(name);
  else if(kind == CORBAComponent::KIND)
    return new CORBAComponent(name);
  else if(kind == SalomePythonComponent::KIND)
    return new SalomePythonComponent(name);
  else if (kind == CppComponent::KIND)
    return new CppComponent(name);
  std::string msg="Component Instance kind ("+kind+") unknown";
  throw Exception(msg);
}

Container *RuntimeSALOME::createContainer(const std::string& kind)
{
  if(kind == "" || kind == SalomeComponent::KIND)
    return new SalomeContainer;
  else if (kind == CppComponent::KIND)
    return new CppContainer;
  std::string msg="Container kind ("+kind+") unknown";
  throw Exception(msg);
}

InputPort * RuntimeSALOME::createInputPort(const std::string& name,
                                           const std::string& impl,
                                           Node * node,
                                           TypeCode * type)
{
  if(impl == CppNode::IMPL_NAME)
    {
      return new InputCppPort(name, node, type);
    }
  else if(impl == PythonNode::IMPL_NAME)
    {
      return new InputPyPort(name, node, type);
    }
  else if(impl == CORBANode::IMPL_NAME)
    {
      return new InputCorbaPort(name, node, type);
    }
  else if(impl == XmlNode::IMPL_NAME)
    {
      return new InputXmlPort(name, node, type);
    }
  else if(impl == PresetNode::IMPL_NAME)
    {
      return new InputPresetPort(name, node, type);
    }
  else
    {
      stringstream msg;
      msg << "Cannot create " << impl << " InputPort" ;
      msg << " ("__FILE__ << ":" << __LINE__ << ")";
      throw Exception(msg.str());
    }
}

OutputPort * RuntimeSALOME::createOutputPort(const std::string& name,
                                             const std::string& impl,
                                             Node * node,
                                             TypeCode * type)
{
  if(impl == CppNode::IMPL_NAME)
    {
      return new OutputCppPort(name, node, type);
    }
  else if(impl == PythonNode::IMPL_NAME)
    {
      return new OutputPyPort(name, node, type);
    }
  else if(impl == CORBANode::IMPL_NAME)
    {
      return new OutputCorbaPort(name, node, type);
    }
  else if(impl == XmlNode::IMPL_NAME)
    {
      return new OutputXmlPort(name, node, type);
    }
  else if(impl == PresetNode::IMPL_NAME)
    {
      return new OutputPresetPort(name, node, type);
    }
  else
    {
      stringstream msg;
      msg << "Cannot create " << impl << " OutputPort" ;
      msg << " ("__FILE__ << ":" << __LINE__ << ")";
      throw Exception(msg.str());
    }
}

InputDataStreamPort* RuntimeSALOME::createInputDataStreamPort(const std::string& name,
                                                              Node *node,TypeCode *type)
{
  DEBTRACE("createInputDataStreamPort: " << name << " " << type->shortName());
  if(type->kind() == Objref && std::string(type->shortName(),7) == "CALCIUM")
    {
      return new InputCalStreamPort(name,node,type);
    }
  else
    {
      return new InputDataStreamPort(name,node,type);
    }
}

OutputDataStreamPort* RuntimeSALOME::createOutputDataStreamPort(const std::string& name,
                                                                Node *node,TypeCode *type)
{
  DEBTRACE("createOutputDataStreamPort: " << name << " " << type->shortName());
  if(type->kind() == Objref && std::string(type->shortName(),7) == "CALCIUM")
    {
      return new OutputCalStreamPort(name,node,type);
    }
  else
    {
      return new OutputDataStreamPort(name,node,type);
    }
}

//! Main adapter function : adapt an InputPort to be able to connect it to an OutputPort with a possible different implementation 
/*!
 *  \param source : InputPort to be adapted
 *  \param impl : new implementation (C++, python, CORBA, XML, Neutral)
 *  \param type : data type provided by the InputPort
 * 
 * \return : adapted InputPort
 */
InputPort* RuntimeSALOME::adapt(InputPort* source,
                                const std::string& impl,
                                TypeCode * type) throw (ConversionException)
{
  string imp_source=source->getNode()->getImplementation();
  if(imp_source == PythonNode::IMPL_NAME)
    {
      return adapt((InputPyPort*)source,impl,type);
    }
  else if(imp_source == CppNode::IMPL_NAME)
    {
      return adapt((InputCppPort*)source,impl,type);
    }
  else if(imp_source == CORBANode::IMPL_NAME)
    {
      return adapt((InputCorbaPort*)source,impl,type);
    }
  else if(imp_source == XmlNode::IMPL_NAME || imp_source == PresetNode::IMPL_NAME)
    {
      return adapt((InputXmlPort*)source,impl,type);
    }
  else if(imp_source == Runtime::RUNTIME_ENGINE_INTERACTION_IMPL_NAME)
    {
      return adaptNeutral(source,impl,type);
    }
  else
    {
      stringstream msg;
      msg << "Cannot adapt " << imp_source << " InputPort to " << impl;
      msg << " ("__FILE__ << ":" << __LINE__ << ")";
      throw ConversionException(msg.str());
    }
}

//! Adapt a Neutral input port to a Corba output port
/*!
 *   \param inport : Neutral input port to adapt to Corba type type
 *   \param type : output port type
 *   \return an adaptated input port of type InputCorbaPort
 */
InputPort* RuntimeSALOME::adaptNeutralToCorba(InputPort* inport,
                      TypeCode * type) throw (ConversionException)
{
  // BEWARE : using the generic check
  if(inport->edGetType()->isAdaptable(type))
    {
      //the output data is convertible to inport type
      return new CorbaNeutral(inport);
    }
  //non convertible type
  stringstream msg;
  msg << "Cannot connect Neutral InputPort to OutputCorbaPort : " ;
  msg << "(" <<__FILE__ << ":" <<__LINE__<< ")";
  throw ConversionException(msg.str());
}

//! Adapt a Neutral input port to a Python output port
/*!
 *   \param inport : input port to adapt to Python type type
 *   \param type : output port type
 *   \return an adaptated input port of type InputPyPort
 */
InputPort* RuntimeSALOME::adaptNeutralToPython(InputPort* inport,
                      TypeCode * type) throw (ConversionException)
{
  // BEWARE : using the generic check
  if(inport->edGetType()->isAdaptable(type))
    {
      //convertible type
      return new PyNeutral(inport);
    }
  //non convertible type
  stringstream msg;
  msg << "Cannot connect Neutral InputPort to OutputPyPort : " ;
  msg << "(" <<__FILE__ << ":" <<__LINE__<< ")";
  throw ConversionException(msg.str());
}

//! Adapt a Neutral input port to a Xml output port 
/*!
 *   \param inport : input port to adapt to Xml type type
 *   \param type : output port type
 *   \return an input port of type InputXmlPort
 */
InputPort* RuntimeSALOME::adaptNeutralToXml(InputPort* inport,
                      TypeCode * type) throw (ConversionException)
{
  // BEWARE : using the generic check
  if(inport->edGetType()->isAdaptable(type))
    {
      //convertible type
      return new XmlNeutral(inport);
    }
  //non convertible type
  stringstream msg;
  msg << "Cannot connect Neutral InputPort to OutputXmlPort : " ;
  msg << "(" <<__FILE__ << ":" <<__LINE__<< ")";
  throw ConversionException(msg.str());
}

//! Adapt a Neutral input port to a C++ output port 
/*!
 *   \param inport : input port to adapt to C++ type type
 *   \param type : output port type
 *   \return an input port of type InputCppPort
 */
InputPort* RuntimeSALOME::adaptNeutralToCpp(InputPort* inport,
                      TypeCode * type) throw (ConversionException)
{
  DEBTRACE("RuntimeSALOME::adaptNeutralToCpp(InputPort* inport" );
  if(isAdaptableNeutralCpp(type,inport->edGetType()))
    {
      //convertible type
      return new CppNeutral(inport);
    }
  //non convertible type
  stringstream msg;
  msg << "Cannot connect Neutral " << inport->edGetType()->getKindRepr() 
      << " InputPort to " << type->getKindRepr() << " OutputCppPort : " ;
  msg << "(" <<__FILE__ << ":" <<__LINE__<< ")";
  throw ConversionException(msg.str());
}

//! Adapt a Neutral input port to connect it to an output port with a given implementation
/*!
 *   \param source : Neutral input port to adapt to implementation impl and type type
 *   \param impl : output port implementation (C++, Python, Corba, Xml or Neutral)
 *   \param type : output port supported type
 *   \return       the adaptated port
 */
InputPort* RuntimeSALOME::adaptNeutral(InputPort* source,
                                       const std::string& impl,
                                       TypeCode * type) throw (ConversionException)
{
  if(impl == CppNode::IMPL_NAME)
    {
      return adaptNeutralToCpp(source,type);
    }
  else if(impl == PythonNode::IMPL_NAME)
    {
      return adaptNeutralToPython(source,type);
    }
  else if(impl == CORBANode::IMPL_NAME)
    {
      return adaptNeutralToCorba(source,type);
    }
  else if((impl == XmlNode::IMPL_NAME) || (impl == PresetNode::IMPL_NAME))
    {
      return adaptNeutralToXml(source,type);
    }
  else if(impl == Runtime::RUNTIME_ENGINE_INTERACTION_IMPL_NAME)
    {
      return new ProxyPort(source);
    }
  stringstream msg;
  msg << "Cannot connect InputPort : unknown implementation " << impl;
  msg << " (" <<__FILE__ << ":" <<__LINE__ << ")";
  throw ConversionException(msg.str());
}

//! Adapt a XML input port to connect it to a CORBA output port 
/*!
 *   \param inport : input port to adapt to CORBA type type
 *   \param type : type supported by output port
 *   \return an adaptator port of type InputCorbaPort 
 */

InputPort* RuntimeSALOME::adaptXmlToCorba(InputXmlPort* inport,
                                          TypeCode * type) throw (ConversionException)
{
  if(isAdaptableXmlCorba(type,inport->edGetType()))
    {
      //output type is convertible to input type
      return new CorbaXml(inport);
    }
  //output type is not convertible
  stringstream msg;
  msg << "Cannot connect InputXmlPort to Corba output port " ;
  msg << type->id() << " != " << inport->edGetType()->id();
  msg << " ("__FILE__ << ":" << __LINE__ << ")";
  throw ConversionException(msg.str());
}

//! Adapt a XML input port to a Python output port
/*!
 *   \param inport : input port to adapt to Python type type
 *   \param type : output port type
 *   \return an adaptated input port of type InputPyPort
 */
InputPort* RuntimeSALOME::adaptXmlToPython(InputXmlPort* inport,
                      TypeCode * type) throw (ConversionException)
{
  if(inport->edGetType()->isAdaptable(type))
    {
      //the output data is convertible to inport type
      return new PyXml(inport);
    }
  //non convertible type
  stringstream msg;
  msg << "Cannot connect Xml InputPort to OutputPyPort : " ;
  msg << "(" <<__FILE__ << ":" <<__LINE__<< ")";
  throw ConversionException(msg.str());
}

//! Adapt a XML input port to a C++ output port
/*!
 *   \param inport : input port to adapt to C++ type type
 *   \param type : output port type
 *   \return an adaptated input port of type InputPyPort
 */
InputPort* RuntimeSALOME::adaptXmlToCpp(InputXmlPort* inport,
                      TypeCode * type) throw (ConversionException)
{
  DEBTRACE("RuntimeSALOME::adaptXmlToCpp(InputPort* inport" );
  DEBTRACE(type->kind() << "   " << inport->edGetType()->kind() );
  if(type->isAdaptable(inport->edGetType()))
    {
      //the output data is convertible to inport type
      return new CppXml(inport);
    }
  //non convertible type
  stringstream msg;
  msg << "Cannot connect Xml InputPort to OutputCppPort : " ;
  msg << "(" <<__FILE__ << ":" <<__LINE__<< ")";
  throw ConversionException(msg.str());
}

//! Adapt a XML input port to a Neutral output port
/*!
 *   \param inport : input port to adapt to Neutral type type
 *   \param type : output port type
 *   \return an adaptated input port of type Neutralxxxx
 */
InputPort* RuntimeSALOME::adaptXmlToNeutral(InputXmlPort* inport,
                      TypeCode * type) throw (ConversionException)
{
  if(inport->edGetType()->isAdaptable(type))
    {
      //the output data is convertible to inport type
      return new NeutralXml(inport);
    }
  //non convertible type
  stringstream msg;
  msg << "Cannot connect Xml InputPort to OutputNeutralPort : " ;
  msg << "(" <<__FILE__ << ":" <<__LINE__<< ")";
  throw ConversionException(msg.str());
}

//! Adapt an Xml input port to an output port which implementation is given by impl
/*!
 *   \param source : input port to adapt to implementation impl and type type
 *   \param impl : output port implementation (C++, Python or Corba)
 *   \param type : output port supported type
 *   \return       the adaptated port
 */

InputPort* RuntimeSALOME::adapt(InputXmlPort* source,
                                const std::string& impl,
                                TypeCode * type) throw (ConversionException)
{
  if(impl == CORBANode::IMPL_NAME)
    {
      return adaptXmlToCorba(source,type);
    }
  else if(impl == PythonNode::IMPL_NAME)
    {
      return adaptXmlToPython(source,type);
    }
  else if(impl == CppNode::IMPL_NAME)
    {
      return adaptXmlToCpp(source,type);
    }
  else if((impl == XmlNode::IMPL_NAME) || (impl == PresetNode::IMPL_NAME))
    {
      return new ProxyPort(source);
    }
  else if(impl == Runtime::RUNTIME_ENGINE_INTERACTION_IMPL_NAME)
    {
      return adaptXmlToNeutral(source,type);
    }
  else
    {
      stringstream msg;
      msg << "Cannot connect InputXmlPort to " << impl << " implementation";
      msg << " ("__FILE__ << ":" << __LINE__ << ")";
      throw ConversionException(msg.str());
    }
}


//! Adapt a CORBA input port to a CORBA output port 
/*!
 *   \param inport : input port to adapt to CORBA outport data type
 *   \param type : outport data type 
 *   \return an adaptator port of type InputCORBAPort 
 */
InputPort* RuntimeSALOME::adaptCorbaToCorba(InputCorbaPort* inport,
                                            TypeCode * type) throw (ConversionException)
{
  if(type->isA(inport->edGetType()))
    {
      //types are compatible : no conversion 
      //outport data type is more specific than inport required type
      //so the inport can be used safely 
      return new ProxyPort(inport);
    }
  else if(isAdaptableCorbaCorba(type,inport->edGetType()))
    {
      //ouport data can be converted to inport data type
      return new CorbaCorba(inport);
    }
  //outport data can not be converted
  stringstream msg;
  msg << "Cannot connect 2 CorbaPort with non convertible types: " ;
  msg << type->id() << " != " << inport->edGetType()->id();
  throw ConversionException(msg.str());
}

//! Adapt a CORBA input port to a Python output port 
/*!
 *   \param inport : input port to adapt to Python type type
 *   \param type : outport data type 
 *   \return an adaptator port of type InputPyPort 
 */

InputPort* RuntimeSALOME::adaptCorbaToPython(InputCorbaPort* inport,
                                             TypeCode * type) throw (ConversionException)
{
  if(inport->edGetType()->kind() == Double)
    {
      if(isAdaptableCorbaPyObject(type,inport->edGetType()))return new PyCorbaDouble(inport);
    }
  else if(inport->edGetType()->kind() == Int)
    {
      if(isAdaptableCorbaPyObject(type,inport->edGetType()))return new PyCorbaInt(inport);
    }
  else if(inport->edGetType()->kind() == String)
    {
      if(isAdaptableCorbaPyObject(type,inport->edGetType()))return new PyCorbaString(inport);
    }
  else if(inport->edGetType()->kind() == Bool)
    {
      if(isAdaptableCorbaPyObject(type,inport->edGetType()))return new PyCorbaBool(inport);
    }
  else if(inport->edGetType()->kind() == Objref )
    {
      if(isAdaptableCorbaPyObject(type,inport->edGetType()))
        {
          return new PyCorbaObjref(inport);
        }
      else
        {
          stringstream msg;
          msg << "Cannot connect InputPyPort : incompatible objref types " << type->id() << " " << inport->edGetType()->id();
          msg << " " << __FILE__ << ":" <<__LINE__;
          throw ConversionException(msg.str());
        }
    }
  else if(inport->edGetType()->kind() == Sequence)
    {
      if(isAdaptableCorbaPyObject(type,inport->edGetType()))
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
  else if(inport->edGetType()->kind() == YACS::ENGINE::Struct)
    {
      if(isAdaptableCorbaPyObject(type,inport->edGetType()))
        {
          return new PyCorbaStruct(inport);
        }
      else
        {
          stringstream msg;
          msg << "Cannot convert this struct type " << type->id() << " to " << inport->edGetType()->id();
          msg << __FILE__ << ":" <<__LINE__;
          throw ConversionException(msg.str());
        }
    }
  // Adaptation not possible
  stringstream msg;
  msg << "Cannot connect Python output port with type: " << type->id() << " to CORBA input port with type: " << inport->edGetType()->id();
  msg << " (" << __FILE__ << ":" <<__LINE__ << ")";
  throw ConversionException(msg.str());
}

//! Adapt a CORBA input port to connect it to a XML output port 
/*!
 *   \param inport : input port to adapt to Xml type type
 *   \param type : type supported by output port
 *   \return an adaptator port of type InputXmlPort 
 */

InputPort* RuntimeSALOME::adaptCorbaToXml(InputCorbaPort* inport,
                                          TypeCode * type) throw (ConversionException)
{
  // BEWARE : using the generic check
  if(inport->edGetType()->isAdaptable(type))
    {
      //output type is convertible to input type
      return new XmlCorba(inport);
    }
  //output type is not convertible
  stringstream msg;
  msg << "Cannot connect InputCorbaPort with OutputXmlPort : " ;
  msg << __FILE__ << ":" <<__LINE__;
  throw ConversionException(msg.str());
}

//! Adapt a CORBA input port to a C++ output port 
/*!
 *   \param inport : input port to adapt to C++ type type
 *   \param type : outport data type 
 *   \return an adaptator port of type InputCPPPort 
 */

InputPort* RuntimeSALOME::adaptCorbaToCpp(InputCorbaPort* inport,
                                          TypeCode * type) throw (ConversionException)
{
  DEBTRACE("RuntimeSALOME::adaptCorbaToCpp(InputCorbaPort* inport" );
  if(isAdaptableCorbaCpp(type,inport->edGetType()))
    {
      //output type is convertible to input type
      return new CppCorba(inport);
    }
  //output type is not convertible
  stringstream msg;
  msg << "Cannot connect InputCorbaPort with OutputCppPort : " ;
  msg << __FILE__ << ":" <<__LINE__;
  throw ConversionException(msg.str());
}

//! Adapt a CORBA input port to a neutral data 
/*!
 *   \param inport : InputPort to adapt to Neutral type type
 *   \param type : outport data type 
 *   \return an adaptator port of type Neutralxxxx
 */

InputPort* RuntimeSALOME::adaptCorbaToNeutral(InputCorbaPort* inport,
                                              TypeCode * type) throw (ConversionException)
{
  if(inport->edGetType()->kind() == Double)
    {
      if(isAdaptableCorbaNeutral(type,inport->edGetType()))return new NeutralCorbaDouble(inport);
    }
  else if(inport->edGetType()->kind() == Int)
    {
      if(isAdaptableCorbaNeutral(type,inport->edGetType()))return new NeutralCorbaInt(inport);
    }
  else if(inport->edGetType()->kind() == String)
    {
      if(isAdaptableCorbaNeutral(type,inport->edGetType())) return new NeutralCorbaString(inport);
    }
  else if(inport->edGetType()->kind() == Bool)
    {
      if(isAdaptableCorbaNeutral(type,inport->edGetType()))return new NeutralCorbaBool(inport);
    }
  else if(inport->edGetType()->kind() == Objref)
    {
      if(isAdaptableCorbaNeutral(type,inport->edGetType())) return new NeutralCorbaObjref(inport);
    }
  else if(inport->edGetType()->kind() == Sequence)
    {
      if(isAdaptableCorbaNeutral(type,inport->edGetType()))
        return new NeutralCorbaSequence(inport);
      else
        {
          stringstream msg;
          msg << "Cannot convert this sequence type " ;
          msg << __FILE__ << ":" <<__LINE__;
          throw ConversionException(msg.str());
        }
    }

  // Adaptation not possible
  stringstream msg;
  msg << "Cannot connect InputCorbaPort to Neutral output " ;
  msg << __FILE__ << ":" <<__LINE__;
  throw ConversionException(msg.str());
}

//! Adapt a CORBA input port to an output which implementation and type are given by impl and type
/*!
 *   \param source : input port to adapt to implementation impl and type type
 *   \param impl : output port implementation (C++, Python or Corba)
 *   \param type : outport data type 
 *   \return an adaptator port which type depends on impl
 */

InputPort* RuntimeSALOME::adapt(InputCorbaPort* source,
                                const std::string& impl,
                                TypeCode * type) throw (ConversionException)
{
  if(impl == CppNode::IMPL_NAME)
    {
      return adaptCorbaToCpp(source,type);
    }
  else if(impl == PythonNode::IMPL_NAME)
    {
      return adaptCorbaToPython(source,type);
    }
  else if(impl == CORBANode::IMPL_NAME)
    {
      return adaptCorbaToCorba(source,type);
    }
  else if((impl == XmlNode::IMPL_NAME) || (impl == PresetNode::IMPL_NAME))
    {
      return adaptCorbaToXml(source,type);
    }
  else if(impl == Runtime::RUNTIME_ENGINE_INTERACTION_IMPL_NAME)
    {
      return adaptCorbaToNeutral(source,type);
    }
  else
    {
      stringstream msg;
      msg << "Cannot connect InputCorbaPort : unknown implementation " ;
      msg << __FILE__ << ":" <<__LINE__;
      throw ConversionException(msg.str());
    }
}

//! Adapt a Python input port to a Python output port
/*!
 * No need to make conversion or cast. 
 * Only check, it's possible.
 *   \param inport : InputPort to adapt to Python type type
 *   \param type : outport data type 
 *   \return an adaptator port of type InputPyPort 
 */

InputPort* RuntimeSALOME::adaptPythonToPython(InputPyPort* inport,
                                              TypeCode * type) throw (ConversionException)
{
  if(isAdaptablePyObjectPyObject(type,inport->edGetType()))
    {
      //output data is convertible to input type
      //With python, no need to convert. Conversion will be done automatically
      //by the interpreter
      return new ProxyPort(inport);
    }
  //output data is not convertible to input type
  stringstream msg;
  msg << "Cannot connect 2 Python Port with non convertible types: " ;
  msg << type->id() << " != " << inport->edGetType()->id();
  msg <<  " ("<<__FILE__ << ":" << __LINE__<<")";
  throw ConversionException(msg.str());
}

//! Adapt a Python input port to a C++ output port
/*!
 *   \param inport : InputPort to adapt to C++ type type
 *   \param type : outport data type 
 *   \return an adaptator port of C++ type (InputCppPort)
 */

InputPort* RuntimeSALOME::adaptPythonToCpp(InputPyPort* inport,
                                           TypeCode * type) throw (ConversionException)
{
  DEBTRACE("RuntimeSALOME::adaptPythonToCpp(InputPyPort* inport" );
  if(isAdaptablePyObjectCpp(type,inport->edGetType()))
    {
      //output type is convertible to input type
      return new CppPy(inport);
    }
  //output type is not convertible
  stringstream msg;
  msg << "Cannot connect InputPythonPort with OutputCppPort : " ;
  msg << __FILE__ << ":" <<__LINE__;
  throw ConversionException(msg.str());
}

//! Adapt a Python input port to a Neutral data port
/*!
 *   \param inport : InputPort to adapt to Neutral type type
 *   \param type : outport data type 
 *   \return an adaptator port of Neutral type (Neutralxxxx)
 */

InputPort* RuntimeSALOME::adaptPythonToNeutral(InputPyPort* inport,
                                               TypeCode * type) throw (ConversionException)
{
  if(inport->edGetType()->kind() == Double)
    {
      if(isAdaptablePyObjectNeutral(type,inport->edGetType()))return new NeutralPyDouble(inport);
    }
  else if(inport->edGetType()->kind() == Int)
    {
      if(isAdaptablePyObjectNeutral(type,inport->edGetType()))return new NeutralPyInt(inport);
    }
  else if(inport->edGetType()->kind() == String)
    {
      if(isAdaptablePyObjectNeutral(type,inport->edGetType()))return new NeutralPyString(inport);
    }
  else if(inport->edGetType()->kind() == Bool)
    {
      if(isAdaptablePyObjectNeutral(type,inport->edGetType()))return new NeutralPyBool(inport);
    }
  else if(inport->edGetType()->kind() == Objref)
    {
      if(isAdaptablePyObjectNeutral(type,inport->edGetType()))return new NeutralPyObjref(inport);
    }
  else if(inport->edGetType()->kind() == Sequence)
    {
      if(isAdaptablePyObjectNeutral(type,inport->edGetType()))
        return new NeutralPySequence(inport);
      else
        {
          stringstream msg;
          msg << "Cannot convert this sequence type " ;
          msg << __FILE__ << ":" <<__LINE__;
          throw ConversionException(msg.str());
        }
    }
  // Adaptation not possible
  stringstream msg;
  msg << "Cannot connect InputPyPort to Neutral output " ;
  msg << "Output typeid: " << type->id() << " Input typeid: " << inport->edGetType()->id();
  msg << " ("__FILE__ << ":" << __LINE__ << ")";
  throw ConversionException(msg.str());
}

//! Adapt a Python input port to a Corba output port
/*!
 * Always convert the data
 *   \param inport : InputPort to adapt to Corba type type
 *   \param type : outport data type 
 *   \return an adaptator port of Corba type (InputCorbaPort)
 */

InputPort* RuntimeSALOME::adaptPythonToCorba(InputPyPort* inport,
                                             TypeCode * type) throw (ConversionException)
{
  if(inport->edGetType()->kind() == Double)
    {
      if(isAdaptablePyObjectCorba(type,inport->edGetType()))return new CorbaPyDouble(inport);
    }
  else if(inport->edGetType()->kind() == Int)
    {
      if(isAdaptablePyObjectCorba(type,inport->edGetType()))return new CorbaPyInt(inport);
    }
  else if(inport->edGetType()->kind() == String)
    {
      if(isAdaptablePyObjectCorba(type,inport->edGetType()))return new CorbaPyString(inport);
    }
  else if(inport->edGetType()->kind() == Bool)
    {
      if(isAdaptablePyObjectCorba(type,inport->edGetType()))return new CorbaPyBool(inport);
    }
  else if(inport->edGetType()->kind() == Objref)
    {
      if(isAdaptablePyObjectCorba(type,inport->edGetType()))
        {
          return new CorbaPyObjref(inport);
        }
      else
        {
          stringstream msg;
          msg << "Cannot connect InputCorbaPort : incompatible objref types " << type->id() << " " << inport->edGetType()->id();
          msg << " " << __FILE__ << ":" <<__LINE__;
          throw ConversionException(msg.str());
        }
    }
  else if(inport->edGetType()->kind() == Sequence)
    {
      if(isAdaptablePyObjectCorba(type,inport->edGetType()))
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
  else if(inport->edGetType()->kind() == YACS::ENGINE::Struct)
    {
      if(isAdaptablePyObjectCorba(type,inport->edGetType()))
        {
          return new CorbaPyStruct(inport);
        }
      else
        {
          stringstream msg;
          msg << "Cannot convert this struct type " << type->id() << " to " << inport->edGetType()->id();
          msg << " " << __FILE__ << ":" <<__LINE__;
          throw ConversionException(msg.str());
        }
    }
  // Adaptation not possible
  stringstream msg;
  msg << "Cannot connect InputPyPort to Corba output " ;
  msg <<  __FILE__ << ":" << __LINE__;
  throw ConversionException(msg.str());
}

//! Adapt a Python input port to a Xml output port 
/*!
 *   \param inport : input port to adapt to Xml type type
 *   \param type : output port type
 *   \return an input port of type InputXmlPort
 */

InputPort* RuntimeSALOME::adaptPythonToXml(InputPyPort* inport,
                                          TypeCode * type) throw (ConversionException)
{
  // BEWARE : using the generic check
  if(inport->edGetType()->isAdaptable(type))
    {
      //convertible type
      return new XmlPython(inport);
    }
  //non convertible type
  stringstream msg;
  msg << "Cannot connect InputPyPort with OutputXmlPort : " ;
  msg << "(" <<__FILE__ << ":" <<__LINE__<< ")";
  throw ConversionException(msg.str());
}

//! Adapt a Python input port to an output port with a given implementation
/*!
 *   \param source : input port to adapt to implementation impl and type type
 *   \param impl : output port implementation (C++, Python or Corba)
 *   \param type : output port type
 *   \return     adaptated input port
 */

InputPort* RuntimeSALOME::adapt(InputPyPort* source,
                                const std::string& impl,
                                TypeCode * type) throw (ConversionException)
{
  if(impl == CppNode::IMPL_NAME)
    {
      return adaptPythonToCpp(source,type);
    }
  else if(impl == PythonNode::IMPL_NAME)
    {
      return adaptPythonToPython(source,type);
    }
  else if(impl == CORBANode::IMPL_NAME)
    {
      return adaptPythonToCorba(source,type);
    }
  else if(impl == Runtime::RUNTIME_ENGINE_INTERACTION_IMPL_NAME)
    {
      return adaptPythonToNeutral(source,type);
    }
  else if((impl == XmlNode::IMPL_NAME) || (impl == PresetNode::IMPL_NAME))
    {
      return adaptPythonToXml(source,type);
    }
  else
    {
      stringstream msg;
      msg << "Cannot connect InputPyPort : unknown implementation " << impl;
      msg <<  " ("<<__FILE__ << ":" << __LINE__<<")";
      throw ConversionException(msg.str());
    }
}


//! Adapt a C++ input port to connect it to a CORBA output port
/*!
 *   \param inport : input port to adapt to CORBA type type
 *   \param type : type supported by output port
 *   \return an adaptator port of type InputCorbaPort
 */

InputPort* RuntimeSALOME::adaptCppToCorba(InputCppPort* inport,
                                          TypeCode * type) throw (ConversionException)
{
  DEBTRACE("RuntimeSALOME::adaptCppToCorba(InputCppPort* inport)");
  if(isAdaptableCppCorba(type,inport->edGetType()))
    {
      //output type is convertible to input type
      return new CorbaCpp(inport);
    }
  //output type is not convertible
  stringstream msg;
  msg << "Cannot connect InputCppPort to Corba output port " ;
  msg << type->id() << " != " << inport->edGetType()->id();
  msg << " ("__FILE__ << ":" << __LINE__ << ")";
  throw ConversionException(msg.str());
}

//! Adapt a C++ input port to a Python output port
/*!
 *   \param inport : input port to adapt to Python type type
 *   \param type : output port type
 *   \return an adaptated input port of type InputPyPort
 */
InputPort* RuntimeSALOME::adaptCppToPython(InputCppPort* inport,
                      TypeCode * type) throw (ConversionException)
{
  DEBTRACE("RuntimeSALOME::adaptCppToPython(InputCppPort* inport)");
  if(isAdaptableCppPyObject(type,inport->edGetType()))
    {
      //output type is convertible to input type
      return new PyCpp(inport);
    }
  //output type is not convertible
  stringstream msg;
  msg << "Cannot connect InputCppPort with OutputPythonPort : " ;
  msg << __FILE__ << ":" <<__LINE__;
  throw ConversionException(msg.str());
}

//! Adapt a C++ input port to a C++ output port
/*!
 *   \param inport : input port to adapt to C++ type type
 *   \param type : output port type
 *   \return an adaptated input port of type InputPyPort
 */
InputPort* RuntimeSALOME::adaptCppToCpp(InputCppPort* inport,
                      TypeCode * type) throw (ConversionException)
{
  DEBTRACE("RuntimeSALOME::adaptCppToCpp(InputPort* inport" );
  DEBTRACE(type->kind() << "   " << inport->edGetType()->kind() );
  if(type->isAdaptable(inport->edGetType()))
    {
      //the output data is convertible to inport type
      return new CppCpp(inport);
    }
  //non convertible type
  stringstream msg;
  msg << "Cannot connect Cpp InputPort to OutputCppPort : " ;
  msg << "(" <<__FILE__ << ":" <<__LINE__<< ")";
  throw ConversionException(msg.str());
}

//! Adapt a C++ input port to a Neutral output port
/*!
 *   \param inport : input port to adapt to C++ type type
 *   \param type : output port type
 *   \return an adaptated input port of type InputPyPort
 */
InputPort* RuntimeSALOME::adaptCppToNeutral(InputCppPort* inport,
                      TypeCode * type) throw (ConversionException)
{
  DEBTRACE("RuntimeSALOME::adaptCppToNeutral(InputPort* inport" );
  DEBTRACE(type->kind() << "   " << inport->edGetType()->kind() );
  if(type->isAdaptable(inport->edGetType()))
    {
      //the output data is convertible to inport type
      return new NeutralCpp(inport);
    }
  //non convertible type
  stringstream msg;
  msg << "Cannot connect Cpp InputPort to OutputNeutralPort : " ;
  msg << "(" <<__FILE__ << ":" <<__LINE__<< ")";
  throw ConversionException(msg.str());
}

InputPort* RuntimeSALOME::adaptCppToXml(InputCppPort* inport,
                      TypeCode * type) throw (ConversionException)
{
   DEBTRACE("RuntimeSALOME::adaptCppToXml(InputCppPort* inport" );
   if(isAdaptableCppXml(type,inport->edGetType()))
   {
      //convertible type
      return new XmlCpp(inport);
   }
   //non convertible type
   stringstream msg;
   msg << "Cannot connect InputCppPort with OutputXmlPort : " ;
   msg << "(" <<__FILE__ << ":" <<__LINE__<< ")";
   throw ConversionException(msg.str());
}

//! Adapt a C++ input port to connect it to an output port with a given implementation
/*!
 *   \param source : input port to adapt to implementation impl and type type
 *   \param impl : output port implementation (C++, Python or Corba)
 *   \param type : output port supported type
 *   \return       the adaptated port
 */

InputPort* RuntimeSALOME::adapt(InputCppPort* source,
                                const std::string& impl,
                                TypeCode * type) throw (ConversionException)
{
  DEBTRACE("RuntimeSALOME::adapt(InputCppPort* source)");
  if(impl == CORBANode::IMPL_NAME)
    {
      return adaptCppToCorba(source,type);
    }
  else if(impl == PythonNode::IMPL_NAME)
    {
      return adaptCppToPython(source,type);
    }
  else if(impl == XmlNode::IMPL_NAME)
    {
      return adaptCppToXml(source,type);
    }
  else if(impl == CppNode::IMPL_NAME)
    {
      return adaptCppToCpp(source, type);
    }
  else if(impl == Runtime::RUNTIME_ENGINE_INTERACTION_IMPL_NAME)
    {
      return adaptCppToNeutral(source, type);
    }
  else
    {
      stringstream msg;
      msg << "Cannot connect InputCppPort to " << impl << " implementation";
      msg << " ("__FILE__ << ":" << __LINE__ << ")";
      throw ConversionException(msg.str());
    }
}

// bool RuntimeSALOME::isCompatible(const OutputPort* outputPort, 
//                               const InputPort*  inputPort)
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

