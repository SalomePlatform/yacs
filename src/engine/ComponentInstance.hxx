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

#ifndef __COMPONENTINSTANCE_HXX__
#define __COMPONENTINSTANCE_HXX__

#include "YACSlibEngineExport.hxx"
#include "RefCounter.hxx"
#include "PropertyInterface.hxx"

#include <list>
#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class Task;
    class ServiceNode;
    class Container;

    class YACSLIBENGINE_EXPORT ComponentInstance : public PropertyInterface, public RefCounter
    {
    protected:
      virtual ~ComponentInstance();
    public:
      ComponentInstance(const std::string& name);
      ComponentInstance(const ComponentInstance& other);
      const std::string& getCompoName() const { return _compoName; }
      const std::string& getInstanceName() const { return _instanceName; }
      void setName(const std::string& name) { _instanceName = name; };
      virtual void setAnonymous(bool anon) { _anonymous = anon; };
      virtual bool isAnonymous() { return _anonymous; };
      int getNumId() const { return _numId; }
      virtual bool setContainer(Container *cont);
      Container *getContainer() const { return _container; }
//! Load the component instance
      virtual void load(Task *askingNode) = 0;
//! Unload the component instance
      virtual void unload(Task *askingNode) = 0;
//! Indicate if the component instance is loaded (true) or not
      virtual bool isLoaded(Task *askingNode) const = 0;
      virtual void attachOnCloning() const;
      virtual void dettachOnCloning() const;
      bool isAttachedOnCloning() const;
      virtual std::string getFileRepr() const;
      virtual ServiceNode* createNode(const std::string& name)=0;
      virtual ComponentInstance *clone() const = 0;
      virtual ComponentInstance *cloneAlways() const = 0;
      virtual std::string getKind() const;
      virtual std::string getKindForNode() const;
      static const char KIND[];
      virtual void shutdown(int level);
    protected:
      //! \b WARNING : _compoName identify only the component type.
      std::string _compoName;
      //! \b WARNING : _InstanceName has a strong semantic. It discriminates ComponentInstance instances each other.
      std::string _instanceName;
      int _numId;
      Container *_container;
      mutable bool _isAttachedOnCloning;
    protected:
      static const char NULL_FILE_REPR[];
      static int _total;
      bool _anonymous;
    };
  }
}

#endif
