#ifndef __WHILELOOP_HXX__
#define __WHILELOOP_HXX__

#include "Loop.hxx"
#include "ConditionInputPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
/*! \brief Class for a while loop
 *
 * \ingroup Nodes
 *
 *  This node makes steps while the condition is true
 *
 */
    class WhileLoop : public Loop
    {
    protected:
      static const char NAME_OF_INPUT_CONDITION[];
      ConditionInputPort _conditionPort;
    public:
      WhileLoop(const WhileLoop& other, ComposedNode *father, bool editionOnly);
      WhileLoop(const std::string& name);
      void exUpdateState();
      void init(bool start=true);
      InputPort *edGetConditionPort() { return &_conditionPort; }
      InputPort* getInputPort(const std::string& name) const throw(Exception);
      std::list<InputPort *> getLocalInputPorts() const;
      virtual void accept(Visitor *visitor);
      InputPort *getDecisionPort() const { return (InputPort *)&_conditionPort; }
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly=true) const;
      void checkLinkPossibility(OutPort *start, const std::set<ComposedNode *>& pointsOfViewStart,
                                InPort *end, const std::set<ComposedNode *>& pointsOfViewEnd) throw(Exception);
      YACS::Event updateStateOnFinishedEventFrom(Node *node);
    };
  }
}

#endif
