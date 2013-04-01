// Copyright (C) 2006-2013  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include "parserBase.hxx"
#include "Exception.hxx"
#include "Proc.hxx"
#include "Logger.hxx"
#include <sstream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

YACS::ENGINE::Proc* currentProc;
XML_Parser p ;
std::stack<YACS::parser*> sp;

namespace YACS
{
  parser parser::main_parser;

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

std::stack<parser*>& parser::getStack()
{
  return sp;
}

void parser::SetUserDataAndPush(parser* pp)
{
  XML_SetUserData(p,pp);
  sp.push(pp);
  DEBTRACE("parser::SetUserDataAndPush, stack size: " << sp.size());
}

void XMLCALL parser::start(void *data, const XML_Char* el, const XML_Char** attr)
{
  DEBTRACE("parser::start, stack size: " << sp.size());
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
  DEBTRACE("parser::end, stack size: " << sp.size());
  parser* pp=sp.top();
  XML_SetUserData(p,pp);
  pp->onEnd(el,child);
  child->endParser();
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

void parser::endParser()
{
  DEBTRACE("parser::endParser, level: " <<_level);
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
  DEBTRACE("parser::init, level: " <<_level);
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
      throw YACS::Exception(msg);
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
      throw YACS::Exception(msg.str());
    }
}

void parser::mincount(std::string name,int min )
{
  if((*_counts)[name]<min)
    {
      std::stringstream msg;
      msg<<"expected "+name+" element (count="<<(*_counts)[name];
      msg << " < minOccurs=" << min << ")";
      throw YACS::Exception(msg.str());
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
      throw YACS::Exception(msg.str());
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
      throw YACS::Exception(msg.str());
    }
}

void parser::required(const std::string& name, const XML_Char** attr)
{
  for (int i = 0; attr[i]; i += 2) 
    {
      if(name == std::string(attr[i]))return;
    }
  throw YACS::Exception("Attribute: "+name+" is required");
}

void parser::buildAttr(const XML_Char** attr)
{
  for (int i = 0; attr[i]; i += 2) 
    {
      DEBTRACE(attr[i] << "=" << attr[i + 1]);
    }
}

void parser::onStart(const XML_Char* el, const XML_Char** attr)
{
  DEBTRACE( "parser::onStart: " << el );
  SetUserDataAndPush(&main_parser);
  main_parser.init();
  main_parser.pre();
  main_parser.buildAttr(attr);
}

void parser::logError(const std::string& reason)
{
  DEBTRACE( "parser::logError: " << _file );
  currentProc->getLogger("parser")->error(reason,main_parser._file.c_str(),XML_GetCurrentLineNumber(p));
}

}
