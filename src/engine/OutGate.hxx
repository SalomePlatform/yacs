#ifndef __OUTGATE_HXX__
#define __OUTGATE_HXX__

#include "Port.hxx"
#include "Exception.hxx"

#include <list>

namespace YACS
{
  namespace ENGINE
  {
    class InGate;
    
    class OutGate : public Port
    {
    protected:
      std::list<InGate *> _listOfInGate;
    public:
      static const char NAME[];
    public:
      OutGate(Node *node);
      std::string getNameOfTypeOfCurrentInstance() const;
      void exNotifyDone();
      bool edAddInGate(InGate *inGate);
      std::list<InGate *> edListInGate() const;
      void edRemoveInGate(InGate *inGate) throw(Exception);
      int getNbOfInGatesConnected() const;
    protected:
      bool isAlreadyInList(InGate *inGate) const;
    };
  }
}

#endif
