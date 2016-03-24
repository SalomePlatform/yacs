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

#ifndef _LINKPARSERS_HXX_
#define _LINKPARSERS_HXX_

#include "parserBase.hxx"
#include "dataParsers.hxx"
#include "propertyParsers.hxx"

#include "factory.hxx"

namespace YACS
{
template <class T=mycontrol>
struct controltypeParser: parser
{
  static controltypeParser<T> controlParser;

  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      parser* pp=&parser::main_parser;
      this->maxcount("fromnode",1,element);
      this->maxcount("tonode",1,element);
      if(element == "fromnode")pp=&stringtypeParser::stringParser;
      else if(element == "tonode")pp=&stringtypeParser::stringParser;
      this->SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  virtual void onEnd(const char *el,parser* child)
    {
      std::string element(el);
      if(element == "fromnode")fromnode(((stringtypeParser*)child)->post());
      else if(element == "tonode")tonode(((stringtypeParser*)child)->post());
    }
  virtual void pre ()
    {
      _link.clear();
    }
  virtual void fromnode (const std::string& name)
    {
      _link.fromnode(name);
    }
  virtual void tonode (const std::string& name)
    {
      _link.tonode(name);
    }
  virtual void property (const myprop& prop)
    {
      DEBTRACE( "property_set: " << prop._name << prop._value )             
      _link.setProperty(prop._name,prop._value);
    }
  virtual T& post()
    {
      mincount("fromnode",1);
      mincount("tonode",1);
      return _link;
    }
    T _link;
};

template <class T=mylink>
struct linktypeParser: controltypeParser<T>
{
  static linktypeParser<T> linkParser;

  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      this->maxcount("fromnode",1,element);
      this->maxcount("tonode",1,element);
      this->maxcount("fromport",1,element);
      this->maxcount("toport",1,element);
      parser* pp=&parser::main_parser;
      if(element == "fromnode")pp=&stringtypeParser::stringParser;
      else if(element == "tonode")pp=&stringtypeParser::stringParser;
      else if(element == "toport")pp=&stringtypeParser::stringParser;
      else if(element == "fromport")pp=&stringtypeParser::stringParser;
      else if(element == "property")pp=&propertytypeParser::propertyParser;
      this->SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  virtual void onEnd(const char *el,parser* child)
    {
      std::string element(el);
      if(element == "fromnode")this->fromnode(((stringtypeParser*)child)->post());
      else if(element == "tonode")this->tonode(((stringtypeParser*)child)->post());
      else if(element == "toport")toport(((stringtypeParser*)child)->post());
      else if(element == "fromport")fromport(((stringtypeParser*)child)->post());
      else if(element == "property")this->property(((propertytypeParser*)child)->post());
    }
  virtual void buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      for (int i = 0; attr[i]; i += 2) 
        {
          if((std::string(attr[i]) == "control")
             && (std::string(attr[i+1]) == "false"))
            this->_link._withControl=false;
        }
    }
  virtual void fromport (const std::string& name)
    {
      this->_link.fromport(name);
    }
  virtual void toport (const std::string& name)
    {
      this->_link.toport(name);
    }
  virtual T& post()
    {
      this->mincount("fromnode",1);
      this->mincount("tonode",1);
      this->mincount("fromport",1);
      this->mincount("toport",1);
      return this->_link;
    }
};

template <class T=mystream>
struct streamtypeParser: linktypeParser<T>
{
  static streamtypeParser<T> streamParser;
};

template <class T> streamtypeParser<T> streamtypeParser<T>::streamParser;
template <class T> controltypeParser<T> controltypeParser<T>::controlParser;
template <class T> linktypeParser<T> linktypeParser<T>::linkParser;

}

#endif
