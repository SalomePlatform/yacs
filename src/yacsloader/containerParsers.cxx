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

#include "containerParsers.hxx"
#include "propertyParsers.hxx"
#include "HomogeneousPoolContainer.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

namespace YACS
{
  containertypeParser containertypeParser::containerParser;
  machinetypeParser machinetypeParser::machineParser;
  loadtypeParser loadtypeParser::loadParser;

  void machinetypeParser::buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      required("name",attr);
      for (int i = 0; attr[i]; i += 2)
      {
        if(std::string(attr[i]) == "name")name(attr[i+1]);
      }
    }
  void machinetypeParser::pre (){_mach._name="";}
  void machinetypeParser::name(const std::string& name){ _mach._name=name; }
  machine machinetypeParser::post()
    {
      return _mach;
    }

  void containertypeParser::buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      required("name",attr);
      for (int i = 0; attr[i]; i += 2)
      {
        if(std::string(attr[i]) == "name")name(attr[i+1]);
      }
    }
  void containertypeParser::onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::string element(el);
      parser* pp=&parser::main_parser;
      if(element == "machine")pp=&machinetypeParser::machineParser;
      if(element == "property")pp=&propertytypeParser::propertyParser;
      if(element == "initializescriptkey")pp=&codetypeParser::codeParser;
      SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  void containertypeParser::onEnd(const char *el,parser* child)
    {
      std::string element(el);
      if(element == "machine")machine_(((machinetypeParser*)child)->post());
      if(element == "property")property(((propertytypeParser*)child)->post());
      if(element == "initializescriptkey")initializescriptkey(((codetypeParser*)child)->post());
    }
  void containertypeParser::pre ()
    {
      _container._machs.clear();
      _container._props.clear();
    }
  void containertypeParser::name(const std::string& name){ _container._name=name; }
  void containertypeParser::machine_(const machine& m)
    {
      DEBTRACE( "machine: " << m._name )             
      _container._machs.push_back(m);
    }
  void containertypeParser::property (const myprop& prop)
    {
      DEBTRACE( "property_set: " << prop._name << " " << prop._value );
      _container._props[prop._name]=prop._value;
    }
  void containertypeParser::initializescriptkey(const myfunc& f)
  {
    _container._props[YACS::ENGINE::HomogeneousPoolContainer::INITIALIZE_SCRIPT_KEY]=f._code;
  }

  mycontainer containertypeParser::post()
    {
      //mincount("machine",1);
      return _container;
    }

  void loadtypeParser::buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      required("container",attr);
      for (int i = 0; attr[i]; i += 2)
      {
        if(std::string(attr[i]) == "container")container(attr[i+1]);
      }
    }
  void loadtypeParser::pre (){_loadon._container="";}
  void loadtypeParser::container(const std::string& name){ _loadon._container=name; }
  loadon loadtypeParser::post()
    {
      return _loadon;
    }

}
