#ifdef USE_CPPUNIT
#undef USE_CPPUNIT
#endif

#include "runtimeTest.hxx"

int main()
{
  YACS::RuntimeTest T;
  T.setUp();
  T.initRuntimeTypeCode();
  T.createPythonNodes();
  T.createCORBANodes();
  T.createBloc();
  T.createRecursiveBlocs();
  T.createDataLinks();
  T.createPythonNodesWithScript();
  T.createCORBANodesWithMethod();
  T.createXMLNodes();
  T.createBloc2();
  T.createDataLinksPythonPython();
  /*
  T.createDataLinksPythonCORBA();
  T.createDataLinksCORBACORBA();
  T.createDataLinksCORBAPython();
  T.createDataLinksXML();
  T.manualInitInputPort();
  T.manualExecuteNoThread();
  T.manualGetOutputs();
  T.createCppNodes();
  T.convertPorts();
  T.executeCppNode();
  T.createGraphWithCppNodes();
  T.classTeardown();
*/  
  return 0;
}

