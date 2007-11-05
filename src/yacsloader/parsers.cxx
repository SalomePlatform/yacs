#include <stdio.h>
#include <stdexcept>
#include <expat.h>
#include <iostream>
#include <sstream>
#include <map>
#include <list>
#include <stdlib.h>
#include "parsers.hxx"
#include "factory.hxx"
#include "Runtime.hxx"
#include "Exception.hxx"
#include "Cstr2d.hxx"
#include "TypeCode.hxx"
#include "Loop.hxx"
#include "ForLoop.hxx"
#include "ForEachLoop.hxx"
#include "WhileLoop.hxx"
#include "Switch.hxx"
#include "Bloc.hxx"
#include "Proc.hxx"
#include "Logger.hxx"
#include "InlineNode.hxx"
#include "ServiceNode.hxx"
#include "ServiceInlineNode.hxx"
#include "OutputPort.hxx"
#include "InputPort.hxx"
#include "OutputDataStreamPort.hxx"
#include "InputDataStreamPort.hxx"
#include "ComponentInstance.hxx"
#include "Container.hxx"
#include "ProcCataLoader.hxx"

using namespace YACS;
using YACS::ENGINE::Runtime;
using YACS::ENGINE::getRuntime;

using YACS::ENGINE::TypeCode;
using YACS::Exception;
using YACS::ENGINE::TypeCodeObjref;
using YACS::ENGINE::TypeCodeStruct;
using YACS::ENGINE::Objref;
using YACS::ENGINE::InlineNode;
using YACS::ENGINE::InlineFuncNode;
using YACS::ENGINE::ServiceNode;
using YACS::ENGINE::ServiceInlineNode;
using YACS::ENGINE::Node;
using YACS::ENGINE::Loop;
using YACS::ENGINE::ForLoop;
using YACS::ENGINE::ForEachLoop;
using YACS::ENGINE::WhileLoop;
using YACS::ENGINE::Switch;
using YACS::ENGINE::Bloc;
using YACS::ENGINE::Proc;
using YACS::ENGINE::InputPort;
using YACS::ENGINE::OutputPort;
using YACS::ENGINE::InputDataStreamPort;
using YACS::ENGINE::OutputDataStreamPort;

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

Runtime* theRuntime=0;
static Proc* currentProc;

#define BUFFSIZE        8192

char Buff[BUFFSIZE];

XML_Parser p ;

std::stack<parser*> sp;

parser::~parser()
{
  if(_level==0)
    {
      delete _counts;
    }
  else
    {
      DEBTRACE("Problem with parser: final stack level should be 0 and not " << _level);
    }
}

void parser::SetUserDataAndPush(parser* pp)
{
  XML_SetUserData(p,pp);
  sp.push(pp);
}

void XMLCALL parser::start(void *data, const XML_Char* el, const XML_Char** attr)
{
  parser* pp=static_cast <parser *> (data);
  pp->incrCount(el);
  pp->onStart(el,attr);
}

void parser::onEnd(const XML_Char *el,parser* child)
{
  DEBTRACE("parser::onEnd: " << el)
}

void XMLCALL parser::end(void *data, const char *el)
{
  DEBTRACE("parser::end: " << el);
  parser* child=static_cast <parser *> (data);
  sp.pop();
  parser* pp=sp.top();
  XML_SetUserData(p,pp);
  pp->onEnd(el,child);
  child->end();
}

void parser::charData(const XML_Char *s, int len)
{
  _content=_content+std::string(s,len);
}

void XMLCALL parser::charac(void *data, const XML_Char *s, int len)
{
  parser* pp=static_cast <parser *> (data);
  pp->charData(s,len);
}

void parser::end ()
{
  _level=_level-1;
  if(_level>0)
    {
      delete _counts;
      _counts=_stackCount.top();
      _orderState=_stackOrder.top();
      _stackCount.pop();
      _stackOrder.pop();
    }
}

void parser::init ()
{
  if(_level>0)
    {
      _stackCount.push(_counts);
      _stackOrder.push(_orderState);
      _counts=new std::map<std::string,int>;
    }
  _level=_level+1;
  _counts->clear();
  _orderState=0;
}

void parser::incrCount(const XML_Char *el)
{
  if(_counts->count(el)==0)
    (*_counts)[el]=1;
  else
    (*_counts)[el]=(*_counts)[el]+1;
}

void parser::checkOrder(std::string& el)
{
  if(_orders.count(el)==0)return;
  if(_orders[el] < _orderState)
    {
      std::string msg="unexpected "+el+" element (wrong order)";
      throw YACS::Exception::Exception(msg);
    }
  else if(_orders[el] > _orderState)
    {
      _orderState=_orders[el];
    }
}

void parser::maxcount(std::string name, int max, std::string& el)
{
  if(el!=name)return;
  if((*_counts)[name]>max)
    {
      std::stringstream msg;
      msg <<"unexpected "+name+" element (count="<<(*_counts)[name];
      msg <<" > maxOccurs=" << max << ")";
      throw YACS::Exception::Exception(msg.str());
    }
}

void parser::mincount(std::string name,int min )
{
  if((*_counts)[name]<min)
    {
      std::stringstream msg;
      msg<<"expected "+name+" element (count="<<(*_counts)[name];
      msg << " < minOccurs=" << min << ")";
      throw YACS::Exception::Exception(msg.str());
    }
}

void parser::maxchoice(std::string *names, int max, std::string& el)
{
  int i=0;
  int ncount=0;
  while (names[i]!= "")
    {
      ncount=ncount+(*_counts)[names[i]];
      ++i;
    }
  if(ncount>max)
    {
      std::stringstream msg;
      msg<<"unexpected "+el+" element (choice count="<<ncount<<" > maxOccurs=" << max << ")";
      throw YACS::Exception::Exception(msg.str());
    }
}

void parser::minchoice(std::string *names, int min)
{
  int i=0;
  int ncount=0;
  while (names[i]!= "")
    {
      ncount=ncount+(*_counts)[names[i]];
      ++i;
    }
  if(ncount<min)
    {
      std::stringstream msg;
      msg << "expected element ";
      i=0;
      while (names[i]!= "")
        {
          msg << names[i] << ",";
          ++i;
        }
      msg << "(choice count="<<ncount<<" < minOccurs=" << min << ")";
      throw YACS::Exception::Exception(msg.str());
    }
}

void parser::required(const std::string& name, const XML_Char** attr)
{
  for (int i = 0; attr[i]; i += 2) 
    {
      if(name == std::string(attr[i]))return;
    }
  throw YACS::Exception::Exception("Attribute: "+name+" is required");
}

void parser::buildAttr(const XML_Char** attr)
{
  for (int i = 0; attr[i]; i += 2) 
    {
      DEBTRACE(attr[i] << "=" << attr[i + 1])
    }
}

void parser::onStart(const XML_Char* el, const XML_Char** attr)
{
  DEBTRACE( "parser::onStart: " << el )             
  XML_SetUserData(p,&main_parser);
  sp.push(&main_parser);
  main_parser.buildAttr(attr);
}

struct roottypeParser:parser
{
  void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child);
  virtual void proc (Proc* const& b)
    {
      DEBTRACE( "root_proc_set" << b->getName() )
      _proc=b;
    }
  Proc* _proc;
  const char* file;
};
static roottypeParser rootParser;

/*! \brief Class for string parser.
 *
 *  Class used to parse string
 */
struct stringtypeParser:parser
{
  std::string post()
    {
      return _content;
    }
};
static stringtypeParser stringParser;

/*! \brief Class for double parser.
 *
 *  Class used to parse double 
 */
struct doubletypeParser:parser
{
  double post()
    {
      return Cstr2d(_content.c_str());
//       std::istringstream s(_content.c_str());
//       double a;
//       if (!(s >> a))
//         throw YACS::Exception::Exception("problem in conversion from string to double");
//       std::cerr << "--------------_content s a "<< _content.c_str() << " " << s.str() << " " << a << std::endl;
//       return a;
    }
};
static doubletypeParser doubleParser;

/*! \brief Class for integer parser.
 *
 *  Class used to parse integer
 */
struct inttypeParser:parser
{
  int post()
    {
      return atoi(_content.c_str());
    }
};
static inttypeParser intParser;

/*! \brief Class for boolean parser.
 *
 *  Class used to parse bool
 */
struct booltypeParser:parser
{
  bool post()
    {
      DEBTRACE( _content )             
      if(_content == "true")return true;
      if(_content == "false")return false;
      std::stringstream temp(_content);
      bool b ;
      temp >> b;
      //std::cerr << b << std::endl;
      return b;
    }
};
static booltypeParser boolParser;

/*! \brief Class for property parser.
 *
 *  Class used to parse a property
 *  A property is a pair of name(string), value(string)
 *  XML schema is 
 *    <xsd:complexType name="PropertyType">
 *      <xsd:attribute name="name" type="xsd:string" use="required"/>
 *      <xsd:attribute name="value" type="xsd:string" use="required"/>
 *    </xsd:complexType>
 *
 */
struct propertytypeParser: parser
{
  virtual void buildAttr(const XML_Char** attr)
    {
      required("name",attr);
      required("value",attr);
      for (int i = 0; attr[i]; i += 2) 
      {
        if(std::string(attr[i]) == "name")name(attr[i+1]);
        if(std::string(attr[i]) == "value")value(attr[i+1]);
      }
    }
  virtual void name(const std::string& name){ _prop._name=name; }
  virtual void value(const std::string& name){ _prop._value=name; }
  myprop post(){return _prop;}
  myprop _prop;
};
static propertytypeParser propertyParser;

/*! \brief Class for type parser.
 *
 *  Class used to parse a type definition (class TypeCode in implementation)
 *  with a name and a kind (reserved to atomic types)
 *  XML schema is 
 *    <xsd:complexType name="TypeType">
 *      <xsd:attribute name="name" type="xsd:string" use="required"/>
 *      <xsd:attribute name="kind" type="xsd:string" use="required"/>
 *    </xsd:complexType>
 *
 */
struct typetypeParser: parser
{
  virtual void buildAttr(const XML_Char** attr)
    {
      required("name",attr);
      required("kind",attr);
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
          if(std::string(attr[i]) == "kind")kind(attr[i+1]);
        }
    }
  virtual void pre (){}
  virtual void name(const std::string& name)
    {
      DEBTRACE( "type_name: " << name )             
      _name=name;
    }
  virtual void kind(const std::string& name)
    {
      DEBTRACE( "type_kind: " << name )             
      _kind=name;
    }
  virtual mytype post()
    {
      DEBTRACE( "type_post" )             
      mytype t;
      t._kind=_kind;
      t._name=_name;
      return t;
    }
  std::string _name;
  std::string _kind;
};
static typetypeParser typeParser;

/*! \brief Class for sequence parser.
 *
 *  Class used to parse a sequence (type) definition (class TypeCodeSeq in implementation)
 *  XML schema is 
 *    <xsd:complexType name="SequenceType">
 *      <xsd:attribute name="name" type="xsd:string" use="required"/>
 *      <xsd:attribute name="content" type="xsd:string" use="required"/>
 *    </xsd:complexType>
 *
 */
