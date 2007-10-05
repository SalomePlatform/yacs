#ifndef __RUNTIMEFORENGINEINTEGRATIONTEST_HXX__
#define __RUNTIMEFORENGINEINTEGRATIONTEST_HXX__

#include "Runtime.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class RuntimeForEngineIntegrationTest : public Runtime
    {
    public:
      static void setRuntime();
      ElementaryNode* createNode(const std::string& implementation, const std::string& name) throw(Exception);
      InputPort* createInputPort(const std::string& name, const std::string& impl, Node * node, TypeCode * type);
      OutputPort* createOutputPort(const std::string& name, const std::string& impl, Node * node, TypeCode * type);
      InputPort* adapt(InputPort* source, const std::string& impl,TypeCode * type) throw (ConversionException);
      ComponentInstance* createComponentInstance(const std::string& name, const std::string& kind="");
    };
  }
}

#endif
