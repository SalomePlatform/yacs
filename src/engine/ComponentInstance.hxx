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
#ifndef __COMPONENTINSTANCE_HXX__
#define __COMPONENTINSTANCE_HXX__

#include "RefCounter.hxx"

#include <list>
#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class Container;
    class ServiceNode;

/*! \brief Base class for all component instances.
 *
 *
 * This is an abstract class that must be specialized in runtime.
 * Specialized classes must provide implementation for loading of
 * a component (load method) unloading (unload method) and an 
 * information method (isLoaded) about the state of the component
 *
 * A component instance is used by one or more ServiceNode to execute
 * services of this component instance
 *
 * \see ServiceNode
 */
    class ComponentInstance : public RefCounter
    {
    protected:
      virtual ~ComponentInstance();
    public:
      ComponentInstance(const std::string& name);
      ComponentInstance(const ComponentInstance& other);
      const std::string& getCompoName() const { return _compoName; }
      const std::string& getInstanceName() const { return _instanceName; }
      int getNumId() const { return _numId; }
      virtual void setContainer(Container *cont);
      Container *getContainer() const { return _container; }
//! Load the component instance
      virtual void load() = 0;
//! Unload the component instance
      virtual void unload() = 0;
//! Indicate if the component instance is loaded (true) or not
      virtual bool isLoaded() = 0;
      virtual void attachOnCloning() const;
      virtual void dettachOnCloning() const;
      bool isAttachedOnCloning() const;
//! For dump in file
      virtual std::string getFileRepr() const;
      virtual ServiceNode* createNode(const std::string& name)=0;
      virtual ComponentInstance *clone() const = 0;
//! Return the component kind
/*!
 * A runtime can provide several implementations of a component instance.
 * Each implementation has a different kind. A ComponentInstance can be 
 * associated to a ServiceNode is they have the same kind.
 */
      virtual std::string getKind() const;
      static const char KIND[];
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
    };
  }
}

#endif
