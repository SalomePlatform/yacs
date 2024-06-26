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

#ifndef _PYTHONNODE_HXX_
#define _PYTHONNODE_HXX_

#include "YACSRuntimeSALOMEExport.hxx"
#include "InlineNode.hxx"
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_PyNode)
#include CORBA_CLIENT_HEADER(SALOME_Component)

#include <Python.h>

#include <set>

namespace YACS
{
  namespace ENGINE
  {
    class YACSRUNTIMESALOME_EXPORT PythonEntry
    {
    public:
      static bool IsProxy( PyObject *ob );
      /*! return true only if ob is a proxy and destroy flag set to true*/
      static bool GetDestroyStatus( PyObject *ob );
      static void DoNotTouchFileIfProxy( PyObject *ob );
      static void UnlinkOnDestructorIfProxy( PyObject *ob );
      static void IfProxyDoSomething( PyObject *ob, const char *meth );
    protected:
      PythonEntry();
      ~PythonEntry();
      virtual void assignRemotePyInterpretor(Engines::PyNodeBase_var remoteInterp) = 0;
      //! returns (if any) an object, you have to deal with (UnRegister)
      virtual Engines::PyNodeBase_var retrieveDftRemotePyInterpretorIfAny(Engines::Container_ptr objContainer) const = 0;
      //! returns an object, you have to deal with (UnRegister)
      virtual void createRemoteAdaptedPyInterpretor(Engines::Container_ptr objContainer) = 0;
      virtual Engines::PyNodeBase_var getRemoteInterpreterHandle() = 0;
      virtual const char *getSerializationScript() const = 0;
      //
      void commonRemoteLoad(InlineNode *reqNode);
      void loadRemoteContainer(InlineNode *reqNode);
      Engines::Container_var loadPythonAdapter(InlineNode *reqNode, bool& isInitializeRequested);
      void loadRemoteContext(InlineNode *reqNode, Engines::Container_ptr objContainer, bool isInitializeRequested);
      static std::string GetContainerLog(const std::string& mode, Container *container, const Task *askingTask);
      virtual bool hasImposedResource()const;
    protected:
      PyObject *_context;
      PyObject *_pyfuncSer;
      PyObject *_pyfuncUnser;
      PyObject *_pyfuncSimpleSer;
      static PyObject *_pyClsBigObject;
      std::string _imposedResource;
      std::string _imposedContainer;
    public:
      static const char SCRIPT_FOR_SIMPLE_SERIALIZATION[];
    };

    class YACSRUNTIMESALOME_EXPORT PythonNode : public InlineNode, public PythonEntry
    {
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
      // overload part of PythonEntry
      void createRemoteAdaptedPyInterpretor(Engines::Container_ptr objContainer);
      Engines::PyNodeBase_var retrieveDftRemotePyInterpretorIfAny(Engines::Container_ptr objContainer) const;
      void assignRemotePyInterpretor(Engines::PyNodeBase_var remoteInterp);
      Engines::PyNodeBase_var getRemoteInterpreterHandle();
      const char *getSerializationScript() const  { return SCRIPT_FOR_SERIALIZATION; }
      // A kernel container may manage several python contexts identified by
      // their name (PyNode and PyScript node). This function returns  the name
      // of the context used by this object. See SALOME_Component.idl in KERNEL.
      std::string pythonEntryName()const;
    public:
      PythonNode(const PythonNode& other, ComposedNode *father);
      PythonNode(const std::string& name);
      virtual ~PythonNode();
      virtual void checkBasicConsistency() const ;
      virtual void execute();
      virtual void load();
      virtual void loadRemote();
      virtual void loadLocal();
      virtual void executeRemote();
      virtual void executeLocal();
      virtual void shutdown(int level);
      void imposeResource(const std::string& resource_name,
                          const std::string& container_name) override;
      bool canAcceptImposedResource()override;
      bool hasImposedResource()const override;
      bool isUsingPythonCache()const;
      std::string getContainerLog();
      PythonNode* cloneNode(const std::string& name);
      virtual std::string typeName() { return "YACS__ENGINE__PythonNode"; }
      void applyDPLScope(ComposedNode *gfn);
      void setSqueezeStatusWithExceptions(bool sqStatus, const std::vector<std::string>& squeezeExceptions);
      void setSqueezeStatus(bool sqStatus) { _autoSqueeze=sqStatus; }
      bool getSqueezeStatus() const { return _autoSqueeze; }
      void squeezeMemorySafe();
      static void ExecuteLocalInternal(const std::string& codeStr, PyObject *context, std::string& errorDetails);
    private:
      void executeLocalInternal(const std::string& codeStr);
    protected:
      void squeezeMemory();
      void squeezeMemoryRemote();
      void freeKernelPynode();
    public:
      static const char KIND[];
      static const char IMPL_NAME[];
      static const char SCRIPT_FOR_SERIALIZATION[];
      static const char REMOTE_NAME[];
      static const char DPL_INFO_NAME[];
    protected:
      bool _autoSqueeze = false;
      //! list on output port name excluded from the squeeze mecanism
      std::set<std::string> _nonSqueezableOutputNodes;
      Engines::PyScriptNode_var _pynode;
    };

    class YACSRUNTIMESALOME_EXPORT PyFuncNode : public InlineFuncNode, public PythonEntry
    {
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
      // overload part of PythonEntry
      void createRemoteAdaptedPyInterpretor(Engines::Container_ptr objContainer);Engines::
      PyNodeBase_var retrieveDftRemotePyInterpretorIfAny(Engines::Container_ptr objContainer) const;
      void assignRemotePyInterpretor(Engines::PyNodeBase_var remoteInterp);
      Engines::PyNodeBase_var getRemoteInterpreterHandle();
      const char *getSerializationScript() const { return SCRIPT_FOR_SERIALIZATION; }
    public:
      PyFuncNode(const PyFuncNode& other, ComposedNode *father);
      PyFuncNode(const std::string& name);
      virtual ~PyFuncNode();
      virtual void init(bool start=true);
      virtual void checkBasicConsistency() const ;
      virtual void execute();
      virtual void load();
      virtual void loadRemote();
      virtual void loadLocal();
      virtual void executeRemote();
      virtual void executeLocal();
      virtual void shutdown(int level);
      void imposeResource(const std::string& resource_name,
                          const std::string& container_name) override;
      bool canAcceptImposedResource()override;
      bool hasImposedResource()const override;
      std::string getContainerLog();
      PyFuncNode* cloneNode(const std::string& name);
      virtual std::string typeName() { return "YACS__ENGINE__PyFuncNode"; }
    public:
      static const char SCRIPT_FOR_SERIALIZATION[];
    protected:
      PyObject* _pyfunc;
      Engines::PyNode_var _pynode;
    };
  }
}

#endif
