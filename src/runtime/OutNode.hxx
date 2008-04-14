
#ifndef _OUTNODE_HXX_
#define _OUTNODE_HXX_

#include "DataNode.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class OutNode: public DataNode
    {
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      OutNode(const std::string& name);
      OutNode(const OutNode& other, ComposedNode *father);
      virtual void execute();
      virtual void accept(Visitor *visitor);
      virtual void checkBasicConsistency() const throw(Exception);
      virtual void setData(InputPort* port, std::string& data);
      virtual void dump(std::ostream &os);
      virtual InputPort *createInputPort(const std::string& inputPortName, TypeCode* type);
    public:
      static const char IMPL_NAME[];
    };
  }
}

#endif
