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

#ifndef _SALOMEPYTHONNODE_HXX_
#define _SALOMEPYTHONNODE_HXX_

#include "ServiceInlineNode.hxx"
#include <Python.h>

namespace YACS
{
  namespace ENGINE
  {
    /*!
     * This class is in charge to deal with python willing to interact with YACS placed container.
     * Typical use is for nodes building GEOM and MESH. The great number of calls needed by this type of application,
     * implies a human impossibility to make the corresponding graph.
    */
    class SalomePythonNode : public ServiceInlineNode 
    {
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      SalomePythonNode(const SalomePythonNode& other, ComposedNode *father);
      SalomePythonNode(const std::string& name);
      virtual void execute();
      virtual void load();
      std::string getKind() const;
      ServiceNode *createNode(const std::string &name);
      SalomePythonNode* cloneNode(const std::string& name);
    protected:
      PyObject* _context;
      PyObject* _pyfunc;
    public:
      static const char PLACEMENT_VAR_NAME_IN_INTERP[];
    };
  }
}

#endif
