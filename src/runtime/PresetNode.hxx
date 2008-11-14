
#ifndef _PRESETNODE_HXX_
#define _PRESETNODE_HXX_

#include "DataNode.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class PresetNode: public DataNode
    {
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      PresetNode(const std::string& name);
      PresetNode(const PresetNode& other, ComposedNode *father);
      virtual void execute();
      virtual void accept(Visitor *visitor);
      virtual void checkBasicConsistency() const throw(Exception);
      virtual void setData(OutputPort* port, const std::string& data);
      virtual OutputPort *createOutputPort(const std::string& outputPortName, TypeCode* type);
      virtual std::string typeName() {return "YACS__ENGINE__PresetNode";}
    public:
      static const char IMPL_NAME[];
    };
  }
}

#endif
