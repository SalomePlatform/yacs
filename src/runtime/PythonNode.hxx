// Copyright (C) 2006-2012  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include <Python.h>

namespace YACS
{
  namespace ENGINE
  {
    class YACSRUNTIMESALOME_EXPORT PythonNode : public InlineNode 
    {
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      PythonNode(const PythonNode& other, ComposedNode *father);
      PythonNode(const std::string& name);
      virtual ~PythonNode();
      virtual void checkBasicConsistency() const throw(Exception);
      virtual void execute();
      virtual void load();
      virtual void loadRemote();
      virtual void loadLocal();
      virtual void executeRemote();
      virtual void executeLocal();
      virtual void shutdown(int level);
      std::string getContainerLog();
      PythonNode* cloneNode(const std::string& name);
      static const char KIND[];
      static const char IMPL_NAME[];
      virtual std::string typeName() {return "YACS__ENGINE__PythonNode";}
    protected:
      PyObject* _context;
      PyObject* _pyfuncSer;
      PyObject* _pyfuncUnser;
      Engines::PyScriptNode_var _pynode;
    };

    class PyFuncNode : public InlineFuncNode 
    {
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      PyFuncNode(const PyFuncNode& other, ComposedNode *father);
      PyFuncNode(const std::string& name);
      virtual ~PyFuncNode();
      virtual void checkBasicConsistency() const throw(Exception);
      virtual void execute();
      virtual void load();
      virtual void loadRemote();
      virtual void loadLocal();
      virtual void executeRemote();
      virtual void executeLocal();
      virtual void shutdown(int level);
      std::string getContainerLog();
      PyFuncNode* cloneNode(const std::string& name);
      virtual std::string typeName() {return "YACS__ENGINE__PyFuncNode";}
    protected:
      PyObject* _context;
      PyObject* _pyfunc;
      PyObject* _pyfuncSer;
      PyObject* _pyfuncUnser;
      Engines::PyNode_var _pynode;
    };
  }
}

#endif
