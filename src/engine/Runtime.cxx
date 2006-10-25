#include "Runtime.hxx"

#include<cassert>

using namespace YACS::ENGINE;
using namespace std;


Runtime* Runtime::_singleton = 0;

void Runtime::setRuntime() // singleton creation (not thread safe!)
{
  if (! Runtime::_singleton) Runtime::_singleton = new Runtime();
}

// singleton creation must be done before by a derived class

Runtime* YACS::ENGINE::getRuntime() throw(Exception)
{
  if ( !  Runtime::_singleton )
    throw Exception("Runtime is not yet initialized");
  return Runtime::_singleton;
}

ElementaryNode* Runtime::createNode(string implementation,
				    string name) throw(Exception)
{
  return new TestElemNode(name);
}
      
InputPort* Runtime::createInputPort(const string& name,
				    const string& impl,
				    Node * node,
				    TypeCode * type)
{
  return new InputPort(name, node, type);
}

OutputPort* Runtime::createOutputPort(const string& name,
				      const string& impl,
				      Node * node,
				      TypeCode * type)
{
  return new OutputPort(name, node, type);
}

InputPort* Runtime::adapt(const string& imp_source, 
			  InputPort* source,
			  const string& impl,TypeCode * type)
  throw (ConversionException)
{
  return source;
}
