#ifndef __OUTGATE_HXX__
#define __OUTGATE_HXX__

#include "Port.hxx"
#include "Exception.hxx"

#include <set>

namespace YACS
{
  namespace ENGINE
  {
    class InGate;
    class ElementaryNode;
    
    class OutGate : public Port
    {
      friend class ElementaryNode;
    protected:
      std::set<InGate *> _setOfInGate;
    public:
      static const char NAME[];
    public:
      OutGate(Node *node);
      std::string getNameOfTypeOfCurrentInstance() const;
      void exNotifyDone();
      bool edAddInGate(InGate *inGate);
      std::set<InGate *> edSetInGate() const;
      void edRemoveInGate(InGate *inGate) throw(Exception);
      int getNbOfInGatesConnected() const;
    protected:
      void edRemoveInGateOneWay(InGate *inGate);
      bool isAlreadyInSet(InGate *inGate) const;
    };
  }
}

#endif
