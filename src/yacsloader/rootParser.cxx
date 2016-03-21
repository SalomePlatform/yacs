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
#include "rootParser.hxx"

// Uncomment the following line to activate devdebug traces for ALL parsers defined in hxx files
//#define _DEVDEBUG_
#include "YacsTrace.hxx"

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
