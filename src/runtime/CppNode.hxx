
#ifndef _CPPNODE_HXX_
#define _CPPNODE_HXX_

#include "ServiceNode.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class Any;

    // local C++ implementation - single process

    class CppComponent;

    typedef void (*MYRUN)(int nbin, int nbout, 
                          YACS::ENGINE::Any **in, YACS::ENGINE::Any ** out);
    
/*! \brief Class for C++ Nodes (in process component)
 *
 * local C++ implementation - single process
 *
 * \ingroup Nodes
 *
 */
    class CppNode : public YACS::ENGINE::ServiceNode 
    {
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      CppNode(const CppNode &other, ComposedNode *father);
      CppNode(const std::string &name);
      virtual void load();
      virtual void execute();
      virtual ServiceNode* createNode(const std::string& name);
      CppNode* cloneNode(const std::string& name);

      void setCode(const std::string & componentName, const std::string & service);
      void setFunc(MYRUN fonc);
    
      static const char IMPL_NAME[];
      static const char KIND[];
      std::string getKind() const { return CppNode::KIND; }
    protected:
      std::string _componentName;
      MYRUN _run;

    };
  }
}

#endif
