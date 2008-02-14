
#ifndef _NODEPARSERS_HXX_
#define _NODEPARSERS_HXX_

#include "parserBase.hxx"
#include "containerParsers.hxx"
#include "dataParsers.hxx"
#include "portParsers.hxx"
#include "codeParsers.hxx"
#include "propertyParsers.hxx"

#include "Proc.hxx"
#include "TypeCode.hxx"
#include "InlineNode.hxx"
#include "ServiceNode.hxx"
#include "ServiceInlineNode.hxx"
#include "Exception.hxx"
#include "Runtime.hxx"
#include "OutputDataStreamPort.hxx"
#include "InputDataStreamPort.hxx"
#include "ComponentInstance.hxx"
#include "factory.hxx"

extern YACS::ENGINE::Proc* currentProc;
extern YACS::ENGINE::Runtime* theRuntime;

namespace YACS
{

/*! \brief Class for node parser.
 *
 *  This class is a base class for other parsers
 */
template <class T=YACS::ENGINE::InlineNode*>
struct nodetypeParser: parser
{
  static nodetypeParser<T> nodeParser;

  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      DEBTRACE( "nodetypeParser::onStart: " << el )             
      std::string element(el);
      parser* pp=&parser::main_parser;
      this->SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  virtual void onEnd(const char *el,parser* child)
    {
      DEBTRACE( "nodetypeParser::onEnd: " << el )             
      std::string element(el);
    }
  virtual void buildAttr(const XML_Char** attr)
    {
      required("name",attr);
      required("type",attr);
      for (int i = 0; attr[i]; i += 2) 
      {
        if(std::string(attr[i]) == "name")name(attr[i+1]);
        if(std::string(attr[i]) == "state")state(attr[i+1]);
        if(std::string(attr[i]) == "type")type(attr[i+1]);
      }
    }
  virtual void pre()
    {
      _node=0;
      _container="";
    }
  virtual void name (const std::string& name)
    {
      DEBTRACE( "inline_name: " << name )             
      _name=name;
    }
  virtual void state (const std::string& name)
    {
      _state=name;
    }
  virtual void type (const std::string& name)
    {
      DEBTRACE( "node_type " << name )             
      _type=name;
    }
  virtual void property (const myprop& prop);
  virtual T post();
  std::string _type;
  std::string _name;
  std::string _state;
  std::string _container;
  T _node;
};

template <class T> nodetypeParser<T> nodetypeParser<T>::nodeParser;

template <class T>
void nodetypeParser<T>::property (const myprop& prop)
{
  _node->setProperty(prop._name,prop._value);
}

template <class T>
T nodetypeParser<T>::post()
{
  return _node;
}

template <>
YACS::ENGINE::InlineNode* nodetypeParser<YACS::ENGINE::InlineNode*>::post ()
{
  std::string fullname = currentProc->names.back()+_type;
  if(currentProc->inlineMap.count(_type) != 0)
    {
      //InlineNode type with absolute name found
      YACS::ENGINE::InlineNode* n=currentProc->inlineMap[_type];
      _node=n->cloneNode(_name);
    }
  else if(currentProc->inlineMap.count(fullname) != 0)
    {
      //InlineNode type with relative name found
      YACS::ENGINE::InlineNode* n=currentProc->inlineMap[fullname];
      _node=n->cloneNode(_name);
    }
  else
    {
      throw YACS::Exception("Unknown InlineNode type");
    }
  if(_state == "disabled")_node->exDisabledState();
  DEBTRACE( "node_post " << _node->getName() )             
  return _node;
}

static std::string t1[]={"script","function",""};

template <class T=YACS::ENGINE::InlineNode*>
struct inlinetypeParser:public nodetypeParser<T>
{
  static inlinetypeParser<T> inlineParser;

  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child)
    {
      DEBTRACE( "inlinetypeParser::onEnd: " << el )             
      std::string element(el);
      if(element == "kind")kind(((stringtypeParser*)child)->post());
      else if(element == "script")script(((codetypeParser*)child)->post());
      else if(element == "function")function(((functypeParser*)child)->post());
      else if(element == "inport") inport(((inporttypeParser<myinport>*)child)->post());
      else if(element == "outport") outport(((outporttypeParser<myoutport>*)child)->post());
    }
  virtual void buildAttr(const XML_Char** attr)
    {
      this->required("name",attr);
      for (int i = 0; attr[i]; i += 2) 
      {
        if(std::string(attr[i]) == "name")this->name(attr[i+1]);
        if(std::string(attr[i]) == "state")this->state(attr[i+1]);
      }
    }
  virtual void pre ()
    {
      this->_node=0;
      _kind="";
      this->_state="";
      this->_container="";
    }
  virtual void kind (const std::string& name)
    {
      DEBTRACE( "inline_kind " << name )             
      _kind=name;
    }

