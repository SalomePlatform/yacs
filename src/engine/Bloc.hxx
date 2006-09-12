#ifndef __BLOC_HXX__
#define __BLOC_HXX__

#include "ComposedNode.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class Bloc : public ComposedNode
    {
    protected:
      std::list<Node *> _listOfNode;//OWNERSHIP OF ALL NODES
    public:
      Bloc(const std::string& name);
      ~Bloc();
      void init();
      bool isFinished();
      int getNumberOfCFLinks() const;
      std::vector<Task *> getNextTasks(bool& isMore);
      void getReadyTasks(std::vector<Task *>& tasks);
      std::list<ElementaryNode *> getRecursiveConstituents();
      void exUpdateState();
      bool edAddChild(Node *node) throw(Exception);
      void edRemoveChild(Node *node) throw(Exception);
      std::list<Node *> getChildren() { return _listOfNode; }
      void selectRunnableTasks(std::vector<Task *>& tasks);
    protected:
      bool areAllSubNodesFinished() const;
      bool isNodeAlreadyAggregated(Node *node) const;
      bool isNameAlreadyUsed(const std::string& name) const;
      void checkNoCyclePassingThrough(Node *node) throw(Exception);
      YACS::Event updateStateOnStartEventFrom(Node *node);
      YACS::Event updateStateOnFinishedEventFrom(Node *node);
    private:
      void initChildrenForDFS() const;
    };
  }
}

#endif
