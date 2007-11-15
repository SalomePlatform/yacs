
#ifndef _CODEPARSERS_HXX_
#define _CODEPARSERS_HXX_

#include "parserBase.hxx"
#include "dataParsers.hxx"

#include "factory.hxx"

namespace YACS
{

/*! \brief Class for code parser in inline nodes.
 *
 */
struct codetypeParser: parser
{
  static codetypeParser codeParser;
  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      parser* pp=&parser::main_parser;
      if(element == "code")pp=&stringtypeParser::stringParser;
      this->SetUserDataAndPush(pp);
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

/*! \brief Class for function parser in inline nodes.
 *
 */
struct functypeParser: codetypeParser
{
  static functypeParser funcParser;
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

}

#endif