  virtual void script (const myfunc& f){}
  virtual void function (const myfunc& f) {}

  virtual void inport (const myinport& p)
    {
      DEBTRACE( "inline_inport: " << p._name <<":"<<p._type)             
      if(this->_node==0)
        throw YACS::Exception("Node must be completely defined before defining its ports");

      if(currentProc->typeMap.count(p._type)==0)
      {
        //Check if the typecode is defined in the runtime
        YACS::ENGINE::TypeCode* t=theRuntime->getTypeCode(p._type);
        if(t==0)
        {
          std::string msg="Unknown InPort Type: ";
          msg=msg+p._type+" for node: "+this->_node->getName()+" port name: "+p._name;
          throw YACS::Exception(msg);
        }
        else
        {
          currentProc->typeMap[p._type]=t;
          t->incrRef();
        }
      }
      this->_node->edAddInputPort(p._name,currentProc->typeMap[p._type]);
    }
  virtual void outport (const myoutport& p)
    {
      DEBTRACE( "inline_outport: " << p._name <<":"<<p._type)             
      if(this->_node==0)
        throw YACS::Exception("Node must be completely defined before defining its ports");

      if(currentProc->typeMap.count(p._type)==0)
      {
        YACS::ENGINE::TypeCode* t=theRuntime->getTypeCode(p._type);
        if(t==0)
        {
          std::string msg="Unknown OutPort Type: ";
          msg=msg+p._type+" for node: "+this->_node->getName()+" port name: "+p._name;
          throw YACS::Exception(msg);
        }
        else
        {
          currentProc->typeMap[p._type]=t;
          t->incrRef();
        }
      }
      this->_node->edAddOutputPort(p._name,currentProc->typeMap[p._type]);
    }
  virtual T post()
    {
      DEBTRACE( "inline_post " << this->_node->getName() )             
      if(this->_state == "disabled")this->_node->exDisabledState();
      /*
      std::list<OutputPort *>::iterator iter;
      std::list<OutputPort *> s=_node->getSetOfOutputPort();
      for(iter=s.begin();iter!=s.end();iter++)
        {
          std::cerr << "port name: " << (*iter)->getName() << std::endl;
          std::cerr << "port kind: " << (*iter)->edGetType()->kind() << std::endl;
        }
        */
      return this->_node;
    }
  std::string _kind;
};

template <class T> inlinetypeParser<T> inlinetypeParser<T>::inlineParser;

template <>
void inlinetypeParser<YACS::ENGINE::InlineNode*>::script (const myfunc& f)
{
  DEBTRACE( "inline_script: " << f._code )             
  _node=theRuntime->createScriptNode(_kind,_name);
  _node->setScript(f._code);
}

template <>
void inlinetypeParser<YACS::ENGINE::InlineNode*>::function (const myfunc& f)
{
  DEBTRACE( "inline_function: " << f._code )             
  YACS::ENGINE::InlineFuncNode *fnode;
  fnode=theRuntime->createFuncNode(_kind,_name);
  fnode->setScript(f._code);
  fnode->setFname(f._name);
  _node=fnode;
}

/*! \brief Class for parsing ServiceInlineNode description
 *
 *  
 */
template <class T=YACS::ENGINE::ServiceInlineNode*>
struct sinlinetypeParser:public inlinetypeParser<T>
{
  static sinlinetypeParser<T> sinlineParser;

  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child)
    {
      DEBTRACE( "sinlinetypeParser::onEnd: " << el )             
      std::string element(el);
      if(element == "kind")this->kind(((stringtypeParser*)child)->post());
      else if(element == "function")this->function(((functypeParser*)child)->post());
      else if(element == "load") load(((loadtypeParser*)child)->post());
      else if(element == "inport") this->inport(((inporttypeParser<myinport>*)child)->post());
      else if(element == "outport") this->outport(((outporttypeParser<myoutport>*)child)->post());
    }
  //virtual void service (const myfunc& f) {}
  virtual void load (const loadon& l)
    {
      DEBTRACE( "sinline_load: " )             
      this->_container=l._container;
    }
  virtual T post()
    {
      DEBTRACE( "sinline_post " << this->_node->getName() );
      if(this->_state == "disabled")this->_node->exDisabledState();

      if(currentProc->containerMap.count(this->_container) != 0)
        {
          this->_node->getComponent()->setContainer(currentProc->containerMap[this->_container]);
        }
      else if(this->_container == "")
        {
          if(currentProc->containerMap.count("DefaultContainer") != 0)
          {
            //a default container is defined : use it
            this->_node->getComponent()->setContainer(currentProc->containerMap["DefaultContainer"]);
          }
        }
      else
        {
          std::cerr << "WARNING: Unknown container " << this->_container << " ignored" << std::endl;
        }

      return this->_node;
    }
};
template <class T> sinlinetypeParser<T> sinlinetypeParser<T>::sinlineParser;

