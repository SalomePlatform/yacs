#ifndef __OUTGATE_HXX__
#define __OUTGATE_HXX__

#include "Port.hxx"
#include "Exception.hxx"

#include <map>
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
      std::map<InGate *, bool> _setOfInGate;
    public:
      static const char NAME[];
    public:
      OutGate(Node *node);
      std::string getNameOfTypeOfCurrentInstance() const;
      void exReset();
      void exNotifyDone();
      void exNotifyFailed();
      void exNotifyDisabled();
      void edDisconnectAllLinksFromMe();
      bool edAddInGate(InGate *inGate);
      std::map<InGate *, bool>& edMapInGate() { return _setOfInGate; }
      std::set<InGate *> edSetInGate() const;
      void edRemoveInGate(InGate *inGate, bool coherenceWithInGate=true) throw(Exception);
      int getNbOfInGatesConnected() const;
      bool isAlreadyInSet(InGate *inGate) const;
      virtual std::string typeName() {return "YACS__ENGINE__OutGate";}
    protected:
      void edRemoveInGateOneWay(InGate *inGate);
    };
  }
}

#endif
