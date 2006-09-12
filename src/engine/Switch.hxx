#ifndef __SWITCH_HXX__
#define __SWITCH_HXX__

#include "ComposedNode.hxx"

#include <vector>

namespace YACS
{
  namespace ENGINE
  {
    class Switch : public ComposedNode
    {
    protected:
      std::vector<Node *> _vectorOfNode;//Nodes ownered
    public:
      Switch(const std::string& name);
      ~Switch();
      void edSetNumberOfCases(int numberOfCases);
      void edSetNode(int caseId, Node *node) throw(Exception);
    protected:
      void checkNoCyclePassingThrough(Node *node) throw(Exception);
    };
  }
}

#endif