// sinline ????
template <>
void inlinetypeParser<YACS::ENGINE::ServiceInlineNode*>::function (const myfunc& f)
{
  DEBTRACE( "sinline_function: " << f._code )             
  YACS::ENGINE::ServiceInlineNode *fnode;
  fnode=theRuntime->createSInlineNode(_kind,_name);
  fnode->setScript(f._code);
  fnode->setMethod(f._name);
  fnode->setComponent(theRuntime->createComponentInstance("PyCompo","SalomePy"));
  //fnode->setRef("PyCompo");
  _node=fnode;
}

static std::string t2[]={"ref","node","component",""};

template <class T=YACS::ENGINE::ServiceNode*>
struct servicetypeParser:public inlinetypeParser<T>
{
  static servicetypeParser<T> serviceParser;

  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child)
    {
      DEBTRACE( "servicetypeParser::onEnd: " << el )             
      std::string element(el);
      if(element == "kind")this->kind(((stringtypeParser*)child)->post());
      else if(element == "ref") ref(((stringtypeParser*)child)->post());
      else if(element == "component") component(((stringtypeParser*)child)->post());
      else if(element == "node") node(((stringtypeParser*)child)->post());
      else if(element == "method") method(((stringtypeParser*)child)->post());
      else if(element == "load") load(((loadtypeParser*)child)->post());
      else if(element == "inport") this->inport(((inporttypeParser<myinport>*)child)->post());
      else if(element == "outport") this->outport(((outporttypeParser<myoutport>*)child)->post());
      else if(element == "instream") instream(((inporttypeParser<myinport>*)child)->post());
      else if(element == "outstream") outstream(((outporttypeParser<myoutport>*)child)->post());
    }
  virtual void ref (const std::string& name)
    {
      DEBTRACE( "service_ref: " << name )             
      this->_node=theRuntime->createRefNode(this->_kind,this->_name);
      this->_node->setRef(name);
    }
  virtual void component (const std::string& name)
    {
      DEBTRACE( "service_component: " << name )             
      this->_node=theRuntime->createCompoNode(this->_kind,this->_name);
      this->_node->setRef(name);
    }
  virtual void node (const std::string& name)
    {
      DEBTRACE( "service_node: " << name )             
      std::string fullname = currentProc->names.back()+name;
      if(currentProc->serviceMap.count(name) != 0)
        {
          //ServiceNode with absolute name found
          YACS::ENGINE::ServiceNode* n=currentProc->serviceMap[name];
          this->_node =n->createNode(this->_name);
        }
      else if(currentProc->serviceMap.count(fullname) != 0)
        {
          //ServiceNode with relative name found
          //TODO: must be a short name (possible only in the same context)
          YACS::ENGINE::ServiceNode* n=currentProc->serviceMap[fullname];
          this->_node =n->createNode(this->_name);
        }
      else
        {
          throw YACS::Exception("Unknown ServiceNode");
        }
    }

  virtual void method (const std::string& name)
    {
      DEBTRACE( "service_method: " << name )             
      if(this->_node==0)
        throw YACS::Exception("ServiceNode must be completely defined before defining its method");
      if(name == "")
      {
        this->logError("a service name must be a non empty string");
        return;
      }
      this->_node->setMethod(name);
    }

  virtual void load (const loadon& l)
    {
      DEBTRACE( "service_load: " );
      this->_container=l._container;
    }

  virtual void instream (const myinport& p)
    {
      DEBTRACE( "service_instream" )             
      DEBTRACE( p._type )             
      DEBTRACE( p._name )             
      if(this->_node==0)
        throw YACS::Exception("ServiceNode must be completely defined before defining its ports");

      if(currentProc->typeMap.count(p._type)==0)
      {
        YACS::ENGINE::TypeCode* t=theRuntime->getTypeCode(p._type);
        if(t==0)
        {
          std::string msg="Unknown InStreamPort Type: ";
          msg=msg+p._type+" for node: "+this->_node->getName()+" port name: "+p._name;
          throw YACS::Exception(msg);
        }
        else
        {
          currentProc->typeMap[p._type]=t;
          t->incrRef();
        }
      }
      YACS::ENGINE::InputDataStreamPort* port;
      port=this->_node->edAddInputDataStreamPort(p._name,currentProc->typeMap[p._type]);
      // Set all properties for this port
      std::map<std::string, std::string>::const_iterator pt;
      for(pt=p._props.begin();pt!=p._props.end();pt++)
        port->setProperty((*pt).first,(*pt).second);
    }
  virtual void outstream (const myoutport& p)
    {
      DEBTRACE( "service_outstream" )             
      DEBTRACE( p._type )             
      DEBTRACE( p._name )             
      if(this->_node==0)
        throw YACS::Exception("ServiceNode must be completely defined before defining its ports");

      if(currentProc->typeMap.count(p._type)==0)
      {
        YACS::ENGINE::TypeCode* t=theRuntime->getTypeCode(p._type);
        if(t==0)
        {
          std::string msg="Unknown OutStreamPort Type: ";
          msg=msg+p._type+" for node: "+this->_node->getName()+" port name: "+p._name;
          throw YACS::Exception(msg);
        }
        else
        {
          currentProc->typeMap[p._type]=t;
          t->incrRef();
        }
      }
      YACS::ENGINE::OutputDataStreamPort* port;
      port=this->_node->edAddOutputDataStreamPort(p._name,currentProc->typeMap[p._type]);
      // Set all properties for this port
      std::map<std::string, std::string>::const_iterator pt;
      for(pt=p._props.begin();pt!=p._props.end();pt++)
        port->setProperty((*pt).first,(*pt).second);
    }
  virtual T post()
    {
      DEBTRACE( "service_post " << this->_node->getName() )             
      this->mincount("method",1);
      if(this->_state == "disabled")this->_node->exDisabledState();

      if(currentProc->containerMap.count(this->_container) != 0)
        {
          this->_node->getComponent()->setContainer(currentProc->containerMap[this->_container]);
        }
      else if(this->_container == "")
        {
          if(currentProc->containerMap.count("DefaultContainer") != 0)
          {
            //a default container is defined : use it
            this->_node->getComponent()->setContainer(currentProc->containerMap["DefaultContainer"]);
          }
        }
      else
        {
          std::cerr << "WARNING: Unknown container " << this->_container << " ignored" << std::endl;
        }
      return this->_node;
    }
};