struct seqtypeParser:public parser
{
  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      DEBTRACE( "seqtypeParser::onStart: " << el )             
      parser* pp=&main_parser;
      XML_SetUserData(p,pp);
      sp.push(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  virtual void onEnd(const char *el,parser* child)
    {
      DEBTRACE( "seqtypeParser::onEnd: " << el )             
    }
  virtual void buildAttr(const XML_Char** attr)
    {
      required("name",attr);
      required("content",attr);
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
          if(std::string(attr[i]) == "content")content(attr[i+1]);
        }
    }
  void name(const std::string& name)
    {
      DEBTRACE( "seqtype_name: " << name )             
      _name=name;
    }
  void content(const std::string& name)
    {
      DEBTRACE( "seqtype_content: " << name )             
      if(currentProc->typeMap.count(name)==0)
        {
          std::stringstream msg;
          msg << "Type " << name << " does not exist" << " (" <<__FILE__ << ":" << __LINE__ << ")";
          throw Exception(msg.str());
        }
      _contentType=currentProc->typeMap[name];

    }
  TypeCode* post()
    {
      DEBTRACE( "seqtype_post" )             
      TypeCode *t = currentProc->createSequenceTc(_name,_name,_contentType);
      return t;
    }
  TypeCode* _contentType;
  std::string _name;
};
static seqtypeParser seqParser;

/*! \brief Class for objref parser.
 *
 *  Class used to parse a objref (type) definition (class TypeCodeObjref in implementation)
 *  XML schema is 
 *    <xsd:complexType name="ObjrefType">
 *      <xsd:sequence>
 *        <xsd:element name="base" type="xsd:string" minOccurs="0" maxOccurs="unbounded"/>
 *      </xsd:sequence>
 *      <xsd:attribute name="name" type="xsd:string" use="required"/>
 *      <xsd:attribute name="id" type="xsd:string" />
 *    </xsd:complexType>
 *
 */
struct objtypeParser: parser
{
  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      DEBTRACE( "objtypeParser::onStart: " << el )             
      std::string element(el);
      parser* pp=&main_parser;
      if(element == "base")pp=&stringParser;
      XML_SetUserData(p,pp);
      sp.push(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  virtual void onEnd(const char *el,parser* child)
    {
      DEBTRACE( "objtypeParser::onEnd: " << el )             
      std::string element(el);
      if(element == "base")base(((stringtypeParser*)child)->post());
    }
  virtual void buildAttr(const XML_Char** attr)
    {
      required("name",attr);
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
          if(std::string(attr[i]) == "id")id(attr[i+1]);
        }
    }
  virtual void pre ()
    {
      _id="";
      _ltc.clear();
    }
  virtual void name(const std::string& name)
    {
      DEBTRACE( "objtype_name: " << name )             
      _name=name;
    }
  virtual void id(const std::string& name)
    {
      DEBTRACE( "objtype_id: " << name )             
      _id=name;
    }
  virtual void base(const std::string& name)
    {
      DEBTRACE( "base_name: " << name )             
      if(currentProc->typeMap.count(name)==0)
        {
          std::stringstream msg;
          msg << "Type " << name << " does not exist" ;
          msg << " (" <<__FILE__ << ":" << __LINE__ << ")";
          throw Exception(msg.str());
        }
      if(currentProc->typeMap[name]->kind() != Objref)
        {
          std::stringstream msg;
          msg << "Type " << name << " is not an objref" ;
          msg << " (" <<__FILE__ << ":" << __LINE__ << ")";
          throw Exception(msg.str());
        }
      _ltc.push_back((TypeCodeObjref *)currentProc->typeMap[name]);
    }
  virtual TypeCode * post()
    {
      DEBTRACE( "objtype_post" )             
      TypeCode *t = currentProc->createInterfaceTc(_id,_name,_ltc);
      return t;
    }
  std::string _name;
  std::string _id;
  std::list<TypeCodeObjref *> _ltc;
};
static objtypeParser objParser;

/*! \brief Class for member parser.
 *
 *  Class used to parse a struct member
 *  A struct member is a pair of name(string), type(string)
 *  XML schema is
 *    <xsd:complexType name="MemberType">
 *      <xsd:attribute name="name" type="xsd:string" use="required"/>
 *      <xsd:attribute name="type" type="xsd:string" use="required"/>
 *    </xsd:complexType>
 *
 */
struct membertypeParser: parser
{
  virtual void buildAttr(const XML_Char** attr)
    {
      required("name",attr);
      required("type",attr);
      for (int i = 0; attr[i]; i += 2)
      {
        if(std::string(attr[i]) == "name")name(attr[i+1]);
        if(std::string(attr[i]) == "type")type(attr[i+1]);
      }
    }
  virtual void name(const std::string& name){ _prop._name=name; }
  virtual void type(const std::string& name){ _prop._value=name; }
  myprop post(){return _prop;}
  myprop _prop;
};
static membertypeParser memberParser;

/*! \brief Class for struct parser.
 *
 *  Class used to parse a struct (type) definition (class TypeCodeStruct in implementation)
 *  XML schema is 
 *    <xsd:complexType name="StructType">
 *      <xsd:sequence>
 *        <xsd:element name="member" type="MemberType" minOccurs="0" maxOccurs="unbounded"/>
 *      </xsd:sequence>
 *      <xsd:attribute name="name" type="xsd:string" use="required"/>
 *      <xsd:attribute name="id" type="xsd:string" />
 *    </xsd:complexType>
 *
 */
struct structtypeParser: parser
{
  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      DEBTRACE( "structtypeParser::onStart: " << el )             
      std::string element(el);
      parser* pp=&main_parser;
      if(element == "member")pp=&memberParser;
      XML_SetUserData(p,pp);
      sp.push(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  virtual void onEnd(const char *el,parser* child)
    {
      DEBTRACE( "structtypeParser::onEnd: " << el )             
      std::string element(el);
      if(element == "member")member(((membertypeParser*)child)->post());
    }
  virtual void buildAttr(const XML_Char** attr)
    {
      required("name",attr);
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
          if(std::string(attr[i]) == "id")id(attr[i+1]);
        }
    }
  virtual void pre ()
    {
      _id="";
      _members.clear();
    }
  virtual void name(const std::string& name)
    {
      DEBTRACE( "structtype_name: " << name );
      _name=name;
    }
  virtual void id(const std::string& name)
    {
      DEBTRACE( "structtype_id: " << name );
      _id=name;
    }
  virtual void member (const myprop& prop)
    {
      DEBTRACE( "structtype_member: " << prop._name << prop._value );
      if(currentProc->typeMap.count(prop._value)!=0)
        _members.push_back(prop);
      else
        {
          std::string msg="Unknown type " + prop._value + " for member " + prop._name + " in struct " + _name;
          throw Exception(msg);
        }
    }
  virtual TypeCode * post()
    {
      DEBTRACE( "structtype_post" );
      TypeCodeStruct *t;
      if(currentProc->typeMap.count(_name)!=0)
        {
          //reuse a forward declaration
          TypeCode* tt=currentProc->typeMap[_name];
          if(tt->kind()==YACS::ENGINE::Struct)
            {
              t=(TypeCodeStruct*)tt;
            }
          else
            {
              std::string msg="Forward declaration must be a struct type but " + std::string(tt->name()) + " is not one" ;
              throw Exception(msg);
            }
        }
      else
        {
          t = (TypeCodeStruct*)currentProc->createStructTc(_id,_name);
        }
      std::vector<myprop>::const_iterator iter;
      for(iter=_members.begin();iter!=_members.end();iter++)
        {
          DEBTRACE("member: " << iter->_name << " " <<iter->_value);
          t->addMember(iter->_name,currentProc->typeMap[iter->_value]);
        }
      return t;
    }
  std::string _name;
  std::string _id;
  std::vector<myprop> _members;
};
static structtypeParser structParser;

/*! \brief Class for machine parser.
 *
 *  Class used to parse computer adress on which container must be started
 *  XML schema is 
 *    <xsd:complexType name="MachineType">
 *      <xsd:attribute name="name" type="xsd:string" use="required"/>
 *    </xsd:complexType>
 */
struct machinetypeParser: parser
{
  virtual void buildAttr(const XML_Char** attr)
    {
      required("name",attr);
      for (int i = 0; attr[i]; i += 2)
      {
        if(std::string(attr[i]) == "name")name(attr[i+1]);
      }
    }
  virtual void pre (){_mach._name="";}
  virtual void name(const std::string& name){ _mach._name=name; }
  machine post()
    {
      return _mach;
    }
  machine _mach;
};
static machinetypeParser machineParser;

/*! \brief Class for container parser
 *
 *  Class used to parse container description
 *  XML schema is 
 *    <xsd:complexType name="ContainerType">
 *      <xsd:sequence>
 *        <xsd:element name="machine" type="MachineType" minOccurs="0" maxOccurs="unbounded"/>
 *        <xsd:element name="property" type="PropertyType" minOccurs="0" maxOccurs="unbounded"/>
 *      </xsd:sequence>
 *      <xsd:attribute name="name" type="xsd:string" use="required"/>
 *    </xsd:complexType>
 */
