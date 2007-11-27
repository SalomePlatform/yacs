
#include "rootParser.hxx" 
#include "procParsers.hxx" 

namespace YACS
{
  roottypeParser roottypeParser::rootParser;
  defaultcasetypeParser defaultcasetypeParser::defaultcaseParser;
  switchtypeParser switchtypeParser::switchParser;
  casetypeParser casetypeParser::caseParser;
  presetdatatypeParser presetdatatypeParser::presetdataParser;
  outputdatatypeParser outputdatatypeParser::outputdataParser;

void roottypeParser::proc (YACS::ENGINE::Proc* const& b)
{
      DEBTRACE( "root_proc_set" << b->getName() )
      _proc=b;
}

void roottypeParser::onStart(const XML_Char* el, const XML_Char** attr)
{
      DEBTRACE( "roottypeParser::onStart: " << el )
      std::string element(el);
      parser* pp=&parser::main_parser;
      if(element == "proc")pp=&proctypeParser<>::procParser;
      SetUserDataAndPush(pp);
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

void roottypeParser::setDefaultMap(std::map<std::string,parser*> *defaultMap)
{
  proctypeParser<>::procParser._defaultParsersMap=defaultMap;
}

}
