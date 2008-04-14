#ifndef __OUTPUTDATASTREAMPORT_HXX__
#define __OUTPUTDATASTREAMPORT_HXX__

#include "OutPort.hxx"
#include "DataStreamPort.hxx"
#include "ConversionException.hxx"

#include <set>

namespace YACS
{
  namespace ENGINE
  {
    class ElementaryNode;
    class InputDataStreamPort;

    class OutputDataStreamPort : public DataStreamPort, public OutPort
    {
      friend class ElementaryNode;
    protected:
      std::set<InputDataStreamPort *> _setOfInputDataStreamPort;
    public:
      static const char NAME[];
    public:
      OutputDataStreamPort(const OutputDataStreamPort& other, Node *newHelder);
      OutputDataStreamPort(const std::string& name, Node *node, TypeCode* type);
      virtual ~OutputDataStreamPort();
      virtual OutputDataStreamPort *clone(Node *newHelder) const;
      std::set<InPort *> edSetInPort() const;
      bool isAlreadyLinkedWith(InPort *with) const;
      virtual std::string getNameOfTypeOfCurrentInstance() const;
      virtual bool addInPort(InPort *inPort) throw(Exception);
      virtual bool edAddInputDataStreamPort(InputDataStreamPort *port) throw(ConversionException);
      int edRemoveInputDataStreamPort(InputDataStreamPort *inPort, bool forward) throw(Exception);
      void edRemoveAllLinksLinkedWithMe() throw(Exception);
      int removeInPort(InPort *inPort, bool forward) throw(Exception);
      virtual std::string typeName() {return "YACS__ENGINE__OutputDataStreamPort";}
    private:
      bool isAlreadyInSet(InputDataStreamPort *inPort) const;
    };
  }
}

#endif
