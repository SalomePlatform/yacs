#ifndef __INGATE_HXX__
#define __INGATE_HXX__

#include "Port.hxx"
#include "define.hxx"

#include <map>
#include <list>

namespace YACS
{
  namespace ENGINE
  {
    class OutGate;

    class InGate : public Port
    {
      friend class Bloc;
      friend class Node;
    protected:
      static const char NAME[];
    private:
      std::map< OutGate *, bool > _backLinks;
    public:
      InGate(Node *node);
      virtual ~InGate();
      std::string getNameOfTypeOfCurrentInstance() const;
      void exNotifyFromPrecursor(OutGate *from);
      std::map<OutGate *, bool>& edMapOutGate() { return _backLinks; }
      void edAppendPrecursor(OutGate *from);
      void edRemovePrecursor(OutGate *from);
      int getNumberOfBackLinks() const;
      void edDisconnectAllLinksToMe();
      void exNotifyFailed();
      void exNotifyDisabled();
      void exReset();
      bool exIsReady() const;
      std::list<OutGate *> getBackLinks();
      void setPrecursorDone(OutGate *from);
      virtual std::string typeName() {return "YACS__ENGINE__InGate";}
    };
  }
}

#endif
