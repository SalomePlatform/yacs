#ifndef __SERVICENODE_HXX__
#define __SERVICENODE_HXX__

#include "ElementaryNode.hxx"
#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class ComponentInstance;

/*! \brief Class for calculation node associated with a component service
 *
 * \ingroup Nodes
 *
 * \see InlineNode
 * \see ElementaryNode
 */
    class ServiceNode: public ElementaryNode 
    {
    protected:
      ServiceNode(const std::string& name);
      ServiceNode(const ServiceNode& other, ComposedNode *father);
      void performDuplicationOfPlacement(const Node& other);
    public:
      virtual void load();
      virtual bool isDeployable() const;
      virtual void setComponent(ComponentInstance* compo) throw(Exception);
      virtual ComponentInstance *getComponent();
      virtual void setRef(const std::string& ref);
      virtual std::string getRef();
      virtual void setMethod(const std::string& method){ _method=method; }
      virtual std::string getMethod(){return _method;}
      virtual ServiceNode *createNode(const std::string& name) = 0;
      virtual ~ServiceNode();
      virtual void accept(Visitor *visitor);

//! Return the service node kind
/*!
 * A runtime can provide several implementations of a service node.
 * Each implementation has a different kind. A ComponentInstance can be
 * associated to a ServiceNode with the same kind.
 */
      virtual std::string getKind() const;
      static const char KIND[];
    protected:
      ComponentInstance* _component;
      std::string _method;
      std::string _ref;
    };
  }
}

#endif