struct containertypeParser: parser
{
  virtual void buildAttr(const XML_Char** attr)
    {
      required("name",attr);
      for (int i = 0; attr[i]; i += 2)
      {
        if(std::string(attr[i]) == "name")name(attr[i+1]);
      }
    }
  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      parser* pp=&main_parser;
      if(element == "machine")pp=&machineParser;
      if(element == "property")pp=&propertyParser;
      XML_SetUserData(p,pp);
      sp.push(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  virtual void onEnd(const char *el,parser* child)
    {
      std::string element(el);
      if(element == "machine")machine_(((machinetypeParser*)child)->post());
      if(element == "property")property(((propertytypeParser*)child)->post());
    }
  virtual void pre (){_container._machs.clear();}
  virtual void name(const std::string& name){ _container._name=name; }
  virtual void machine_(const machine& m)
    {
      DEBTRACE( "machine: " << m._name )             
      _container._machs.push_back(m);
    }
  virtual void property (const myprop& prop)
    {
      DEBTRACE( "property_set: " << prop._name << prop._value )             
      _container._props[prop._name]=prop._value;
    }
  mycontainer post()
    {
      //mincount("machine",1);
      return _container;
    }
  mycontainer _container;
};
static containertypeParser containerParser;

/*! \brief Class for loading parser
 *
 *  Class used to parse service node loading information
 *  XML schema is 
 *    <xsd:complexType name="LoadType">
 *      <xsd:attribute name="container" type="xsd:string" use="required"/>
 *    </xsd:complexType>
 */
struct loadtypeParser: parser
{
  virtual void buildAttr(const XML_Char** attr)
    {
      required("container",attr);
      for (int i = 0; attr[i]; i += 2)
      {
        if(std::string(attr[i]) == "container")container(attr[i+1]);
      }
    }
  virtual void pre (){_loadon._container="";}
  virtual void container(const std::string& name){ _loadon._container=name; }
  loadon post()
    {
      return _loadon;
    }
  loadon _loadon;
};
static loadtypeParser loadParser;

/*! \brief Class for Inport parser.
 *
 *  This class is a base class for other inport parsers
 *    <xsd:complexType name="InPortType">
 *      <xsd:sequence>
 *        <xsd:element name="property" type="PropertyType" minOccurs="0"/>
 *      </xsd:sequence>
 *      <xsd:attribute name="name" type="xsd:string" use="required"/>
 *      <xsd:attribute name="type" type="xsd:string" use="required"/>
 *    </xsd:complexType>
 *
 */
template <class T=myinport>
struct inporttypeParser: parser
{
  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      parser* pp=&main_parser;
      if(element == "property")pp=&propertyParser;
      XML_SetUserData(p,pp);
      sp.push(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  virtual void onEnd(const char *el,parser* child)
    {
      std::string element(el);
      if(element == "property")property(((propertytypeParser*)child)->post());
    }
  virtual void buildAttr(const XML_Char** attr)
    {
      required("name",attr);
      required("type",attr);
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
          if(std::string(attr[i]) == "type")type(attr[i+1]);
        }
    }
  virtual void pre ()
    {
      _port._name="";
      _port._type="";
      _port.clear();
    }
  virtual void name(const std::string& name)
    {
      _port._name=name;
    }
  virtual void type(const std::string& type)
    {
      _port._type=type;
    }
  virtual void property (const myprop& prop)
    {
      DEBTRACE( "property_set: " << prop._name << prop._value )             
      _port.setProperty(prop._name,prop._value);
    }
  virtual T& post()
    {
      return _port;
    }
protected:
    T _port;
};
static inporttypeParser<> inportParser;

/*! \brief Class for Outport parser.
 *
 *  This class is also used for OutputDataStream Port
 *  same XML schema as inporttypeParser
 */
template <class T=myoutport>
struct outporttypeParser:public inporttypeParser<T>
{
};
static outporttypeParser<> outportParser;

/*! \brief Class for node parser.
 *
 *  This class is a base class for other parsers
 */
template <class T=InlineNode*>
struct nodetypeParser:public parser
{
  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      DEBTRACE( "nodetypeParser::onStart: " << el )             
      std::string element(el);
      parser* pp=&main_parser;
      XML_SetUserData(p,pp);
      sp.push(pp);
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
  virtual T post()
    {
      return _node;
    }
  std::string _type;
  std::string _name;
  std::string _state;
  T _node;
};
static nodetypeParser<> nodeParser;

template <>
InlineNode* nodetypeParser<InlineNode*>::post ()
{
  std::string fullname = currentProc->names.back()+_type;
  if(currentProc->inlineMap.count(_type) != 0)
    {
      //InlineNode type with absolute name found
      InlineNode* n=currentProc->inlineMap[_type];
      _node=n->cloneNode(_name);
    }
  else if(currentProc->inlineMap.count(fullname) != 0)
    {
      //InlineNode type with relative name found
      InlineNode* n=currentProc->inlineMap[fullname];
      _node=n->cloneNode(_name);
    }
  else
    {
      throw Exception("Unknown InlineNode type");
    }
  if(_state == "disabled")_node->exDisabledState();
  DEBTRACE( "node_post " << _node->getName() )             
  return _node;
}

struct codetypeParser: parser
{
  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      parser* pp=&main_parser;
      if(element == "code")pp=&stringParser;
      XML_SetUserData(p,pp);
      sp.push(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  virtual void onEnd(const char *el,parser* child)
    {
      std::string element(el);
      if(element == "code")code(((stringtypeParser*)child)->post());
    }
  virtual void pre (){_code="";}
  virtual void code (const std::string& s)
    {
      if(_code == "")
        _code=s;
      else 
        _code=_code + '\n' + s;
    }
  virtual myfunc post ()
    {
      _func._name="script";
      _func._code=_code;
      return _func;
    }
    std::string _code;
    myfunc _func;
};
static codetypeParser codeParser;

struct functypeParser: codetypeParser
{
  virtual void buildAttr(const XML_Char** attr)
    {
      required("name",attr);
      for (int i = 0; attr[i]; i += 2) 
      {
        if(std::string(attr[i]) == "name")name(attr[i+1]);
      }
    }
  virtual void name (const std::string& name)
    {
      _func._name=name;
    }
  virtual myfunc post ()
    {
      _func._code=_code;
      return _func;
    }
};
static functypeParser funcParser;

static std::string t1[]={"script","function",""};

template <class T=InlineNode*>
struct inlinetypeParser:public nodetypeParser<T>
{
  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      DEBTRACE( "inlinetypeParser::onStart: " << el )             
      std::string element(el);
      parser* pp=&main_parser;
      this->maxcount("kind",1,element);
      this->maxcount("script",1,element);
      this->maxcount("function",1,element);
      this->maxchoice(t1,1,element);
      if(element == "kind")pp=&stringParser;
      else if(element == "script")pp=&codeParser;
      else if(element == "function")pp=&funcParser;
      else if(element == "inport")pp=&inportParser;
      else if(element == "outport")pp=&outportParser;
      XML_SetUserData(p,pp);
      sp.push(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
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
        throw Exception("Node must be completely defined before defining its ports");
      if(currentProc->typeMap.count(p._type)==0)
        {
          std::string msg="Unknown InPort Type: ";
          msg=msg+p._type+" for node: "+this->_node->getName()+" port name: "+p._name;
          throw Exception(msg);
        }
      this->_node->edAddInputPort(p._name,currentProc->typeMap[p._type]);
    }
  virtual void outport (const myoutport& p)
    {
      DEBTRACE( "inline_outport: " << p._name <<":"<<p._type)             
      if(this->_node==0)
        throw Exception("Node must be completely defined before defining its ports");
      if(currentProc->typeMap.count(p._type)==0)
        {
          std::string msg="Unknown OutPort Type: ";
          msg=msg+p._type+" for node: "+this->_node->getName()+" port name: "+p._name;
          throw Exception(msg);
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
static inlinetypeParser<> inlineParser;

template <>
void inlinetypeParser<InlineNode*>::script (const myfunc& f)
{
  DEBTRACE( "inline_script: " << f._code )             
  _node=theRuntime->createScriptNode(_kind,_name);
  _node->setScript(f._code);
}
template <>
void inlinetypeParser<InlineNode*>::function (const myfunc& f)
{
  DEBTRACE( "inline_function: " << f._code )             
  InlineFuncNode *fnode;
  fnode=theRuntime->createFuncNode(_kind,_name);
  fnode->setScript(f._code);
  fnode->setFname(f._name);
  _node=fnode;
}

/*! \brief Class for parsing ServiceInlineNode description
 *
 *  
 */
template <class T=ServiceInlineNode*>
struct sinlinetypeParser:public inlinetypeParser<T>
{
  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      DEBTRACE( "sinlinetypeParser::onStart: " << el )             
      std::string element(el);
      parser* pp=&main_parser;
      this->maxcount("kind",1,element);
      this->maxcount("function",1,element);
      this->maxcount("load",1,element);
      if(element == "kind")pp=&stringParser;
      else if(element == "function")pp=&funcParser;
      else if(element == "load")pp=&loadParser;
      else if(element == "inport")pp=&inportParser;
      else if(element == "outport")pp=&outportParser;
      XML_SetUserData(p,pp);
      sp.push(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
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
      if(this->_node==0)
        throw Exception("ServiceInlineNode must be completely defined before defining how to load it");

      if(currentProc->containerMap.count(l._container) != 0)
        {
          //If it has already a container replace it ?????
          this->_node->getComponent()->setContainer(currentProc->containerMap[l._container]);
        }
      else
        {
          std::cerr <<  "WARNING: Unknown container " << l._container << std::endl;
        }
    }
};

template <>
void inlinetypeParser<ServiceInlineNode*>::function (const myfunc& f)
{
  DEBTRACE( "sinline_function: " << f._code )             
  ServiceInlineNode *fnode;
  fnode=theRuntime->createSInlineNode(_kind,_name);
  fnode->setScript(f._code);
  fnode->setMethod(f._name);
  fnode->setComponent(theRuntime->createComponentInstance("PyCompo","SalomePy"));
  //fnode->setRef("PyCompo");
  _node=fnode;
}

static sinlinetypeParser<> sinlineParser;

static std::string t2[]={"ref","node","component",""};

template <class T=ServiceNode*>
struct servicetypeParser:public inlinetypeParser<T>
{
  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      DEBTRACE( "servicetypeParser::onStart: " << el )             
      std::string element(el);
      parser* pp=&main_parser;
      this->maxcount("kind",1,element);
      this->maxcount("ref",1,element);
      this->maxcount("node",1,element);
      this->maxcount("component",1,element);
      this->maxcount("method",1,element);
      this->maxcount("load",1,element);
      this->maxchoice(t2,1,element);
      if(element == "kind")pp=&stringParser;
      else if(element == "ref")pp=&stringParser;
      else if(element == "component")pp=&stringParser;
      else if(element == "node")pp=&stringParser;
      else if(element == "method")pp=&stringParser;
      else if(element == "load")pp=&loadParser;
      else if(element == "inport")pp=&inportParser;
      else if(element == "outport")pp=&outportParser;
      else if(element == "instream")pp=&inportParser;
      else if(element == "outstream")pp=&outportParser;
      XML_SetUserData(p,pp);
      sp.push(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
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
          ServiceNode* n=currentProc->serviceMap[name];
          this->_node =n->createNode(this->_name);
        }
      else if(currentProc->serviceMap.count(fullname) != 0)
        {
          //ServiceNode with relative name found
          //TODO: must be a short name (possible only in the same context)
          ServiceNode* n=currentProc->serviceMap[fullname];
          this->_node =n->createNode(this->_name);
        }
      else
        {
          throw Exception("Unknown ServiceNode");
        }
    }
  virtual void method (const std::string& name)
    {
      DEBTRACE( "service_method: " << name )             
      if(this->_node==0)
        throw Exception("ServiceNode must be completely defined before defining its method");
      this->_node->setMethod(name);
    }

  virtual void load (const loadon& l)
    {
      DEBTRACE( "service_load: " )             
      if(this->_node==0)
        throw Exception("ServiceNode must be completely defined before defining how to load it");

      if(currentProc->containerMap.count(l._container) != 0)
        {
          //If it has already a container replace it ?????
          this->_node->getComponent()->setContainer(currentProc->containerMap[l._container]);
        }
      else
        {
          std::cerr << "WARNING: Unknown container " << l._container << std::endl;
        }
    }

  virtual void instream (const myinport& p)
    {
      DEBTRACE( "service_instream" )             
      DEBTRACE( p._type )             
      DEBTRACE( p._name )             
      if(this->_node==0)
        throw Exception("ServiceNode must be completely defined before defining its ports");
      if(currentProc->typeMap.count(p._type)==0)
        {
          std::string msg="Unknown InPort Type: ";
          msg=msg+p._type+" for node: "+this->_node->getName()+" port name: "+p._name;
          throw Exception(msg);
        }
      InputDataStreamPort* port;
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
        throw Exception("ServiceNode must be completely defined before defining its ports");
      if(currentProc->typeMap.count(p._type)==0)
        {
          std::string msg="Unknown OutPort Type: ";
          msg=msg+p._type+" for node: "+this->_node->getName()+" port name: "+p._name;
          throw Exception(msg);
        }
      OutputDataStreamPort* port;
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
      return this->_node;
    }
};
static servicetypeParser<> serviceParser;

template <class T=mycontrol>
struct controltypeParser: parser
{
  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      parser* pp=&main_parser;
      this->maxcount("fromnode",1,element);
      this->maxcount("tonode",1,element);
      if(element == "fromnode")pp=&stringParser;
      else if(element == "tonode")pp=&stringParser;
      XML_SetUserData(p,pp);
      sp.push(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  virtual void onEnd(const char *el,parser* child)
    {
      std::string element(el);
      if(element == "fromnode")fromnode(((stringtypeParser*)child)->post());
      else if(element == "tonode")tonode(((stringtypeParser*)child)->post());
    }
  virtual void pre ()
    {
      _link.clear();
    }
  virtual void fromnode (const std::string& name)
    {
      _link.fromnode(name);
    }
  virtual void tonode (const std::string& name)
    {
      _link.tonode(name);
    }
  virtual void property (const myprop& prop)
    {
      DEBTRACE( "property_set: " << prop._name << prop._value )             
      _link.setProperty(prop._name,prop._value);
    }
  virtual T& post()
    {
      mincount("fromnode",1);
      mincount("tonode",1);
      return _link;
    }
    T _link;
};
static controltypeParser<> controlParser;

template <class T=mylink>
struct linktypeParser: controltypeParser<T>
{
  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      this->maxcount("fromnode",1,element);
      this->maxcount("tonode",1,element);
      this->maxcount("fromport",1,element);
      this->maxcount("toport",1,element);
      parser* pp=&main_parser;
      if(element == "fromnode")pp=&stringParser;
      else if(element == "tonode")pp=&stringParser;
      else if(element == "toport")pp=&stringParser;
      else if(element == "fromport")pp=&stringParser;
      else if(element == "property")pp=&propertyParser;
      XML_SetUserData(p,pp);
      sp.push(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  virtual void onEnd(const char *el,parser* child)
    {
      std::string element(el);
      if(element == "fromnode")this->fromnode(((stringtypeParser*)child)->post());
      else if(element == "tonode")this->tonode(((stringtypeParser*)child)->post());
      else if(element == "toport")toport(((stringtypeParser*)child)->post());
      else if(element == "fromport")fromport(((stringtypeParser*)child)->post());
      else if(element == "property")this->property(((propertytypeParser*)child)->post());
    }
  virtual void buildAttr(const XML_Char** attr)
    {
      for (int i = 0; attr[i]; i += 2) 
        {
          if((std::string(attr[i]) == "control")
             && (std::string(attr[i+1]) == "false"))
            this->_link._withControl=false;
        }
    }
  virtual void fromport (const std::string& name)
    {
      this->_link.fromport(name);
    }
  virtual void toport (const std::string& name)
    {
      this->_link.toport(name);
    }
  virtual T& post()
    {
      this->mincount("fromnode",1);
      this->mincount("tonode",1);
      this->mincount("fromport",1);
      this->mincount("toport",1);
      return this->_link;
    }
};
static linktypeParser<> linkParser;

template <class T=mystream>
struct streamtypeParser: linktypeParser<T>
{
};
static streamtypeParser<> streamParser;

static std::string t4[]={"string","objref","double","int","boolean","array","struct",""};

/*! \brief Class for XML-RPC value parser.
 *
 *  This class is used to parse XML data that describes a sequence in XML-RPC format
 *  Its XML schema is:
 *    <xsd:complexType name="ValueType">
 *      <xsd:choice >
 *        <xsd:element name="int" type="IntType"/>
 *        <xsd:element name="boolean" type="BooleanType"/>
 *        <xsd:element name="double" type="DoubleType"/>
 *        <xsd:element name="string" type="StringType"/>
 *        <xsd:element name="objref" type="ObjrefType"/>
 *        <xsd:element name="array" type="ArrayType"/>
 *        <xsd:element name="struct" type="StructType"/>
 *      </xsd:choice>
 *    </xsd:complexType>
 */
struct valuetypeParser: parser
{
  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child);
  virtual void pre (){ }
  virtual void int_ (const int& d)
    {
      std::ostringstream os;
      os << "<int>"   << d<< "</int>";
      _data=os.str();
      _v.push_back(_data);
    }
  virtual void boolean (const bool& d)
    {
      std::ostringstream os;
      os << "<boolean>"   << d<< "</boolean>";
      _data=os.str();
      _v.push_back(_data);
    }
  virtual void double_ (const double& d)
    {
      std::ostringstream os;
      os << "<double>"<< d<< "</double>";
      _data=os.str();
      _v.push_back(_data);
    }
  virtual void string(const std::string& d)
    {
      _data="<string>"+ d+ "</string>";
      _v.push_back(_data);
    }
  virtual void objref(const std::string& d)
    {
      _data="<objref>"+ d+ "</objref>";
      _v.push_back(_data);
    }
  virtual void array (const std::string& d)
    {
      _v.push_back(d);
    }
  virtual void struct_ (const std::string& d)
    {
      _v.push_back(d);
    }
  virtual std::string post()
    {
      minchoice(t4,1);
      std::string value="<value>"+_v.back()+"</value>\n";
      _v.pop_back();
      return value;
    }
  std::string _data;
  std::vector<std::string> _v;
};
static valuetypeParser valueParser;

/*! \brief Class for XML-RPC data parser.
 *
 *  This class is used to parse XML data that describes a sequence in XML-RPC format
 *  Its XML schema is:
 *    <xsd:complexType name="DataType">
 *      <xsd:element name="value" type="ValueType"/>
 *    </xsd:complexType>
 */
struct datatypeParser: parser
{
  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      parser* pp=&main_parser;
      if(element == "value")pp=&valueParser;
      XML_SetUserData(p,pp);
      sp.push(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  virtual void onEnd(const char *el,parser* child)
    {
      std::string element(el);
      if(element == "value")value(((valuetypeParser*)child)->post());
    }
  virtual void pre ()
    {
      _datas.push_back(_data);
      _data="";
    }
  virtual void value (const std::string& v){
      _data=_data+v;
    }
  virtual std::string post()
    {
      mincount("value",1);
      std::string d="<data>\n"+_data+"</data>";
      _data=_datas.back();
      _datas.pop_back();
      return d;
    }
  std::string _data;
  std::vector<std::string> _datas;
};
static datatypeParser dataParser;

/*! \brief Class for XML-RPC member parser.
 *
 *  This class is used to parse XML data that describes a sequence in XML-RPC format
 *  Its XML schema is:
 *    <xsd:complexType name="MemberDataType">
 *      <xsd:element name="name" type="StringType"/>
 *      <xsd:element name="value" type="ValueType"/>
 *    </xsd:complexType>
 */
struct memberdatatypeParser: parser
{
  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      parser* pp=&main_parser;
      if(element == "name")pp=&stringParser;
      else if(element == "value")pp=&valueParser;
      XML_SetUserData(p,pp);
      sp.push(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  virtual void onEnd(const char *el,parser* child)
    {
      std::string element(el);
      this->maxcount("name",1,element);
      this->maxcount("value",1,element);
      if(element == "name")name(((stringtypeParser*)child)->post());
      else if(element == "value")value(((valuetypeParser*)child)->post());
    }
  virtual void pre ()
    {
      _datas.push_back(_data);
      _data="";
    }
  virtual void name (const std::string& v)
    {
      _data=_data+"<name>"+v+"</name>";
    }
  virtual void value (const std::string& v)
    {
      _data=_data+v;
    }
  virtual std::string post()
    {
      mincount("value",1);
      mincount("name",1);
      std::string d="<member>\n"+_data+"</member>";
      _data=_datas.back();
      _datas.pop_back();
      return d;
    }
  std::string _data;
  std::vector<std::string> _datas;
};
static memberdatatypeParser memberdataParser;

/*! \brief Class for XML-RPC struct parser.
 *
 *  This class is used to parse XML data that describes a sequence in XML-RPC format
 *  Its XML schema is:
 *    <xsd:complexType name="StructDataType">
 *      <xsd:element name="member" type="MemberDataType" minOccurs="1"/>
 *    </xsd:complexType>
 */
struct structdatatypeParser: parser
{
  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      parser* pp=&main_parser;
      if(element == "member")pp=&memberdataParser;
      XML_SetUserData(p,pp);
      sp.push(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  virtual void onEnd(const char *el,parser* child)
    {
      std::string element(el);
      if(element == "member")member(((memberdatatypeParser*)child)->post());
    }
  virtual void pre ()
    {
      _membersStack.push_back(_members);
      _members="";
    }
  virtual void member (const std::string& d)
    {
      _members=_members+d;
    }
  virtual std::string post()
    {
      mincount("member",1);
      std::string value="<struct>"+_members+"</struct>";
      _members=_membersStack.back();
      _membersStack.pop_back();
      return value;
    }
  std::string _members;
  std::vector<std::string> _membersStack;
};
static structdatatypeParser structdataParser;

/*! \brief Class for XML-RPC array parser.
 *
 *  This class is used to parse XML data that describes a sequence in XML-RPC format
 *  Its XML schema is:
 *    <xsd:complexType name="ArrayType">
 *      <xsd:element name="data" type="DataType" minOccurs="1" maxOccurs="1"/>
 *    </xsd:complexType>
 */
struct arraytypeParser: parser
{
  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      this->maxcount("data",1,element);
      parser* pp=&main_parser;
      if(element == "data")pp=&dataParser;
      XML_SetUserData(p,pp);
      sp.push(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  virtual void onEnd(const char *el,parser* child)
    {
      std::string element(el);
      if(element == "data")data(((datatypeParser*)child)->post());
    }
  virtual void pre (){ }
  virtual void data (const std::string& d)
    {
      _arrays.push_back(d);
    }
  virtual std::string post()
    {
      mincount("data",1);
      std::string value="<array>"+_arrays.back()+"</array>";
      _arrays.pop_back();
      return value;
    }
  std::vector<std::string> _arrays;
};
static arraytypeParser arrayParser;


void valuetypeParser::onStart(const XML_Char* el, const XML_Char** attr)
{
  std::string element(el);
  parser* pp=&main_parser;
  this->maxcount("string",1,element);
  this->maxcount("objref",1,element);
  this->maxcount("double",1,element);
  this->maxcount("int",1,element);
  this->maxcount("boolean",1,element);
  this->maxcount("array",1,element);
  this->maxcount("struct",1,element);
  this->maxchoice(t4,1,element);
  if(element == "string")pp=&stringParser;
  else if(element == "objref")pp=&stringParser;
  else if(element == "double")pp=&doubleParser;
  else if(element == "int")pp=&intParser;
  else if(element == "boolean")pp=&boolParser;
  else if(element == "array")pp=&arrayParser;
  else if(element == "struct")pp=&structdataParser;
  XML_SetUserData(p,pp);
  sp.push(pp);
  pp->init();
  pp->pre();
  pp->buildAttr(attr);
}

void valuetypeParser::onEnd(const char *el,parser* child)
{
  std::string element(el);
  if(element == "string")string(((stringtypeParser*)child)->post());
  else if(element == "objref")objref(((stringtypeParser*)child)->post());
  else if(element == "double")double_(((doubletypeParser*)child)->post());
  else if(element == "int")int_(((inttypeParser*)child)->post());
  else if(element == "boolean")boolean(((booltypeParser*)child)->post());
  else if(element == "array")array(((arraytypeParser*)child)->post());
  else if(element == "struct")struct_(((structdatatypeParser*)child)->post());
}

struct parametertypeParser: parser
{
  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      this->maxcount("tonode",1,element);
      this->maxcount("toport",1,element);
      this->maxcount("value",1,element);
      parser* pp=&main_parser;
      if(element == "tonode")pp=&stringParser;
      else if(element == "toport")pp=&stringParser;
      else if(element == "value")pp=&valueParser;
      XML_SetUserData(p,pp);
      sp.push(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  virtual void onEnd(const char *el,parser* child)
    {
      std::string element(el);
      if(element == "tonode")tonode(((stringtypeParser*)child)->post());
      else if(element == "toport")toport(((stringtypeParser*)child)->post());
      else if(element == "value")value(((valuetypeParser*)child)->post());
    }
    virtual void pre (){}
    virtual void tonode (const std::string& name){
      _param._tonode=name;
    }
    virtual void toport (const std::string& name){
      _param._toport=name;
    }
    virtual void value (const std::string& name){
      _param._value=name;
    }
    virtual myparam& post(){
      mincount("tonode",1);
      mincount("toport",1);
      mincount("value",1);
      return _param;
    }
    myparam _param;
};
static parametertypeParser paramParser;

static std::string t3[]={"inline","sinline","service","node","forloop","foreach","while","switch","bloc",""};

struct casetypeParser:parser
{
  void onStart(const XML_Char* el, const XML_Char** attr);
  void onEnd(const char *el,parser* child);
  virtual void buildAttr(const XML_Char** attr)
    {
      this->required("id",attr);
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "id")id(atoi(attr[i+1]));
        }
    }
  virtual void pre ()
    {
      _cnode=0;
      _id=0;
    }
  virtual void id (const int& n)
    {
      DEBTRACE( "case_id: " << n )             
      _id=n;
      //store this level id
      _idStack.push_back(_id);
      //store this level name
      std::stringstream temp;
      if (_id <0) temp << "m" << -_id << "_";
      else temp << "p" << _id << "_";
      std::string fullname=currentProc->names.back()+temp.str();
      DEBTRACE( "case_fullname: " << fullname )             
      currentProc->names.push_back(fullname);
    }
  virtual void property (const myprop& prop)
    {
      DEBTRACE( "property_set: " << prop._name << prop._value )             
    }
  virtual void inline_ (InlineNode* const& n)
    {
      _cnode=n;
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->inlineMap[fullname]=n;
    }
  virtual void sinline (ServiceInlineNode* const& n)
    {
      _cnode=n;
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->serviceMap[fullname]=n;
    }
  virtual void service (ServiceNode* const& n)
    {
      _cnode=n;
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->serviceMap[fullname]=n;
    }
  virtual void node (InlineNode* const& n)
    {
      _cnode=n;
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->inlineMap[fullname]=n;
    }
  virtual void forloop (ForLoop* const& n)
    {
      _cnode=n;
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
    }
  virtual void foreach (ForEachLoop* const& n)
    {
      _cnode=n;
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
      fullname += ".splitter";
      currentProc->nodeMap[fullname]=n->getChildByShortName("splitter");
    }
  virtual void while_ (WhileLoop* const& n)
    {
      _cnode=n;
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
    }
  virtual void switch_ (Switch* const& n)
    {
      _cnode=n;
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
    }
  virtual void bloc (Bloc* const& n)
    {
      _cnode=n;
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
    }
  virtual std::pair<int,Node*> post()
    {
      DEBTRACE( "case_post" )             
      minchoice(t3,1);
      //get back this level id
      _id=_idStack.back();
      _idStack.pop_back();
      //pop back this level name
      currentProc->names.pop_back();
      return std::pair<int,Node*>(_id,_cnode);
    }
  Node* _cnode;
  int _id;
  std::vector<int> _idStack;
};
static casetypeParser caseParser;

struct defaultcasetypeParser:casetypeParser
{
  virtual void buildAttr(const XML_Char** attr)
    {
      for (int i = 0; attr[i]; i += 2) 
      {
        DEBTRACE( attr[i] << "=" << attr[i + 1] )             
      }
    }
  virtual void pre ()
    {
      _id=0;
      _cnode=0;
      //store this level id
      _idStack.push_back(_id);
      //store this level name
      std::string fullname=currentProc->names.back()+"default_";
      DEBTRACE( "case_fullname: " << fullname )             
      currentProc->names.push_back(fullname);
    }
};
static defaultcasetypeParser defaultcaseParser;

struct switchtypeParser:parser
{
  void onStart(const XML_Char* el, const XML_Char** attr);
  void onEnd(const char *el,parser* child);
  virtual void buildAttr(const XML_Char** attr)
    {
      this->required("name",attr);
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
          if(std::string(attr[i]) == "state")state(attr[i+1]);
          if(std::string(attr[i]) == "select")select(atoi(attr[i+1]));
        }
    }
  virtual void pre (){_state="";}
  virtual void case_ (const std::pair<int,Node*>& p)
    {
      Switch* s=_cnodes.back();
      s->edSetNode(p.first,p.second);
    }
  virtual void default_ (const std::pair<int,Node*>& p)
    {
      Switch* s=_cnodes.back();
      s->edSetDefaultNode(p.second);
    }
  virtual void name (const std::string& name)
    {
      Switch* s;
      std::string fullname=currentProc->names.back()+name;
      DEBTRACE( "switch_fullname: " << fullname )             
      s=theRuntime->createSwitch(name);
      _cnodes.push_back(s);
      currentProc->names.push_back(fullname+'.');
    }
  virtual void state (const std::string& state)
    {
      //state is an attribute (no order). It can be defined before name
      //To be improved
      Switch* s=_cnodes.back();
      if(_state == "disabled")
        {
          DEBTRACE( "Switch disabled: " << s->getName())             
          s->exDisabledState();
        }
    }
  virtual void select (const int& s)
    {
      //select is an attribute
      Switch* sw=_cnodes.back();
      InputPort *p=sw->edGetConditionPort();
      p->edInit(s);
    }
  virtual Switch* post ()
    {
      DEBTRACE( "switch_post: " )             
      Switch* sw=_cnodes.back();
      //pop back current level name and node
      _cnodes.pop_back();
      currentProc->names.pop_back();
      return sw;
    }
  // stack to store switches in case of switch in switch
  std::vector<Switch *> _cnodes;
  std::string _state;
};
static switchtypeParser switchParser;

template <class T=Loop*>
struct looptypeParser:parser
{
  void onStart(const XML_Char* el, const XML_Char** attr);
  void onEnd(const char *el,parser* child);
  virtual void buildAttr(const XML_Char** attr)
    {
      this->required("name",attr);
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
          if(std::string(attr[i]) == "state")state(attr[i+1]);
        }
    }
  virtual void pre ()
    {
      _state="";
      _cnode=0;
    }
  virtual void name (const std::string& name)
    {
      DEBTRACE( "bloc_name: " << name );
    }
  virtual void state (const std::string& name)
    {
      DEBTRACE( "bloc_state: " << name );
      _state=name;
    }
  virtual void property (const myprop& prop)
    {
      DEBTRACE( "property_set" << prop._name << prop._value );
    }
  virtual void inline_ (InlineNode* const& n)
    {
      DEBTRACE( "loop_inline" << n->getName() );
      _cnode->edSetNode(n);
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->inlineMap[fullname]=n;
    }
  virtual void sinline (ServiceInlineNode* const& n)
    {
      DEBTRACE( "loop_sinline" << n->getName() )             
      _cnode->edSetNode(n);
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->serviceMap[fullname]=n;
    }
  virtual void service (ServiceNode* const& n)
    {
      DEBTRACE( "loop_service" << n->getName() )             
      _cnode->edSetNode(n);
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->serviceMap[fullname]=n;
    }
  virtual void node (InlineNode* const& n)
    {
      DEBTRACE( "loop_node" << n->getName() )             
      _cnode->edSetNode(n);
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->inlineMap[fullname]=n;
    }
  virtual void forloop (ForLoop* const& b)
    {
      DEBTRACE( "loop_forloop" << b->getName() )             
      _cnode->edSetNode(b);
      std::string fullname=currentProc->names.back()+ b->getName();
      currentProc->nodeMap[fullname]=b;
    }
  virtual void foreach (ForEachLoop* const& b)
    {
      DEBTRACE("loop_foreach" << b->getName())
      _cnode->edSetNode(b);
      std::string fullname=currentProc->names.back()+ b->getName();
      currentProc->nodeMap[fullname]=b;
      fullname += ".splitter";
      currentProc->nodeMap[fullname]=b->getChildByShortName("splitter");
    }
  virtual void while_ (WhileLoop* const& b)
    {
      DEBTRACE( "loop_while: " << b->getName() )             
      _cnode->edSetNode(b);
      std::string fullname=currentProc->names.back()+ b->getName();
      currentProc->nodeMap[fullname]=b;
    }
  virtual void switch_ (Switch* const& b)
    {
      DEBTRACE( "loop_switch: " << b->getName() )             
      _cnode->edSetNode(b);
      std::string fullname=currentProc->names.back()+ b->getName();
      currentProc->nodeMap[fullname]=b;
    }
  virtual void bloc (Bloc* const& b)
    {
      DEBTRACE( "loop_bloc " << b->getName() )             
      _cnode->edSetNode(b);
      std::string fullname=currentProc->names.back()+ b->getName();
      currentProc->nodeMap[fullname]=b;
    }

  virtual void datalink (const mylink& l)
    {
      DEBTRACE( "loop_datalink: " << l.fromnode() << l.fromport() << l.tonode() << l.toport())
      std::string msg;

      //Try only relative name for from node
      std::string fromname = currentProc->names.back()+l.fromnode();
      if(currentProc->nodeMap.count(fromname) == 0)
        {
          msg="from node " + l.fromnode() + " does not exist in data link: ";
          msg=msg+l.fromnode()+"("+l.fromport()+")->"+l.tonode()+"("+l.toport()+")";
          currentProc->getLogger("parser")->error(msg,rootParser.file,XML_GetCurrentLineNumber(p));
          return;
        }
      //Try relative name for to node and then absolute one
      std::string toname = currentProc->names.back()+l.tonode();
      if(currentProc->nodeMap.count(toname) == 0)
        {
          //It's not a relative name. Try an absolute one (I think it's not possible)
          toname=l.tonode();
          if(currentProc->nodeMap.count(toname) == 0)
            {
              // The TO node does not exist -> error
              msg="to node " + l.tonode() + " does not exist in data link: ";
              msg=msg+l.fromnode()+"("+l.fromport()+")->"+l.tonode()+"("+l.toport()+")";
              currentProc->getLogger("parser")->error(msg,rootParser.file,XML_GetCurrentLineNumber(p));
              return;
            }
        }
      // We only link local node and other nodes (relative or absolute name in this order)
      DEBTRACE(fromname <<":"<<l.fromport()<<toname<<":"<<l.toport());
      try
        {
          if (l.withControl())
            _cnode->edAddDFLink(currentProc->nodeMap[fromname]->getOutputPort(l.fromport()),
                                currentProc->nodeMap[toname]->getInputPort(l.toport()));
          else
            _cnode->edAddLink(currentProc->nodeMap[fromname]->getOutputPort(l.fromport()),
                              currentProc->nodeMap[toname]->getInputPort(l.toport()));
        }
      catch(Exception& e)
        {
            currentProc->getLogger("parser")->error(e.what(),rootParser.file,XML_GetCurrentLineNumber(p));
        }
    }

  std::string _state;
  T _cnode;
  std::vector<T> _cnodes;
};

template <class T=ForLoop*>
struct forlooptypeParser:looptypeParser<T>
{
  virtual void buildAttr(const XML_Char** attr)
    {
      this->required("name",attr);
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
          if(std::string(attr[i]) == "state")this->state(attr[i+1]);
          if(std::string(attr[i]) == "nsteps")nsteps(atoi(attr[i+1]));
        }
    }
  virtual void name (const std::string& name)
    {
      DEBTRACE( "forloop_name: " << name );
      std::string fullname=currentProc->names.back()+name;
      this->_cnode=theRuntime->createForLoop(name);
      currentProc->nodeMap[fullname]=this->_cnode;
      this->_cnodes.push_back(this->_cnode);
      currentProc->names.push_back(fullname+'.');
      _nsteps=0;
    }
  virtual void nsteps (const int& n)
    {
      DEBTRACE( "forloop_nsteps: " << n )             
      if(!this->_cnode)
          throw Exception("Node name must be defined before nsteps");
      InputPort *iNbTimes=this->_cnode->edGetNbOfTimesInputPort();
      iNbTimes->edInit(n);
    }
  virtual T post()
    {
      DEBTRACE( "forloop_post" )             
      this->minchoice(t3,1);
      T b=this->_cnode;
      this->_cnodes.pop_back();
      currentProc->names.pop_back();
      this->_cnode=this->_cnodes.back();
      return b;
    }
  int _nsteps;
};
static forlooptypeParser<> forloopParser;

template <class T=WhileLoop*>
struct whilelooptypeParser:looptypeParser<T>
{
  virtual void name (const std::string& name)
    {
      DEBTRACE( "while_name: " << name )             
      std::string fullname=currentProc->names.back()+name;
      this->_cnode=theRuntime->createWhileLoop(name);
      currentProc->nodeMap[fullname]=this->_cnode;
      this->_cnodes.push_back(this->_cnode);
      currentProc->names.push_back(fullname+'.');
    }
  virtual T post()
    {
      DEBTRACE( "while_post" << this->_cnode->getName() )             
      this->minchoice(t3,1);
      InputPort *cond=this->_cnode->edGetConditionPort();
      cond->edInit(true);
      T b=this->_cnode;
      this->_cnodes.pop_back();
      currentProc->names.pop_back();
      this->_cnode=this->_cnodes.back();
      return b;
    }
};
static whilelooptypeParser<> whileloopParser;

template <class T=ForEachLoop*>
struct foreachlooptypeParser:looptypeParser<T>
{
  virtual void buildAttr(const XML_Char** attr)
    {
      this->required("name",attr);
      this->required("type",attr);
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
          if(std::string(attr[i]) == "state")this->state(attr[i+1]);
          if(std::string(attr[i]) == "nbranch")nbranch(atoi(attr[i+1]));
          if(std::string(attr[i]) == "type")datatype(attr[i+1]);
        }
      postAttr();
    }
  virtual void pre ()
    {
      _nbranch=0;
      this->looptypeParser<T>::pre();
    }
  virtual void name (const std::string& name)
    {
      DEBTRACE("foreach_name: " << name)
      _name=name;
      _fullname=currentProc->names.back()+name;
    }
  virtual void nbranch (const int& n)
    {
      DEBTRACE("foreach_nbranch: " << n )
      _nbranch=n;
    }
  virtual void datatype (const std::string& type)
    {
      DEBTRACE("foreach_datatype: "<< type)
      _datatype=type;
    }
  virtual void postAttr()
    {
      if(currentProc->typeMap.count(_datatype)==0)
        {
          std::stringstream msg;
          msg << "Type "<< _datatype <<" does not exist"<<" ("<<__FILE__<<":"<<__LINE__<< ")";
          throw Exception(msg.str());
        }
      this->_cnode=theRuntime->createForEachLoop(_name,currentProc->typeMap[_datatype]);
      //set number of branches
      if(_nbranch > 0)this->_cnode->edGetNbOfBranchesPort()->edInit(_nbranch);
      this->_cnodes.push_back(this->_cnode);
      currentProc->names.push_back(_fullname + '.');
    }
  virtual T post()
    {
      DEBTRACE("foreach_post" << this->_cnode->getName())
      this->minchoice(t3,1);
      T b=this->_cnode;
      this->_cnodes.pop_back();
      currentProc->names.pop_back();
      if(this->_cnodes.size() == 0)
        this->_cnode=0;
      else
        this->_cnode=this->_cnodes.back();
      return b;
    }
  int _nbranch;
  std::string _fullname;
  std::string _name;
  std::string _datatype;
};
static foreachlooptypeParser<> foreachloopParser;

template <class T=Bloc*>
struct bloctypeParser:parser
{
  bloctypeParser():parser()
  {
    _orders["property"]=0;
    _orders["inline"]=2;
    _orders["service"]=2;
    _orders["sinline"]=2;
    _orders["node"]=2;
    _orders["forloop"]=2;
    _orders["foreach"]=2;
    _orders["while"]=2;
    _orders["switch"]=2;
    _orders["bloc"]=2;
    _orders["control"]=3;
    _orders["datalink"]=3;
    _orders["stream"]=3;
    _orders["parameter"]=3;
  }
  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child);
  virtual void buildAttr(const XML_Char** attr)
    {
      this->required("name",attr);
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
          if(std::string(attr[i]) == "state")state(attr[i+1]);
        }
    }
  void name (const std::string& name)
    {
    }
  virtual void state (const std::string& name){
      DEBTRACE( "bloc_state: " << name )             
      _state=name;
      if(_state == "disabled")
        {
          DEBTRACE( "Bloc disabled: " << _bloc->getName())             
          _bloc->exDisabledState();
        }
    }
  virtual void property (const myprop& prop)
    {
      DEBTRACE( "property_set: " << prop._name << prop._value )             
      _bloc->setProperty(prop._name,prop._value);
    }
  virtual void inline_ (InlineNode* const& n)
    {
      DEBTRACE( "bloc_pynode_set: " << n->getName() )             
      _bloc->edAddChild(n);
      std::string fullname = currentProc->names.back()+n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->inlineMap[fullname]=n;
    }
  virtual void sinline (ServiceInlineNode* const& n)
    {
      DEBTRACE( "bloc_sinline: " << n->getName() )             
      _bloc->edAddChild(n);
      std::string fullname = currentProc->names.back()+n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->serviceMap[fullname]=n;
    }
  virtual void service (ServiceNode* const& n)
    {
      DEBTRACE( "bloc_service_set: " << n->getName() )             
      _bloc->edAddChild(n);
      std::string fullname = currentProc->names.back()+n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->serviceMap[fullname]=n;
    }
  virtual void node (InlineNode* const& n)
    {
      DEBTRACE( "bloc_node_set: " << n->getName() )             
      _bloc->edAddChild(n);
      std::string fullname = currentProc->names.back()+n->getName();
      DEBTRACE( "bloc_node_set fullname = "  << fullname )             
      currentProc->nodeMap[fullname]=n;
      currentProc->inlineMap[fullname]=n;
    }
  virtual void forloop (ForLoop* const& b)
    {
      DEBTRACE( "bloc_forloop_set: " << b->getName() )             
      _bloc->edAddChild(b);
      std::string fullname = currentProc->names.back()+b->getName();
      currentProc->nodeMap[fullname]=b;
    }
  virtual void foreach (ForEachLoop* const& b)
    {
      DEBTRACE( "bloc_foreach_set: " << b->getName() )             
      _bloc->edAddChild(b);
      std::string fullname = currentProc->names.back()+b->getName();
      currentProc->nodeMap[fullname]=b;
      fullname += ".splitter";
      currentProc->nodeMap[fullname]=b->getChildByShortName("splitter");
    }
  virtual void while_ (WhileLoop* const& b)
    {
      DEBTRACE( "bloc_while_set: " << b->getName() )             
      _bloc->edAddChild(b);
      std::string fullname = currentProc->names.back()+b->getName();
      currentProc->nodeMap[fullname]=b;
    }
  virtual void switch_ (Switch* const& b)
    {
      DEBTRACE( "bloc_switch_set: " << b->getName() )             
      _bloc->edAddChild(b);
      std::string fullname = currentProc->names.back()+b->getName();
      currentProc->nodeMap[fullname]=b;
    }
  virtual void bloc (Bloc* const& b)
    {
      DEBTRACE( "bloc_bloc_set: " << b->getName() )             
      _bloc->edAddChild(b);
      std::string fullname=currentProc->names.back()+ b->getName();
      currentProc->nodeMap[fullname]=b;
    }
  virtual void control (const mycontrol& l)
    {
      DEBTRACE( "bloc_control_set: " << l.fromnode() << " "<< l.tonode() )             
      std::string msg;

      if(currentProc->nodeMap.count(currentProc->names.back()+l.fromnode()) == 0)
        {
          msg="from node " + l.fromnode() + " does not exist in control link: ";
          msg=msg+l.fromnode()+"->"+l.tonode();
          msg=msg+ " context: "+currentProc->names.back();
          currentProc->getLogger("parser")->error(msg,rootParser.file,XML_GetCurrentLineNumber(p));
          return;
        }
      if(currentProc->nodeMap.count(currentProc->names.back()+l.tonode()) == 0)
        {
          msg="to node " + l.tonode() + " does not exist in control link: ";
          msg=msg+l.fromnode()+"->"+l.tonode();
          msg=msg+ " context: "+currentProc->names.back();
          currentProc->getLogger("parser")->error(msg,rootParser.file,XML_GetCurrentLineNumber(p));
          return;
        }
      // We only link local nodes
      try
        {
          _bloc->edAddCFLink(currentProc->nodeMap[currentProc->names.back()+l.fromnode()],
                             currentProc->nodeMap[currentProc->names.back()+l.tonode()]);
        }
      catch(Exception& e)
        {
          currentProc->getLogger("parser")->error(e.what(),rootParser.file,XML_GetCurrentLineNumber(p));
        }
    }
  virtual void datalink (const mylink& l)
    {
      DEBTRACE( "bloc_datalink_set: "<<l.fromnode()<<"("<<l.fromport()<<")->"<<l.tonode()<<"("<<l.toport()<<")")
      std::string msg;

      //Try only relative name for from node
      std::string fromname = currentProc->names.back()+l.fromnode();
      if(currentProc->nodeMap.count(fromname) == 0)
        {
          msg="from node " + l.fromnode() + " does not exist in data link: ";
          msg=msg+l.fromnode()+"("+l.fromport()+")->"+l.tonode()+"("+l.toport()+")";
          currentProc->getLogger("parser")->error(msg,rootParser.file,XML_GetCurrentLineNumber(p));
          return;
        }
      //Try relative name for to node and then absolute one
      std::string toname = currentProc->names.back()+l.tonode();
      if(currentProc->nodeMap.count(toname) == 0)
        {
          //It's not a relative name. Try an absolute one (I think it's not possible)
          toname=l.tonode();
          if(currentProc->nodeMap.count(toname) == 0)
            {
              // The TO node does not exist -> error
              msg="to node " + l.tonode() + " does not exist in data link: ";
              msg=msg+l.fromnode()+"("+l.fromport()+")->"+l.tonode()+"("+l.toport()+")";
              currentProc->getLogger("parser")->error(msg,rootParser.file,XML_GetCurrentLineNumber(p));
              return;
            }
        }
      // We only link local node and other nodes (relative or absolute name in this order)
      DEBTRACE(fromname <<":"<<l.fromport()<<toname<<":"<<l.toport());
      try
        {
          if (l.withControl())
            _bloc->edAddDFLink(currentProc->nodeMap[fromname]->getOutputPort(l.fromport()),
                               currentProc->nodeMap[toname]->getInputPort(l.toport()));
          else
            _bloc->edAddLink(currentProc->nodeMap[fromname]->getOutputPort(l.fromport()),
                             currentProc->nodeMap[toname]->getInputPort(l.toport()));
        }
      catch(Exception& e)
        {
          YACS::ENGINE::Logger* logger=currentProc->getLogger("parser");
          logger->error(e.what(),rootParser.file,XML_GetCurrentLineNumber(p));
        }
    }
  virtual void stream (const mystream& l)
    {
      DEBTRACE( "bloc_stream_set: " << l.fromnode() << l.fromport() << l.tonode() << l.toport() )
      std::string msg;
      std::string fromname = currentProc->names.back()+l.fromnode();
      std::string toname = currentProc->names.back()+l.tonode();
      //only relative names
      if(currentProc->nodeMap.count(fromname) == 0)
        {
            msg="from node " + l.fromnode() + " does not exist in stream link: ";
            msg=msg+l.fromnode()+"("+l.fromport()+")->"+l.tonode()+"("+l.toport()+")";
            currentProc->getLogger("parser")->error(msg,rootParser.file,XML_GetCurrentLineNumber(p));
            return;
        }
      if(currentProc->nodeMap.count(toname) == 0)
        {
            msg="to node " + l.tonode() + " does not exist in stream link: ";
            msg=msg+l.fromnode()+"("+l.fromport()+")->"+l.tonode()+"("+l.toport()+")";
            currentProc->getLogger("parser")->error(msg,rootParser.file,XML_GetCurrentLineNumber(p));
            return;
        }
      OutputDataStreamPort* pout=currentProc->nodeMap[fromname]->getOutputDataStreamPort(l.fromport());
      InputDataStreamPort* pin=currentProc->nodeMap[toname]->getInputDataStreamPort(l.toport());
      _bloc->edAddLink(pout,pin);
      // Set all properties for this link
      std::map<std::string, std::string>::const_iterator pt;
      for(pt=l._props.begin();pt!=l._props.end();pt++)
        {
          pin->setProperty((*pt).first,(*pt).second);
          pout->setProperty((*pt).first,(*pt).second);
        }
    }
  virtual void parameter (const myparam& param)
    {
      DEBTRACE( "++++++++++++++++++++Parameter+++++++++++++++++++++" );
      std::string msg;
      std::string toname = currentProc->names.back()+param._tonode;
      if(currentProc->nodeMap.count(toname) == 0)
        {
          msg="to node " + param._tonode + " does not exist in parameter: ";
          msg=msg+"->"+param._tonode+"("+param._toport+")";
          currentProc->getLogger("parser")->error(msg,rootParser.file,XML_GetCurrentLineNumber(p));
          return;
        }
      InputPort* inport=currentProc->nodeMap[toname]->getInputPort(param._toport);
      //We don't know the parameter type. So we try to initialize the port
      //with the value. If it's not the right type, edInit throws an exception
      try
        {
          inport->edInit("XML",param._value.c_str());
        }
      catch(Exception& e)
        {
          currentProc->getLogger("parser")->error(e.what(),rootParser.file,XML_GetCurrentLineNumber(p));
        }
      DEBTRACE( "++++++++++++++++++++End parameter+++++++++++++++++++++" );
    }
  T post()
    {
      DEBTRACE( "bloc_post" )             
      currentProc->names.pop_back();
      T b=_bloc;
      _blocs.pop_back();
      if(_blocs.empty())
        _bloc=NULL;
      else
        _bloc=_blocs.back();
      return b;
    }
  T _bloc;
  std::string _state;
  std::vector<Bloc *> _blocs;
};
static bloctypeParser<> blocParser;

template <>
void bloctypeParser<Bloc*>::name (const std::string& name)
{
  DEBTRACE( "bloc_name: " << name )             
  std::string fullname=currentProc->names.back()+name;
  _bloc=theRuntime->createBloc(name);
  _blocs.push_back(_bloc);
  currentProc->names.push_back(fullname+'.');
}

void switchtypeParser::onStart(const XML_Char* el, const XML_Char** attr)
{
  DEBTRACE( "switchtypeParser::onStart: " << el )             
  std::string element(el);
  this->maxcount("default",1,element);
  parser* pp=&main_parser;
  if(element == "case")pp=&caseParser;
  else if(element == "default")pp=&defaultcaseParser;
  XML_SetUserData(p,pp);
  sp.push(pp);
  pp->init();
  pp->pre();
  pp->buildAttr(attr);
}
void switchtypeParser::onEnd(const char *el,parser* child)
{
  DEBTRACE( "switchtypeParser::onEnd: " << el )             
  std::string element(el);
  if(element == "case")case_(((casetypeParser*)child)->post());
  else if(element == "default")default_(((defaultcasetypeParser*)child)->post());
}


void casetypeParser::onStart(const XML_Char* el, const XML_Char** attr)
{
  DEBTRACE( "casetypeParser::onStart: " << el )             
  std::string element(el);
  this->maxcount("inline",1,element);
  this->maxcount("sinline",1,element);
  this->maxcount("service",1,element);
  this->maxcount("node",1,element);
  this->maxcount("forloop",1,element);
  this->maxcount("foreach",1,element);
  this->maxcount("while",1,element);
  this->maxcount("switch",1,element);
  this->maxcount("bloc",1,element);
  this->maxchoice(t3,1,element);
  parser* pp=&main_parser;
  if(element == "property")pp=&propertyParser;
  else if(element == "inline")pp=&inlineParser;
  else if(element == "sinline")pp=&sinlineParser;
  else if(element == "service")pp=&serviceParser;
  else if(element == "node")pp=&nodeParser;
  else if(element == "forloop")pp=&forloopParser;
  else if(element == "foreach")pp=&foreachloopParser;
  else if(element == "while")pp=&whileloopParser;
  else if(element == "switch")pp=&switchParser;
  else if(element == "bloc")pp=&blocParser;
  XML_SetUserData(p,pp);
  sp.push(pp);
  pp->init();
  pp->pre();
  pp->buildAttr(attr);
}

void casetypeParser::onEnd(const char *el,parser* child)
{
  DEBTRACE( "casetypeParser::onEnd: " << el )             
  std::string element(el);
  if(element == "property")property(((propertytypeParser*)child)->post());
  else if(element == "inline")inline_(((inlinetypeParser<>*)child)->post());
  else if(element == "sinline")sinline(((sinlinetypeParser<>*)child)->post());
  else if(element == "service")service(((servicetypeParser<>*)child)->post());
  else if(element == "node")node(((nodetypeParser<>*)child)->post());
  else if(element == "forloop")forloop(((forlooptypeParser<>*)child)->post());
  else if(element == "foreach")foreach(((foreachlooptypeParser<>*)child)->post());
  else if(element == "while")while_(((whilelooptypeParser<>*)child)->post());
  else if(element == "switch")switch_(((switchtypeParser*)child)->post());
  else if(element == "bloc")bloc(((bloctypeParser<>*)child)->post());
}

template <class T>
void looptypeParser<T>::onStart(const XML_Char* el, const XML_Char** attr)
{
  DEBTRACE( "looptypeParser::onStart: " << el )             
  std::string element(el);
  this->maxcount("inline",1,element);
  this->maxcount("sinline",1,element);
  this->maxcount("service",1,element);
  this->maxcount("node",1,element);
  this->maxcount("forloop",1,element);
  this->maxcount("foreach",1,element);
  this->maxcount("while",1,element);
  this->maxcount("switch",1,element);
  this->maxcount("bloc",1,element);
  this->maxchoice(t3,1,element);
  parser* pp=&main_parser;
  if(element == "property")pp=&propertyParser;
  else if(element == "inline")pp=&inlineParser;
  else if(element == "sinline")pp=&sinlineParser;
  else if(element == "service")pp=&serviceParser;
  else if(element == "node")pp=&nodeParser;
  else if(element == "forloop")pp=&forloopParser;
  else if(element == "foreach")pp=&foreachloopParser;
  else if(element == "while")pp=&whileloopParser;
  else if(element == "switch")pp=&switchParser;
  else if(element == "bloc")pp=&blocParser;
  else if(element == "datalink")pp=&linkParser;
  XML_SetUserData(p,pp);
  sp.push(pp);
  pp->init();
  pp->pre();
  pp->buildAttr(attr);
}
template <class T>
void looptypeParser<T>::onEnd(const char *el,parser* child)
{
  DEBTRACE( "looptypeParser::onEnd: " << el )             
  std::string element(el);
  if(element == "property")property(((propertytypeParser*)child)->post());
  else if(element == "inline")inline_(((inlinetypeParser<>*)child)->post());
  else if(element == "sinline")sinline(((sinlinetypeParser<>*)child)->post());
  else if(element == "service")service(((servicetypeParser<>*)child)->post());
  else if(element == "node")node(((nodetypeParser<>*)child)->post());
  else if(element == "forloop")forloop(((forlooptypeParser<>*)child)->post());
  else if(element == "foreach")foreach(((foreachlooptypeParser<>*)child)->post());
  else if(element == "while")while_(((whilelooptypeParser<>*)child)->post());
  else if(element == "switch")switch_(((switchtypeParser*)child)->post());
  else if(element == "bloc")bloc(((bloctypeParser<>*)child)->post());
  else if(element == "datalink") datalink(((linktypeParser<>*)child)->post());
}

template <class T>
void bloctypeParser<T>::onStart(const XML_Char* el, const XML_Char** attr)
{
  DEBTRACE( "bloctypeParser::onStart: " << el )             
  std::string element(el);
  checkOrder(element);
  parser* pp=&main_parser;
  if(element == "property")pp=&propertyParser;
  else if(element == "inline")pp=&inlineParser;
  else if(element == "sinline")pp=&sinlineParser;
  else if(element == "service")pp=&serviceParser;
  else if(element == "node")pp=&nodeParser;
  else if(element == "forloop")pp=&forloopParser;
  else if(element == "foreach")pp=&foreachloopParser;
  else if(element == "while")pp=&whileloopParser;
  else if(element == "switch")pp=&switchParser;
  else if(element == "bloc")pp=&blocParser;
  else if(element == "control")pp=&controlParser;
  else if(element == "datalink")pp=&linkParser;
  else if(element == "stream")pp=&streamParser;
  else if(element == "parameter")pp=&paramParser;
  XML_SetUserData(p,pp);
  sp.push(pp);
  pp->init();
  pp->pre();
  pp->buildAttr(attr);
}
template <class T>
void bloctypeParser<T>::onEnd(const char *el,parser* child)
{
  DEBTRACE( "bloctypeParser::onEnd: " << el )             
  std::string element(el);
  if(element == "property")property(((propertytypeParser*)child)->post());
  else if(element == "inline")inline_(((inlinetypeParser<>*)child)->post());
  else if(element == "sinline")sinline(((sinlinetypeParser<>*)child)->post());
  else if(element == "service")service(((servicetypeParser<>*)child)->post());
  else if(element == "node")node(((nodetypeParser<>*)child)->post());
  else if(element == "forloop")forloop(((forlooptypeParser<>*)child)->post());
  else if(element == "foreach")foreach(((foreachlooptypeParser<>*)child)->post());
  else if(element == "while")while_(((whilelooptypeParser<>*)child)->post());
  else if(element == "switch")switch_(((switchtypeParser*)child)->post());
  else if(element == "bloc")bloc(((bloctypeParser<>*)child)->post());
  else if(element == "control") control(((controltypeParser<>*)child)->post());
  else if(element == "datalink") datalink(((linktypeParser<>*)child)->post());
  else if(element == "stream") stream(((streamtypeParser<>*)child)->post());
  else if(element == "parameter") parameter(((parametertypeParser*)child)->post());
}

template <class T=Proc*>
struct proctypeParser:bloctypeParser<T>
{
  proctypeParser():bloctypeParser<T>()
  {
    this->_orders["type"]=1;
    this->_orders["sequence"]=1;
    this->_orders["objref"]=1;
  }
  void onStart(const XML_Char* el, const XML_Char** attr)
  {
    DEBTRACE( "proctypeParser::onStart: " << el )             
    std::string element(el);
    this->checkOrder(element);
    parser* pp=&main_parser;
    if(element == "property")pp=&propertyParser;
    else if(element == "type")pp=&typeParser;
    else if(element == "sequence")pp=&seqParser;
    else if(element == "objref")pp=&objParser;
    else if(element == "struct")pp=&structParser;
    else if(element == "container")pp=&containerParser;
    else if(element == "inline")pp=&inlineParser;
    else if(element == "sinline")pp=&sinlineParser;
    else if(element == "service")pp=&serviceParser;
    else if(element == "node")pp=&nodeParser;
    else if(element == "forloop")pp=&forloopParser;
    else if(element == "foreach")pp=&foreachloopParser;
    else if(element == "while")pp=&whileloopParser;
    else if(element == "switch")pp=&switchParser;
    else if(element == "bloc")pp=&blocParser;
    else if(element == "control")pp=&controlParser;
    else if(element == "datalink")pp=&linkParser;
    else if(element == "stream")pp=&streamParser;
    else if(element == "parameter")pp=&paramParser;
    else 
      {
        // OCC: san -- Allow external parsers for handling of unknown elements
        // and attributes. This capability is used by YACS GUI to read
        // graph presentation data
        if ( this->_defaultParsersMap ) 
          {
            if((this->_defaultParsersMap)->count(element) != 0)
              {
                pp=(*(this->_defaultParsersMap))[element];
              }
            else
              {
                std::cerr << "There is no parser for this element type. It will be ignored!" << std::endl; 
              }
          }
      }
    XML_SetUserData(p,pp);
    sp.push(pp);
    pp->init();
    pp->pre();
    pp->buildAttr(attr);
  }
  virtual void onEnd(const char *el,parser* child)
    {
      DEBTRACE( "proctypeParser::onEnd: " << el )             
      std::string element(el);
      if(element == "property")this->property(((propertytypeParser*)child)->post());
      else if(element == "type")type(((typetypeParser*)child)->post());
      else if(element == "sequence")sequence(((seqtypeParser*)child)->post());
      else if(element == "objref")objref(((objtypeParser*)child)->post());
      else if(element == "struct")struct_(((structtypeParser*)child)->post());
      else if(element == "container")container(((containertypeParser*)child)->post());
      else if(element == "inline")this->inline_(((inlinetypeParser<>*)child)->post());
      else if(element == "sinline")this->sinline(((sinlinetypeParser<>*)child)->post());
      else if(element == "service")this->service(((servicetypeParser<>*)child)->post());
      else if(element == "node")this->node(((nodetypeParser<>*)child)->post());
      else if(element == "forloop")this->forloop(((forlooptypeParser<>*)child)->post());
      else if(element == "foreach")this->foreach(((foreachlooptypeParser<>*)child)->post());
      else if(element == "while")this->while_(((whilelooptypeParser<>*)child)->post());
      else if(element == "switch")this->switch_(((switchtypeParser*)child)->post());
      else if(element == "bloc")this->bloc(((bloctypeParser<>*)child)->post());
      else if(element == "control") this->control(((controltypeParser<>*)child)->post());
      else if(element == "datalink") this->datalink(((linktypeParser<>*)child)->post());
      else if(element == "stream") this->stream(((streamtypeParser<>*)child)->post());
      else if(element == "parameter") this->parameter(((parametertypeParser*)child)->post());
    }
  virtual void buildAttr(const XML_Char** attr)
    {
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "state")this->state(attr[i+1]);
        }
    }
  virtual void pre ()
    {
        std::string name("proc");
        currentProc=theRuntime->createProc(name);
        this->_bloc=currentProc;
        currentProc->names.push_back("");
    }
  virtual void type (const mytype& t)
    {
        DEBTRACE( "type_set" )             
        currentProc->typeMap[t._name]=currentProc->createType(t._name,t._kind);
    }
  virtual void sequence (TypeCode* const& t)
    {
        DEBTRACE( "sequence_set" )             
        currentProc->typeMap[t->name()]=t;
    }
  virtual void objref (TypeCode* const& t)
    {
        DEBTRACE( "objref_set" )             
        currentProc->typeMap[t->name()]=t;
    }
  virtual void struct_ (TypeCode* const& t)
    {
        DEBTRACE( "struct_set" )             
        currentProc->typeMap[t->name()]=t;
    }
  virtual void container (const mycontainer& t)
    {
      DEBTRACE( "container_set: " << t._name )             
      std::vector<machine>::const_iterator iter;
      for(iter=t._machs.begin();iter!=t._machs.end();iter++)
        {
          DEBTRACE( "machine name: " << (*iter)._name )             
        }

      if(currentProc->containerMap.count(t._name) == 0)
        {
          YACS::ENGINE::Container* cont=theRuntime->createContainer();
          // Set all properties for this container
          std::map<std::string, std::string>::const_iterator pt;
          for(pt=t._props.begin();pt!=t._props.end();pt++)
            cont->setProperty((*pt).first,(*pt).second);
          currentProc->containerMap[t._name]=cont;
        }
      else
        {
          std::cerr << "Warning: container " << t._name << " already defined. It will be ignored" << std::endl;
        }
    }

