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

#include "componentinstanceParsers.hxx"
#include "containerParsers.hxx"
#include "dataParsers.hxx"
#include "propertyParsers.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

namespace YACS
{
  componentinstancetypeParser componentinstancetypeParser::componentinstanceParser;

  void componentinstancetypeParser::buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      required("name",attr);
      for (int i = 0; attr[i]; i += 2)
      {
        if(std::string(attr[i]) == "name")name(attr[i+1]);
        if(std::string(attr[i]) == "kind")kind(attr[i+1]);
      }
    }
  void componentinstancetypeParser::onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      parser* pp=&parser::main_parser;
      this->maxcount("component",1,element);
      this->maxcount("load",1,element);
      if(element == "property")pp=&propertytypeParser::propertyParser;
      else if(element == "component")pp=&stringtypeParser::stringParser;
      else if(element == "load")pp=&loadtypeParser::loadParser;
      SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  void componentinstancetypeParser::onEnd(const char *el,parser* child)
    {
      std::string element(el);
      if(element == "property")property(((propertytypeParser*)child)->post());
      else if(element == "component") component(((stringtypeParser*)child)->post());
      else if(element == "load") load(((loadtypeParser*)child)->post());
    }

  void componentinstancetypeParser::pre ()
    {
      _componentinstance._props.clear();
    }

  void componentinstancetypeParser::name(const std::string& name)
    { _componentinstance._name=name; }

  void componentinstancetypeParser::kind(const std::string& kind)
    { _componentinstance._kind=kind; }

  void componentinstancetypeParser::property (const myprop& prop)
    {
      DEBTRACE("property_set: "<<prop._name<<" "<<prop._value);
      _componentinstance._props[prop._name]=prop._value;
    }

  void componentinstancetypeParser::component (const std::string& name)
    {
      DEBTRACE( "component_set: " << name )
      _componentinstance._component=name;
    }

  void componentinstancetypeParser::load (const loadon& l)
    {
      DEBTRACE( "load: " )
      _componentinstance._container=l._container;
    }

  mycomponentinstance componentinstancetypeParser::post()
    {
      this->mincount("component",1);
      return _componentinstance;
    }
}
