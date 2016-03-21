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

#ifndef __SALOMEPYTHONCOMPONENT_HXX__
#define __SALOMEPYTHONCOMPONENT_HXX__

#include "ComponentInstance.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class SalomePythonComponent : public ComponentInstance
    {
    public:
      SalomePythonComponent(const std::string &name);
      SalomePythonComponent(const SalomePythonComponent& other);
      std::string getPlacementId() const;
      virtual ~SalomePythonComponent();
      virtual void load(Task *askingNode);
      virtual void unload(Task *askingNode);
      virtual bool isLoaded(Task *askingNode) const;
      virtual std::string getKind() const;
      virtual ComponentInstance* clone() const;
      virtual ComponentInstance* cloneAlways() const;
      virtual std::string getFileRepr() const;
      virtual ServiceNode *createNode(const std::string &name);
      //! The specific method that justified SalomePythonComponent class.
      std::string getStringValueToExportInInterp(const Task *askingNode) const;
    public:
      unsigned _cntForRepr;
      static unsigned _cntForReprS;
      static const char KIND[];
    };
  }
}

#endif
