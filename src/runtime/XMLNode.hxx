
#ifndef _XMLNODE_HXX_
#define _XMLNODE_HXX_

#include "ElementaryNode.hxx"

namespace YACS
{
  namespace ENGINE
  {

    class XmlNode:public ENGINE::ElementaryNode
    {
    public:
      XmlNode(const std::string& name);
      virtual void execute();
      virtual void set_script(const std::string& script);
    protected:
      std::string _script;
    };
    

  }
}

#endif
