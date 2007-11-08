#ifndef __FORLOOP_HXX__
#define __FORLOOP_HXX__

#include "AnyInputPort.hxx"
#include "Loop.hxx"

namespace YACS
{
  namespace ENGINE
  {
/*! \brief Class for for loop node
 *
 * \ingroup Nodes
 *
 *   This kind of loop makes a fixed number of steps and stops
 *
 */
    class ForLoop : public Loop
    {
    protected:
      static const char NAME_OF_NSTEPS_NUMBER[];
      AnyInputPort _nbOfTimesPort;
    public:
      ForLoop(const ForLoop& other, ComposedNode *father, bool editionOnly);
      ForLoop(const std::string& name);
      void exUpdateState();
      void init(bool start=true);
      InputPort *edGetNbOfTimesInputPort() { return &_nbOfTimesPort; }
      Node *simpleClone(ComposedNode *father, bool editionOnly=true) const;
      InputPort* getInputPort(const std::string& name) const throw(Exception);
      std::list<InputPort *> getLocalInputPorts() const;
      virtual void accept(Visitor *visitor);
      InputPort *getDecisionPort() const { return (InputPort *)&_nbOfTimesPort; }
    protected:
      YACS::Event updateStateOnFinishedEventFrom(Node *node);
    };
  }
}

#endif