template <class T> servicetypeParser<T> servicetypeParser<T>::serviceParser;

}


namespace YACS
{
template <class T>
void inlinetypeParser<T>::onStart(const XML_Char* el, const XML_Char** attr)

    {
      DEBTRACE( "inlinetypeParser::onStart: " << el )
      std::string element(el);
      parser* pp=&parser::main_parser;
      this->maxcount("kind",1,element);
      this->maxcount("script",1,element);
      this->maxcount("function",1,element);
      this->maxchoice(t1,1,element);
      if(element == "kind")pp=&stringtypeParser::stringParser;
      else if(element == "script")pp=&codetypeParser::codeParser;
      else if(element == "function")pp=&functypeParser::funcParser;
      else if(element == "inport")pp=&inporttypeParser<>::inportParser;
      else if(element == "outport")pp=&outporttypeParser<>::outportParser;
      this->SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }

template <class T>
void sinlinetypeParser<T>::onStart(const XML_Char* el, const XML_Char** attr)
    {
      DEBTRACE( "sinlinetypeParser::onStart: " << el )
      std::string element(el);
      parser* pp=&parser::main_parser;
      this->maxcount("kind",1,element);
      this->maxcount("function",1,element);
      this->maxcount("load",1,element);
      if(element == "kind")pp=&stringtypeParser::stringParser;
      else if(element == "function")pp=&functypeParser::funcParser;
      else if(element == "load")pp=&loadtypeParser::loadParser;
      else if(element == "inport")pp=&inporttypeParser<>::inportParser;
      else if(element == "outport")pp=&outporttypeParser<>::outportParser;
      this->SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }

template <class T>
void servicetypeParser<T>::onStart(const XML_Char* el, const XML_Char** attr)
    {
      DEBTRACE( "servicetypeParser::onStart: " << el )
      std::string element(el);
      parser* pp=&parser::main_parser;
      this->maxcount("kind",1,element);
      this->maxcount("ref",1,element);
      this->maxcount("node",1,element);
      this->maxcount("component",1,element);
      this->maxcount("method",1,element);
      this->maxcount("load",1,element);
      this->maxchoice(t2,1,element);
      if(element == "kind")pp=&stringtypeParser::stringParser;
      else if(element == "ref")pp=&stringtypeParser::stringParser;
      else if(element == "component")pp=&stringtypeParser::stringParser;
      else if(element == "node")pp=&stringtypeParser::stringParser;
      else if(element == "method")pp=&stringtypeParser::stringParser;
      else if(element == "load")pp=&loadtypeParser::loadParser;
      else if(element == "inport")pp=&inporttypeParser<>::inportParser;
      else if(element == "outport")pp=&outporttypeParser<>::outportParser;
      else if(element == "instream")pp=&inporttypeParser<>::inportParser;
      else if(element == "outstream")pp=&outporttypeParser<>::outportParser;
      this->SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }




}

#endif
