#ifndef _STUDYNODES_HXX_
#define _STUDYNODES_HXX_

#include "DataNode.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class StudyInNode: public DataNode
    {
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      StudyInNode(const std::string& name);
      StudyInNode(const StudyInNode& other, ComposedNode *father);
      virtual void execute();
      virtual void accept(Visitor *visitor);
      virtual void checkBasicConsistency() const throw(Exception);
      virtual void setData(OutputPort* port, std::string& data);
      virtual OutputPort *createOutputPort(const std::string& outputPortName, TypeCode* type);
    public:
      static const char IMPL_NAME[];
    };
    class StudyOutNode: public DataNode
    {
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      StudyOutNode(const std::string& name);
      StudyOutNode(const StudyOutNode& other, ComposedNode *father);
      virtual void execute();
      virtual void accept(Visitor *visitor);
      virtual void checkBasicConsistency() const throw(Exception);
      virtual void setData(InputPort* port, std::string& data);
      virtual InputPort *createInputPort(const std::string& inputPortName, TypeCode* type);
    public:
      static const char IMPL_NAME[];
    };
  }
}

#endif
