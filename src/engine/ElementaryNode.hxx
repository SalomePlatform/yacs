#ifndef __ELEMENTARYNODE_HXX__
#define __ELEMENTARYNODE_HXX__

#include "Node.hxx"
#include "Task.hxx"
#include "define.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class Port;
    class InputPort;
    class OutputPort;
    class ComposedNode;
    class InputDataStreamPort;
    class OutputDataStreamPort;

    class ElementaryNode : public Node, public Task
    {
      friend class ComposedNode;
    protected:
      ElementaryNode(const std::string& name);
      ~ElementaryNode();
    public:
      void getReadyTasks(std::vector<Task *>& tasks);
      void edRemovePort(Port *port) throw(Exception);
      std::list<ElementaryNode *> getRecursiveConstituents();
      virtual InputPort *edAddInputPort(const std::string& inputPortName, YACS::DynType type) throw(Exception);
      virtual OutputPort *edAddOutputPort(const std::string& outputPortName, YACS::DynType type) throw(Exception);
      virtual InputDataStreamPort *edAddInputDataStreamPort(const std::string& inputPortDSName, YACS::StreamType type) throw(Exception);
      virtual OutputDataStreamPort *edAddOutputDataStreamPort(const std::string& outputPortDSName, YACS::StreamType type) throw(Exception);
      //run part
      void begin();
      bool isReady();
      void finished();
    protected:
      void disconnectAllLinksConnectedTo(Node *node);
    };
  }
}

#endif