  T post(){return this->_bloc;}
};
static proctypeParser<> procParser;

void roottypeParser::onStart(const XML_Char* el, const XML_Char** attr)
{
      DEBTRACE( "roottypeParser::onStart: " << el )
      std::string element(el);
      parser* pp=&main_parser;
      if(element == "proc")pp=&procParser;
      XML_SetUserData(p,pp);
      sp.push(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
}

void roottypeParser::onEnd(const char *el,parser* child)
{
      DEBTRACE( "roottypeParser::onEnd: " << el )
      std::string element(el);
      if(element == "proc")proc(((proctypeParser<>*)child)->post());
}

YACSLoader::YACSLoader()
{
  theRuntime = getRuntime();
  //theRuntime->_catalogLoaderFactoryMap["proc"]=new YACS::ENGINE::ProcCataLoader;
}

void YACSLoader::registerProcCataLoader()
{
  YACS::ENGINE::ProcCataLoader* factory= new YACS::ENGINE::ProcCataLoader(this);
  theRuntime->setCatalogLoaderFactory("proc",factory);
}

Proc* YACSLoader::load(const char * file)
{
  FILE* fin=fopen(file,"r");
  if (! fin) 
    {
      std::cerr << "Couldn't open schema file" << std::endl;
      throw std::invalid_argument("Couldn't open schema file");
    }

  p = XML_ParserCreate(NULL);
  if (! p) 
    {
      std::cerr << "Couldn't allocate memory for parser" << std::endl;
      throw Exception("Couldn't allocate memory for parser");
    }
  XML_SetElementHandler(p, parser::start,parser::end);
  XML_SetCharacterDataHandler(p,parser::charac );
  XML_SetUserData(p,&rootParser);
  sp.push(&rootParser);
  // OCC: san -- Allow external parsers for handling of unknown elements
  // and attributes. This capability is used by YACS GUI to read
  // graph presentation data
  if ( !_defaultParsersMap.empty() )
    procParser._defaultParsersMap = &_defaultParsersMap;
  
  rootParser.file=file;

  try
    {
      for (;;) 
        {
          int done;
          int len;

          len = fread(Buff, 1, BUFFSIZE, fin);
          if (ferror(fin)) 
            {
              std::cerr << "Read error" << std::endl;
              throw Exception("Read error");
            }
          done = feof(fin);

          if (XML_Parse(p, Buff, len, done) == XML_STATUS_ERROR) 
            {
              YACS::ENGINE::Logger* logger=currentProc->getLogger("parser");
              logger->fatal(XML_ErrorString(XML_GetErrorCode(p)),file,XML_GetCurrentLineNumber(p));
              break;
            }

          if (done)
            break;
        }
      XML_ParserFree (p);
      p=0;
      return currentProc;
    }
  catch(Exception& e)
    {
      //get line number from XML parser
      YACS::ENGINE::Logger* logger=currentProc->getLogger("parser");
      logger->fatal(e.what(),file,XML_GetCurrentLineNumber(p));
      XML_ParserFree (p);
      p=0;
      return currentProc;
    }
}

YACSLoader::~YACSLoader()
{
}
