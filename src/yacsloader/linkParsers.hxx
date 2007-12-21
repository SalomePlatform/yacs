
#ifndef _LINKPARSERS_HXX_
#define _LINKPARSERS_HXX_

#include "parserBase.hxx"
#include "dataParsers.hxx"
#include "propertyParsers.hxx"

#include "factory.hxx"

namespace YACS
{
template <class T=mycontrol>
struct controltypeParser: parser
{
  static controltypeParser<T> controlParser;

  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      parser* pp=&parser::main_parser;
      this->maxcount("fromnode",1,element);
      this->maxcount("tonode",1,element);
      if(element == "fromnode")pp=&stringtypeParser::stringParser;
      else if(element == "tonode")pp=&stringtypeParser::stringParser;
      this->SetUserDataAndPush(pp);
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

template <class T=mylink>
struct linktypeParser: controltypeParser<T>
{
  static linktypeParser<T> linkParser;

  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      this->maxcount("fromnode",1,element);
      this->maxcount("tonode",1,element);
      this->maxcount("fromport",1,element);
      this->maxcount("toport",1,element);
      parser* pp=&parser::main_parser;
      if(element == "fromnode")pp=&stringtypeParser::stringParser;
      else if(element == "tonode")pp=&stringtypeParser::stringParser;
      else if(element == "toport")pp=&stringtypeParser::stringParser;
      else if(element == "fromport")pp=&stringtypeParser::stringParser;
      else if(element == "property")pp=&propertytypeParser::propertyParser;
      this->SetUserDataAndPush(pp);
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

template <class T=mystream>
struct streamtypeParser: linktypeParser<T>
{
  static streamtypeParser<T> streamParser;
};

template <class T> streamtypeParser<T> streamtypeParser<T>::streamParser;
template <class T> controltypeParser<T> controltypeParser<T>::controlParser;
template <class T> linktypeParser<T> linktypeParser<T>::linkParser;

}

#endif