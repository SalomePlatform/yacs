#ifndef __DATAFLOWPORT_HXX__
#define __DATAFLOWPORT_HXX__

#include "DataPort.hxx"

#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class DataFlowPort : public virtual DataPort
    {
    public:
      static const char NAME[];
    protected:
      DataFlowPort(const DataFlowPort& other, Node *newHelder);
      DataFlowPort(const std::string& name, Node *node, TypeCode* type);
    public:
      std::string getNameOfTypeOfCurrentInstance() const;
      TypeOfChannel getTypeOfChannel() const { return DATAFLOW; }
      virtual ~DataFlowPort();
      virtual std::string typeName() {return "YACS__ENGINE__DataFlowPort";}
    };
  }
}

#endif
