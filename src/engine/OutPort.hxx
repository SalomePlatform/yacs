#ifndef __OUTPORT_HXX__
#define __OUTPORT_HXX__

#include "DataPort.hxx"
#include "Exception.hxx"

#include <set>
#include <vector>

namespace YACS
{
  namespace ENGINE
  {
    class InPort;
    class LinkInfo;
    class OutPort : public virtual DataPort
    {
    protected:
      OutPort(const OutPort& other, Node *newHelder);
      OutPort(const std::string& name, Node *node, TypeCode* type);
    public:
      virtual void checkConsistency(LinkInfo& info) const;
      virtual int edGetNumberOfOutLinks() const;
      virtual std::set<InPort *> edSetInPort() const = 0;
      virtual bool isAlreadyLinkedWith(InPort *with) const = 0;
      virtual void getAllRepresented(std::set<OutPort *>& represented) const;
      virtual bool addInPort(InPort *inPort) throw(Exception) = 0;
      virtual int removeInPort(InPort *inPort, bool forward) throw(Exception) = 0;
      virtual ~OutPort();
      std::vector<DataPort *> calculateHistoryOfLinkWith(InPort *end);
      virtual std::string typeName() {return "YACS__ENGINE__OutPort";}
    };
  }
}

#endif
