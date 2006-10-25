#ifndef __INPUTDATASTREAMPORT_HXX__
#define __INPUTDATASTREAMPORT_HXX__

#include "InPort.hxx"
#include "DataStreamPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class InputDataStreamPort : public DataStreamPort, public InPort
    {
    public:
      static const char NAME[];
    public:
      InputDataStreamPort(const std::string& name, Node *node, TypeCode* type);
      std::string getNameOfTypeOfCurrentInstance() const;
    };
  }
}

#endif
