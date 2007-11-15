
#include "containerParsers.hxx"
#include "propertyParsers.hxx"

namespace YACS
{
  containertypeParser containertypeParser::containerParser;
  machinetypeParser machinetypeParser::machineParser;
  loadtypeParser loadtypeParser::loadParser;

  void machinetypeParser::buildAttr(const XML_Char** attr)
    {
      required("name",attr);
      for (int i = 0; attr[i]; i += 2)
      {
        if(std::string(attr[i]) == "name")name(attr[i+1]);
      }
    }
  void machinetypeParser::pre (){_mach._name="";}
  void machinetypeParser::name(const std::string& name){ _mach._name=name; }
  machine machinetypeParser::post()
    {
      return _mach;
    }

  void containertypeParser::buildAttr(const XML_Char** attr)
    {
      required("name",attr);
      for (int i = 0; attr[i]; i += 2)
      {
        if(std::string(attr[i]) == "name")name(attr[i+1]);
      }
    }
  void containertypeParser::onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      parser* pp=&parser::main_parser;
      if(element == "machine")pp=&machinetypeParser::machineParser;
      if(element == "property")pp=&propertytypeParser::propertyParser;
      SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  void containertypeParser::onEnd(const char *el,parser* child)
    {
      std::string element(el);
      if(element == "machine")machine_(((machinetypeParser*)child)->post());
      if(element == "property")property(((propertytypeParser*)child)->post());
    }
  void containertypeParser::pre (){_container._machs.clear();}
  void containertypeParser::name(const std::string& name){ _container._name=name; }
  void containertypeParser::machine_(const machine& m)
    {
      DEBTRACE( "machine: " << m._name )             
      _container._machs.push_back(m);
    }
  void containertypeParser::property (const myprop& prop)
    {
      DEBTRACE( "property_set: " << prop._name << prop._value )             
      _container._props[prop._name]=prop._value;
    }
  mycontainer containertypeParser::post()
    {
      //mincount("machine",1);
      return _container;
    }

  void loadtypeParser::buildAttr(const XML_Char** attr)
    {
      required("container",attr);
      for (int i = 0; attr[i]; i += 2)
      {
        if(std::string(attr[i]) == "container")container(attr[i+1]);
      }
    }
  void loadtypeParser::pre (){_loadon._container="";}
  void loadtypeParser::container(const std::string& name){ _loadon._container=name; }
  loadon loadtypeParser::post()
    {
      return _loadon;
    }

}
