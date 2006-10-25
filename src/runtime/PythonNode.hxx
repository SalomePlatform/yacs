
#ifndef _PYTHONNODE_HXX_
#define _PYTHONNODE_HXX_

#include "ElementaryNode.hxx"

namespace YACS
{
  namespace ENGINE
  {
    
    class PythonNode: public ElementaryNode 
    {
    public:
      PythonNode(const std::string& name);
      virtual void execute();
      virtual void set_script(const std::string& script);

    protected:
      std::string _script;
    };
  }
}

#endif
