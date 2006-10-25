
#ifndef _CPPNODE_HXX_
#define _CPPNODE_HXX_

#include "ElementaryNode.hxx"

namespace YACS
{
  namespace ENGINE
  {
    // local C++ implementation - single process
    
    typedef bool (*MYRUN)(int nbin, int nbout, void **in, void** out);
    
    class CppNode: public ENGINE::ElementaryNode 
    {
    public:
      CppNode(const std::string name);
      virtual void execute();
      void setfunc(MYRUN fonc) { _run = fonc;}
      
    protected:
      MYRUN _run;

    };
  }
}

#endif
