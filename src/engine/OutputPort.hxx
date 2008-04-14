#ifndef __OUTPUTPORT_HXX__
#define __OUTPUTPORT_HXX__

#include "OutPort.hxx"
#include "DataFlowPort.hxx"
#include "ConversionException.hxx"

#include <set>

namespace YACS
{
  namespace ENGINE
  {
    class InPort;
    class Runtime;
    class InputPort;
    class OptimizerLoop;
    class ElementaryNode;
    class CollectorSwOutputPort;

    class OutputPort : public DataFlowPort, public OutPort
    {
      friend class CollectorSwOutputPort; // for conect
      friend class ElementaryNode;        // for disconnect...
      friend class OptimizerLoop;         // for interceptors
      friend class InputPort;
      friend class Runtime;               // for port creation
    public:
      virtual ~OutputPort();
      std::set<InPort *> edSetInPort() const;
      bool isAlreadyLinkedWith(InPort *with) const;
      bool isAlreadyInSet(InputPort *inputPort) const;
      std::string getNameOfTypeOfCurrentInstance() const;
      int removeInPort(InPort *inPort, bool forward) throw(Exception);
      virtual bool edAddInputPort(InputPort *phyPort) throw(Exception);
      virtual int edRemoveInputPort(InputPort *inputPort, bool forward) throw(Exception);
      bool addInPort(InPort *inPort) throw(Exception);
      void edRemoveAllLinksLinkedWithMe() throw(Exception);//entry point for forward port deletion
      virtual void exInit();
      virtual void checkBasicConsistency() const throw(Exception);
      virtual OutputPort *clone(Node *newHelder) const = 0;
      virtual std::string dump();

      virtual void put(const void *data) throw(ConversionException);
      virtual std::string typeName() {return "YACS__ENGINE__OutputPort";}

    protected:
      OutputPort(const OutputPort& other, Node *newHelder);
      OutputPort(const std::string& name, Node *node, TypeCode* type);
    protected:
      const std::set<InputPort *>& getSetOfPhyLinks() const;
    protected:
      std::set<InputPort *> _setOfInputPort;//Key is for physical Data link
    public:
      static const char NAME[];
    };
  }
}

#endif
