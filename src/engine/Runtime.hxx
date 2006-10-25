
#ifndef _RUNTIME_HXX_
#define _RUNTIME_HXX_

#include "TypeCode.hxx"
#include "ElementaryNode.hxx"
#include "OutputPort.hxx"
#include "InputPort.hxx"

#include<string>
#include<set>
//#include<utility>
#include<map>



namespace YACS
{
  namespace ENGINE
  {
    class Runtime;
    
    Runtime* getRuntime() throw(Exception);
    //! For unit testing purpose

    class TestElemNode: public ElementaryNode
    {
    public:
      TestElemNode(const std::string& s): ElementaryNode(s) {};
      void execute() {};
    };


    
    class Runtime
    {
    public:
      static void setRuntime(); // singleton creation
      
      friend Runtime* getRuntime() throw(Exception); // singleton creation
      
      virtual void init(){};
      virtual void fini(){};

      virtual ElementaryNode* createNode(std::string implementation,
					 std::string name) throw(Exception);
      
      virtual InputPort* createInputPort(const std::string& name,
					 const std::string& impl,
					 Node * node,
					 TypeCode * type);

      virtual OutputPort* createOutputPort(const std::string& name,
					   const std::string& impl,
					   Node * node,
					   TypeCode * type);

      virtual InputPort* adapt(const std::string& imp_source, 
			       InputPort* source,
			       const std::string& impl,TypeCode * type)
	throw (ConversionException);

    protected:
      static Runtime* _singleton;
      Runtime() {};
      std::set<std::string> _setOfImplementation;
    };
  }
}
#endif
