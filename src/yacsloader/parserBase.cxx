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

#include "parserBase.hxx"
#include "Exception.hxx"
#include "Proc.hxx"
#include "Logger.hxx"
#include <sstream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"
#include <libxml/parserInternals.h>

YACS::ENGINE::Proc* currentProc;
_xmlParserCtxt* saxContext;

namespace YACS
{
  parser parser::main_parser;
  std::stack<parser*> parser::_stackParser;

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
  return _stackParser;
}

void parser::SetUserDataAndPush(parser* pp)
{
  XML_SetUserData(saxContext,pp);
  _stackParser.push(pp);
  DEBTRACE("parser::SetUserDataAndPush, stack size: " << sp.size());
}

void parser::onEnd(const XML_Char *el, parser* child)
{
  DEBTRACE("parser::onEnd: " << el)
}

void parser::charData(const XML_Char *s, int len)
{
  _content=_content+std::string(s,len);
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
  if((*_counts).find(el)==(*_counts).end())
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
  if (!attr)
    return;
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
  currentProc->getLogger("parser")->error(reason,main_parser._file.c_str(),saxContext->input->line);
}
void parser::XML_SetUserData(_xmlParserCtxt* ctxt,
                             parser* par)
{
  ctxt->userData = par;
}

void XMLCALL parser::start_document(void* data)
{
  DEBTRACE("parser::start_document");
  parser *currentParser = static_cast<parser *> (data);
}

void XMLCALL parser::end_document(void* data)
{
  DEBTRACE("parser::end_document");
  parser *currentParser = static_cast<parser *> (data);
}

void XMLCALL parser::start_element(void* data,
                                   const xmlChar* name,
                                   const xmlChar** p)
{
  DEBTRACE("parser::start_element " << name);
  parser *currentParser = static_cast<parser *> (data);
  currentParser->incrCount((const XML_Char *)name);
  currentParser->onStart((const XML_Char *)name, (const XML_Char **)p);
}

void XMLCALL parser::end_element(void* data,
                                 const xmlChar* name)
{
  DEBTRACE("parser::end_element");
  parser *childParser = static_cast<parser *> (data);
  _stackParser.pop();
  parser* pp=_stackParser.top();
  XML_SetUserData(saxContext, pp);
  pp->onEnd((const XML_Char *)name, childParser);
  childParser->endParser();
 }

void XMLCALL parser::characters(void* data,
                                const xmlChar* ch,
                                int len)
{
  DEBTRACE("parser::characters " << len);
  parser *currentParser = (parser *) (data);
  currentParser->charData((const XML_Char*) ch, len);
}

void XMLCALL parser::comment(void* data,
                             const xmlChar* value)
{
  DEBTRACE("parser::comment");
  parser *currentParser = static_cast<parser *> (data);
}

void XMLCALL parser::cdata_block(void* data,
                                 const xmlChar* value,
                                 int len)
{
  DEBTRACE("parser::cdata_block");
  parser *currentParser = static_cast<parser *> (data);
  currentParser->charData((const XML_Char*) value, len);
}

void XMLCALL parser::warning(void* data,
                             const char* fmt, ...)
{
  DEBTRACE("parser::warning");
  parser *currentParser = static_cast<parser *> (data);
  va_list args;
  va_start(args, fmt);
  std::string format = "%s";
  if (format == fmt)
    {
      char* parv;
      parv = va_arg(args, char*);
      std::cerr << parv ;
    }
  else std::cerr << __FILE__ << " [" << __LINE__ << "] : "
                 << "error format not taken into account: " << fmt << std::endl;
  va_end(args);
}

void XMLCALL parser::error(void* data,
                           const char* fmt, ...)
{
  DEBTRACE("parser::error");
  parser *currentParser = static_cast<parser *> (data);
  va_list args;
  va_start(args, fmt);
  std::string format = "%s";
  if (format == fmt)
    {
      char* parv;
      parv = va_arg(args, char*);
      std::cerr << parv ;
    }
  else std::cerr << __FILE__ << " [" << __LINE__ << "] : "
                 << "error format not taken into account: " << fmt << std::endl;
  va_end(args);
}

void XMLCALL parser::fatal_error(void* data,
                                 const char* fmt, ...)
{
  DEBTRACE("parser::fatal_error");
  parser *currentParser = static_cast<parser *> (data);
  va_list args;
  va_start(args, fmt);
  std::string format = "%s";
  if (format == fmt)
    {
      char* parv;
      parv = va_arg(args, char*);
      std::cerr << parv ;
    }
  else std::cerr << __FILE__ << " [" << __LINE__ << "] : "
                 << "error format not taken into account: " << fmt << std::endl;
  va_end(args);
}

}
