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

#include "xmlrpcParsers.hxx"
#include "dataParsers.hxx"
#include <sstream>
#include <iomanip>

namespace YACS
{
  arraytypeParser arraytypeParser::arrayParser;
  valuetypeParser valuetypeParser::valueParser;
  parametertypeParser parametertypeParser::parameterParser;
  datatypeParser datatypeParser::dataParser;
  memberdatatypeParser memberdatatypeParser::memberdataParser;
  structdatatypeParser structdatatypeParser::structdataParser;

static std::string t4[]={"string","objref","double","int","boolean","array","struct",""};

  void valuetypeParser::pre (){ }
  void valuetypeParser::int_ (const int& d)
    {
      std::ostringstream os;
      os << "<int>"   << d<< "</int>";
      _data=os.str();
      _v.push_back(_data);
    }
  void valuetypeParser::boolean (const bool& d)
    {
      std::ostringstream os;
      os << "<boolean>"   << d<< "</boolean>";
      _data=os.str();
      _v.push_back(_data);
    }
  void valuetypeParser::double_ (const double& d)
    {
      std::ostringstream os;
      os << "<double>"<< std::setprecision(16) << d<< "</double>";
      _data=os.str();
      _v.push_back(_data);
    }
  void valuetypeParser::string(const std::string& d)
    {
      _data="<string>"+ d+ "</string>";
      _v.push_back(_data);
    }
  void valuetypeParser::objref(const std::string& d)
    {
      _data="<objref>"+ d+ "</objref>";
      _v.push_back(_data);
    }
  void valuetypeParser::array (const std::string& d)
    {
      _v.push_back(d);
    }
  void valuetypeParser::struct_ (const std::string& d)
    {
      _v.push_back(d);
    }
  std::string valuetypeParser::post()
    {
      minchoice(t4,1);
      std::string value="<value>"+_v.back()+"</value>\n";
      _v.pop_back();
      return value;
    }

  void datatypeParser::onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      parser* pp=&parser::main_parser;
      if(element == "value")pp=&valuetypeParser::valueParser;
      SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  void datatypeParser::onEnd(const char *el,parser* child)
    {
      std::string element(el);
      if(element == "value")value(((valuetypeParser*)child)->post());
    }
  void datatypeParser::pre ()
    {
      _datas.push_back(_data);
      _data="";
    }
  void datatypeParser::value (const std::string& v){
      _data=_data+v;
    }
  std::string datatypeParser::post()
    {
      std::string d="<data>\n"+_data+"</data>";
      _data=_datas.back();
      _datas.pop_back();
      return d;
    }

  void memberdatatypeParser::onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      parser* pp=&parser::main_parser;
      if(element == "name")pp=&stringtypeParser::stringParser;
      else if(element == "value")pp=&valuetypeParser::valueParser;
      SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  void memberdatatypeParser::onEnd(const char *el,parser* child)
    {
      std::string element(el);
      this->maxcount("name",1,element);
      this->maxcount("value",1,element);
      if(element == "name")name(((stringtypeParser*)child)->post());
      else if(element == "value")value(((valuetypeParser*)child)->post());
    }
  void memberdatatypeParser::pre ()
    {
      _datas.push_back(_data);
      _data="";
    }
  void memberdatatypeParser::name (const std::string& v)
    {
      _data=_data+"<name>"+v+"</name>";
    }
  void memberdatatypeParser::value (const std::string& v)
    {
      _data=_data+v;
    }
  std::string memberdatatypeParser::post()
    {
      mincount("value",1);
      mincount("name",1);
      std::string d="<member>\n"+_data+"</member>";
      _data=_datas.back();
      _datas.pop_back();
      return d;
    }

  void structdatatypeParser::onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      parser* pp=&parser::main_parser;
      if(element == "member")pp=&memberdatatypeParser::memberdataParser;
      SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  void structdatatypeParser::onEnd(const char *el,parser* child)
    {
      std::string element(el);
      if(element == "member")member(((memberdatatypeParser*)child)->post());
    }
  void structdatatypeParser::pre ()
    {
      _membersStack.push_back(_members);
      _members="";
    }
  void structdatatypeParser::member (const std::string& d)
    {
      _members=_members+d;
    }
  std::string structdatatypeParser::post()
    {
      std::string value="<struct>"+_members+"</struct>";
      _members=_membersStack.back();
      _membersStack.pop_back();
      return value;
    }

  void arraytypeParser::onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      this->maxcount("data",1,element);
      parser* pp=&parser::main_parser;
      if(element == "data")pp=&datatypeParser::dataParser;
      SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  void arraytypeParser::onEnd(const char *el,parser* child)
    {
      std::string element(el);
      if(element == "data")data(((datatypeParser*)child)->post());
    }
  void arraytypeParser::pre (){ }
  void arraytypeParser::data (const std::string& d)
    {
      _arrays.push_back(d);
    }
  std::string arraytypeParser::post()
    {
      mincount("data",1);
      std::string value="<array>"+_arrays.back()+"</array>";
      _arrays.pop_back();
      return value;
    }


void valuetypeParser::onStart(const XML_Char* el, const XML_Char** attr)
{
  std::string element(el);
  parser* pp=&parser::main_parser;
  this->maxcount("string",1,element);
  this->maxcount("objref",1,element);
  this->maxcount("double",1,element);
  this->maxcount("int",1,element);
  this->maxcount("boolean",1,element);
  this->maxcount("array",1,element);
  this->maxcount("struct",1,element);
  this->maxchoice(t4,1,element);
  if(element == "string")pp=&stringtypeParser::stringParser;
  else if(element == "objref")pp=&stringtypeParser::stringParser;
  else if(element == "double")pp=&doubletypeParser::doubleParser;
  else if(element == "int")pp=&inttypeParser::intParser;
  else if(element == "boolean")pp=&booltypeParser::boolParser;
  else if(element == "array")pp=&arraytypeParser::arrayParser;
  else if(element == "struct")pp=&structdatatypeParser::structdataParser;
  SetUserDataAndPush(pp);
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

  void parametertypeParser::onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      this->maxcount("tonode",1,element);
      this->maxcount("toport",1,element);
      this->maxcount("value",1,element);
      parser* pp=&parser::main_parser;
      if(element == "tonode")pp=&stringtypeParser::stringParser;
      else if(element == "toport")pp=&stringtypeParser::stringParser;
      else if(element == "value")pp=&valuetypeParser::valueParser;
      SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  void parametertypeParser::onEnd(const char *el,parser* child)
    {
      std::string element(el);
      if(element == "tonode")tonode(((stringtypeParser*)child)->post());
      else if(element == "toport")toport(((stringtypeParser*)child)->post());
      else if(element == "value")value(((valuetypeParser*)child)->post());
    }
    void parametertypeParser::pre (){}
    void parametertypeParser::tonode (const std::string& name){
      _param._tonode=name;
    }
    void parametertypeParser::toport (const std::string& name){
      _param._toport=name;
    }
    void parametertypeParser::value (const std::string& name){
      _param._value=name;
    }
    myparam& parametertypeParser::post(){
      mincount("tonode",1);
      mincount("toport",1);
      mincount("value",1);
      return _param;
    }

}
