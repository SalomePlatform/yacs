// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
      if (!attr)
        return;
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
