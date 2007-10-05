#ifndef __SERVICEINLINENODE_HXX__
#define __SERVICEINLINENODE_HXX__

#include "ServiceNode.hxx"
#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class Visitor;
    
    class ServiceInlineNode: public ServiceNode 
    {
    protected:
      ServiceInlineNode(const std::string& name);
      ServiceInlineNode(const ServiceInlineNode& other, ComposedNode *father);
    public:
      void setScript (const std::string &script);
      std::string getScript() const;
      void accept(Visitor *visitor);
    protected:
      std::string _script;
    };
  }
}

#endif
