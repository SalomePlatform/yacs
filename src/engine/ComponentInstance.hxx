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
      const std::string& getName() const { return _name; }
      void setName(const std::string& name) { _name = name; }
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
      //! \b WARNING : _name has a strong semantic. It discriminates ComponentInstance instances each other.
      std::string _name;
      Container *_container;
      mutable bool _isAttachedOnCloning;
    protected:
      static const char NULL_FILE_REPR[];
    };
  }
}

#endif
