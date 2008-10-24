#include "RuntimeForEngineIntegrationTest.hxx"
#include "ComponentInstanceTest.hxx"
#include "ToyNode.hxx"
#include "TypeCode.hxx"
#include <sstream>

using namespace std;
using namespace YACS::ENGINE;

void RuntimeForEngineIntegrationTest::setRuntime()
{
  if (! Runtime::_singleton)
    Runtime::_singleton = new RuntimeForEngineIntegrationTest;
}

ElementaryNode* RuntimeForEngineIntegrationTest::createNode(const string& implementation, const string& name) throw(Exception)
{
  if (implementation == ToyNode::MY_IMPL_NAME)
    return new ToyNode(name);
  else if(implementation == LimitNode::MY_IMPL_NAME)
    return new LimitNode(name);
  string what="RuntimeForEngineIntegrationTest does not handle this implementation: " + implementation;
  throw Exception(what);
}

InputPort* RuntimeForEngineIntegrationTest::createInputPort(const string& name, const string& impl, Node * node, TypeCode * type)
{
  if(impl == ToyNode::MY_IMPL_NAME)
    {
      if(type->kind()!=Double)
        throw Exception("Invalid type");
      return new InputToyPort(name, node);
    }
  else if(impl == LimitNode::MY_IMPL_NAME)
    throw Exception("InputPort creation not allowed for LimitNode");
  ostringstream msg;
  msg << "Cannot create " << impl << " OutputPort" ;
  throw Exception(msg.str());
}

OutputPort* RuntimeForEngineIntegrationTest::createOutputPort(const string& name, const string& impl, Node * node, TypeCode * type)
{
  if(impl == ToyNode::MY_IMPL_NAME)
    {
      if(type->kind()!=Double && type->kind()!=Int)
        throw Exception("Invalid type");
      return new OutputToyPort(name, node, type);
    }
  else if(impl == LimitNode::MY_IMPL_NAME)
    throw Exception("OutputPort creation not allowed for LimitNode");
  stringstream msg;
  msg << "Cannot create " << impl << " OutputPort" ;
  throw Exception(msg.str());
}

InputPort* RuntimeForEngineIntegrationTest::adapt(InputPort* source, const string& impl,TypeCode * type,bool init) throw (ConversionException)
{
  return new ProxyPort(source);
}

ComponentInstance* RuntimeForEngineIntegrationTest::createComponentInstance(const std::string& name, const std::string& kind)
{
  if(kind==ToyNode1S::KIND)
    return new ComponentInstanceTest1(name);
  else if(kind==ToyNode2S::KIND)
    return new ComponentInstanceTest2(name);
  else
    {
      string msg("RuntimeForEngineIntegrationTest::createComponentInstance : Unable to crate component with kind \"");
      msg+=kind; msg+="\"";
      throw Exception(msg);
    }
}
