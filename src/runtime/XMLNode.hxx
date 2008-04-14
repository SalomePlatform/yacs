#ifndef _XMLNODE_HXX_
#define _XMLNODE_HXX_

#include "ServiceNode.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class XmlNode:public ServiceNode
    {
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      XmlNode(const XmlNode& other, ComposedNode *father);
      XmlNode(const std::string& name);
      virtual void execute();
      //! \b DISTRIBUTION \b NOT \b YET implemented for XMLNode.
      virtual void load() { }
      virtual void setRef(const std::string& ref);
      virtual void setScript(const std::string& script);
      virtual std::string getScript() const;
      virtual ServiceNode* createNode(const std::string& name) 
        { throw Exception("not implemented"); }
      virtual std::string getKind() const;
    public:
      static const char IMPL_NAME[];
      static const char KIND[];
    protected:
      std::string _script;
    };
  }
}

#endif
