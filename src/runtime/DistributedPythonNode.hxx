//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef __DISTRIBUTEDPYTHONNODE_HXX__
#define __DISTRIBUTEDPYTHONNODE_HXX__

#include "ServerNode.hxx"

#include <Python.h>
#include <omniORB4/CORBA.h>

namespace YACS
{
  namespace ENGINE
  {
    class DistributedPythonNode : public ServerNode
    {   
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      DistributedPythonNode(const std::string& name);
      DistributedPythonNode(const DistributedPythonNode& other, ComposedNode *father);
      ~DistributedPythonNode();
      void execute();
      void load();
      //!this method returns the type of container expected
      std::string getEffectiveKindOfServer() const;
      //!this method returns the type of input/output expected
      std::string getKind() const;
      ServerNode *createNode(const std::string& name) const;
      virtual std::string typeName() {return "YACS__ENGINE__DistributedPythonNode";}
    private:
      void initMySelf();
      void dealException(CORBA::Exception *exc, const char *method, const char *ref);
    public:
      static const char KIND[];
      static const char IMPL_NAME[];
      static const char SALOME_CONTAINER_METHOD_IDL[];
    protected:
      PyObject* _context;
      PyObject* _pyfuncSer;
      PyObject* _pyfuncUnser;
    };
  }
}

#endif