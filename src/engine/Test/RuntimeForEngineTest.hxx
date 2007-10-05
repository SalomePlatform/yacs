#ifndef __RUNTIMEFORENGINETEST_HXX__
#define __RUNTIMEFORENGINETEST_HXX__

#include "Runtime.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "ElementaryNode.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class TestElemNode: public ElementaryNode
    {
    public:
      TestElemNode(const std::string& s): ElementaryNode(s) { }
      TestElemNode(const TestElemNode& other, ComposedNode *father);
      void execute() { }
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    };

    class TestElemInputPort : public InputPort
    {
    public:
      TestElemInputPort(const std::string& name, Node *node, TypeCode* type);
      TestElemInputPort(const TestElemInputPort& other, Node *newHelder);
      void put(const void *data) throw(ConversionException);
      InputPort *clone(Node *newHelder) const;
      void *get() const throw(Exception);
      void exRestoreInit();
      void exSaveInit();
    protected:
      Any *_value;
    };

    class TestElemOutputPort : public OutputPort
    {
    public:
      TestElemOutputPort(const std::string& name, Node *node, TypeCode* type);
      TestElemOutputPort(const TestElemOutputPort& other, Node *newHelder);
      void put(const void *data) throw(ConversionException);
      OutputPort *clone(Node *newHelder) const;
    };
    
    class RuntimeForEngineTest : public Runtime
    {
    public:
      static void setRuntime();
      ElementaryNode* createNode(const std::string& implementation, const std::string& name) throw(Exception);
      InputPort* createInputPort(const std::string& name, const std::string& impl, Node * node, TypeCode * type);
      OutputPort* createOutputPort(const std::string& name, const std::string& impl, Node * node, TypeCode * type);
      InputPort* adapt(InputPort* source, const std::string& impl, TypeCode * type) throw (ConversionException);
    };
  }
}

#endif
