
#ifndef _CORBANODE_HXX_
#define _CORBANODE_HXX_

#include "ElementaryNode.hxx"

namespace YACS
{
  namespace ENGINE
  {
    
    class CORBANode: public ENGINE::ElementaryNode 
    {
    public:
      CORBANode(const std::string& name);
      virtual void execute();
      virtual void set_ref(const std::string& ref);
      virtual void set_method(const std::string& method);
    protected:
      std::string _ref;
      std::string _method;

    protected:
    };
  }
}

#endif
