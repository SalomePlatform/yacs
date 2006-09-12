#ifndef __TOYNODE_HXX__
#define __TOYNODE_HXX__

#include "ElementaryNode.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class ToyNode : public ElementaryNode
    {
    private:
      int _time;
    public:
      ToyNode(const std::string& name, int time=0);
      void setTime(int time) { _time=time; }
      InputPort *edAddInputPort(const std::string& inputPortName);
      OutputPort *edAddOutputPort(const std::string& outputPortName);
      InputPort *edAddInputPort(const std::string& inputPortName, YACS::DynType type) throw(Exception);
      OutputPort *edAddOutputPort(const std::string& outputPortName, YACS::DynType type) throw(Exception);
      InputDataStreamPort *edAddInputDataStreamPort(const std::string& inputPortDSName, YACS::StreamType type) throw(Exception);
      OutputDataStreamPort *edAddOutputDataStreamPort(const std::string& outputPortDSName, YACS::StreamType type) throw(Exception);
      void execute();
    };
  }
}

#endif
