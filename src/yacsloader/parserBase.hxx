//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef _PARSERBASE_HXX_
#define _PARSERBASE_HXX_

#include <expat.h>
#include <stack>
#include <map>
#include <string>
#include <iostream>

namespace YACS
{

struct parser
{
  static parser main_parser;

  parser():_level(0),_defaultParsersMap(0)
  {
    _counts=new std::map<std::string,int>;
  }
  virtual ~parser();

  virtual void SetUserDataAndPush(parser* pp);
  virtual void onStart(const XML_Char *el, const XML_Char** attr);
  static void XMLCALL start(void *data, const XML_Char* el, const XML_Char** attr);

  virtual void onEnd(const XML_Char *el,parser* child);
    
  static void XMLCALL end(void *data, const char *el);
    
  virtual void charData(const XML_Char *s, int len);
  
  static void XMLCALL charac(void *data, const XML_Char *s, int len);
    
  virtual void endParser();
    
  virtual void init();
    
  virtual void incrCount(const XML_Char *el);
    
  virtual void checkOrder(std::string& el);
    
  virtual void maxcount(std::string name, int max, std::string& el);
    
  virtual void mincount(std::string name,int min );
    
  virtual void maxchoice(std::string *names, int max, std::string& el);
    
  virtual void minchoice(std::string *names, int min);
    
  virtual void pre(){_content="";};
  virtual void required(const std::string& name, const XML_Char** attr);
    
  virtual void buildAttr(const XML_Char** attr);
    
  std::stack<parser*>& getStack();

  template<class T>
  T post() { }

  void logError(const std::string& reason);

  std::string _file;
  std::string _content;
  std::map<std::string,int> *_counts;
  std::map<std::string,int> _orders;
  int _orderState;
  int _level;
  std::stack<std::map<std::string,int>*> _stackCount;
  std::stack<int> _stackOrder;
  // OCC: san -- Allow external parsers for handling of unknown elements
  // and attributes. This capability is used by YACS GUI to read
  // graph presentation data
  std::map<std::string,parser*> *_defaultParsersMap;
};

}
#endif
