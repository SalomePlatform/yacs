#ifndef __INPUTDATASTREAMPORT_HXX__
#define __INPUTDATASTREAMPORT_HXX__

#include "InPort.hxx"
#include "DataStreamPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
    /*! \brief Class for Input DataStream Ports
     *
     * \ingroup Ports
     *
     */
    class InputDataStreamPort : public DataStreamPort, public InPort
    {
    public:
      static const char NAME[];
    public:
      InputDataStreamPort(const InputDataStreamPort& other, Node *newHelder);
      InputDataStreamPort(const std::string& name, Node *node, TypeCode* type);
      virtual ~InputDataStreamPort();
      virtual std::string getNameOfTypeOfCurrentInstance() const;
      virtual InputDataStreamPort *clone(Node *newHelder) const;
    };
  }
}

#endif
