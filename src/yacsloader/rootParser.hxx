#ifndef _ROOTPARSER_HXX_
#define _ROOTPARSER_HXX_

#include "parserBase.hxx"

#include "Proc.hxx"

namespace YACS
{

struct roottypeParser:parser
{
  static roottypeParser rootParser;

  void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child);
  virtual void proc (YACS::ENGINE::Proc* const& b);
  void setDefaultMap(std::map<std::string,parser*> *);
  YACS::ENGINE::Proc* _proc;
  const char* file;
};

}

#endif
