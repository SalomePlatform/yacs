#ifndef __INPORT_HXX__
#define __INPORT_HXX__

#include "DataPort.hxx"

#include <set>

namespace YACS
{
  namespace ENGINE
  {
    class Loop;
    class OutPort;
    class ProxyPort;
    class OutputPort;
    class DynParaLoop;
    class ForEachLoop;
    class SplitterNode;
    class ComposedNode;
    class OptimizerLoop;
    class ElementaryNode;
    class CollectorSwOutPort;
    class OutputDataStreamPort;
    class InterceptorInputPort;

    class InPort : public virtual DataPort
    {
      friend class Loop;
      friend class OutPort;
      friend class ProxyPort;
      friend class OutputPort;
      friend class DynParaLoop;
      friend class ForEachLoop;
      friend class SplitterNode;
      friend class ComposedNode;
      friend class OptimizerLoop;
      friend class ElementaryNode; //for removeAllLinksWithMe
      friend class CollectorSwOutPort;
      friend class OutputDataStreamPort;
      friend class InterceptorInputPort;
    public:
      virtual InPort *getPublicRepresentant() { return this; }
      virtual int edGetNumberOfLinks() const;
      virtual std::set<OutPort *> edSetOutPort() const;
      virtual ~InPort();
    protected:
      InPort(const InPort& other, Node *newHelder);
      InPort(const std::string& name, Node *node, TypeCode* type);
      void edRemoveAllLinksLinkedWithMe() throw(Exception);
      virtual void edNotifyReferencedBy(OutPort *fromPort);
      virtual void edNotifyDereferencedBy(OutPort *fromPort);
      virtual void getAllRepresentants(std::set<InPort *>& repr) const;
    protected:
      std::set<OutPort *> _backLinks;
    };
  }
}

#endif
