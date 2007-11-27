#ifndef _PROCPARSER_HXX_
#define _PROCPARSER_HXX_

#include "blocParsers.hxx"
#include "typeParsers.hxx"
#include "containerParsers.hxx"
#include "nodeParsers.hxx"

#include "Proc.hxx"
#include "Container.hxx"

extern YACS::ENGINE::Proc* currentProc;

namespace YACS
{

template <class T=YACS::ENGINE::Proc*>
struct proctypeParser: bloctypeParser<T>
{

  static proctypeParser<T> procParser;

  proctypeParser():bloctypeParser<T>()
  {
    this->_orders["type"]=1;
    this->_orders["sequence"]=1;
    this->_orders["objref"]=1;
  }
  void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child);
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
  virtual void sequence (ENGINE::TypeCode* const& t)
    {
        DEBTRACE( "sequence_set" )             
        currentProc->typeMap[t->name()]=t;
    }
  virtual void objref (ENGINE::TypeCode* const& t)
    {
        DEBTRACE( "objref_set" )             
        currentProc->typeMap[t->name()]=t;
    }
  virtual void struct_ (ENGINE::TypeCode* const& t)
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
          cont->setName(t._name);
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

template <class T> proctypeParser<T> proctypeParser<T>::procParser;

}

namespace YACS
{
template <class T>
void proctypeParser<T>::onStart(const XML_Char* el, const XML_Char** attr)
  {
    DEBTRACE( "proctypeParser::onStart: " << el )
    std::string element(el);
    this->checkOrder(element);
    parser* pp=&parser::main_parser;
    if(element == "property")pp=&propertytypeParser::propertyParser;
    else if(element == "type")pp=&typetypeParser::typeParser;
    else if(element == "sequence")pp=&seqtypeParser::seqParser;
    else if(element == "objref")pp=&objtypeParser::objParser;
    else if(element == "struct")pp=&structtypeParser::structParser;
    else if(element == "container")pp=&containertypeParser::containerParser;

    else if(element == "inline")pp=&inlinetypeParser<>::inlineParser;
    else if(element == "sinline")pp=&sinlinetypeParser<>::sinlineParser;
    else if(element == "service")pp=&servicetypeParser<>::serviceParser;
    else if(element == "node")pp=&nodetypeParser<>::nodeParser;
    else if(element == "datanode")pp=&presettypeParser<>::presetParser;
    else if(element == "outnode")pp=&outnodetypeParser<>::outnodeParser;

    else if(element == "bloc")pp=&bloctypeParser<>::blocParser;
    else if(element == "forloop")pp=&forlooptypeParser<>::forloopParser;
    else if(element == "foreach")pp=&foreachlooptypeParser<>::foreachloopParser;
    else if(element == "while")pp=&whilelooptypeParser<>::whileloopParser;
    else if(element == "switch")pp=&switchtypeParser::switchParser;

    else if(element == "control")pp=&controltypeParser<>::controlParser;
    else if(element == "datalink")pp=&linktypeParser<>::linkParser;
    else if(element == "stream")pp=&streamtypeParser<>::streamParser;
    else if(element == "parameter")pp=&parametertypeParser::parameterParser;
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
    this->SetUserDataAndPush(pp);
    pp->init();
    pp->pre();
    pp->buildAttr(attr);
  }

template <class T>
void proctypeParser<T>::onEnd(const char *el,parser* child)
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
      else if(element == "datanode")this->preset(((presettypeParser<>*)child)->post());
      else if(element == "outnode")this->outnode(((outnodetypeParser<>*)child)->post());

      else if(element == "bloc")this->bloc(((bloctypeParser<>*)child)->post());
      else if(element == "forloop")this->forloop(((forlooptypeParser<>*)child)->post());
      else if(element == "foreach")this->foreach(((foreachlooptypeParser<>*)child)->post());
      else if(element == "while")this->while_(((whilelooptypeParser<>*)child)->post());
      else if(element == "switch")this->switch_(((switchtypeParser*)child)->post());
 
      else if(element == "control") this->control(((controltypeParser<>*)child)->post());
      else if(element == "datalink") this->datalink(((linktypeParser<>*)child)->post());
      else if(element == "stream") this->stream(((streamtypeParser<>*)child)->post());
      else if(element == "parameter") this->parameter(((parametertypeParser*)child)->post());
    }

}

#endif
