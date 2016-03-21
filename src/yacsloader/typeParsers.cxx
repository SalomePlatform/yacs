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

#include "typeParsers.hxx"
#include "dataParsers.hxx"
#include "Exception.hxx"
#include "Proc.hxx"
#include "Runtime.hxx"

#include <sstream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

extern YACS::ENGINE::Proc* currentProc;
extern YACS::ENGINE::Runtime* theRuntime;

namespace YACS
{
  typetypeParser typetypeParser::typeParser;
  seqtypeParser seqtypeParser::seqParser;
  objtypeParser objtypeParser::objParser;
  structtypeParser structtypeParser::structParser;
  membertypeParser membertypeParser::memberParser;

  void typetypeParser::buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      required("name",attr);
      required("kind",attr);
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
          if(std::string(attr[i]) == "kind")kind(attr[i+1]);
        }
    }
  void typetypeParser::pre (){}
  void typetypeParser::name(const std::string& name)
    {
      DEBTRACE( "type_name: " << name )             
      _name=name;
    }
  void typetypeParser::kind(const std::string& name)
    {
      DEBTRACE( "type_kind: " << name )             
      _kind=name;
    }
  mytype typetypeParser::post()
    {
      DEBTRACE( "type_post" )             
      mytype t;
      t._kind=_kind;
      t._name=_name;
      return t;
    }

  void seqtypeParser::onStart(const XML_Char* el, const XML_Char** attr)
    {
      DEBTRACE( "seqtypeParser::onStart: " << el )             
      parser* pp=&parser::main_parser;
      SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  void seqtypeParser::onEnd(const char *el,parser* child)
    {
      DEBTRACE( "seqtypeParser::onEnd: " << el )             
    }
  void seqtypeParser::buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      required("name",attr);
      required("content",attr);
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
          if(std::string(attr[i]) == "content")content(attr[i+1]);
        }
    }
  void seqtypeParser::name(const std::string& name)
    {
      DEBTRACE( "seqtype_name: " << name )             
      _name=name;
    }
  void seqtypeParser::content(const std::string& name)
    {
      DEBTRACE( "seqtype_content: " << name )             
      if(currentProc->typeMap.count(name)==0)
      {
        //Check if the typecode is defined in the runtime
        YACS::ENGINE::TypeCode* t=theRuntime->getTypeCode(name);
        if(t==0)
        {
          std::stringstream msg;
          msg << "Type " << name << " does not exist" << " (" <<__FILE__ << ":" << __LINE__ << ")";
          throw Exception(msg.str());
        }
        else
        {
          currentProc->typeMap[name]=t;
          t->incrRef();
        }
      }
      _contentType=currentProc->typeMap[name];

    }
  ENGINE::TypeCode* seqtypeParser::post()
    {
      DEBTRACE( "seqtype_post" )             
      ENGINE::TypeCode *t = currentProc->createSequenceTc(_name,_name,_contentType);
      return t;
    }

  void objtypeParser::onStart(const XML_Char* el, const XML_Char** attr)
    {
      DEBTRACE( "objtypeParser::onStart: " << el )             
      std::string element(el);
      parser* pp=&parser::main_parser;
      if(element == "base")pp=&stringtypeParser::stringParser;
      SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  void objtypeParser::onEnd(const char *el,parser* child)
    {
      DEBTRACE( "objtypeParser::onEnd: " << el )             
      std::string element(el);
      if(element == "base")base(((stringtypeParser*)child)->post());
    }
  void objtypeParser::buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      required("name",attr);
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
          if(std::string(attr[i]) == "id")id(attr[i+1]);
        }
    }
  void objtypeParser::pre ()
    {
      _id="";
      _ltc.clear();
    }
  void objtypeParser::name(const std::string& name)
    {
      DEBTRACE( "objtype_name: " << name )             
      _name=name;
    }
  void objtypeParser::id(const std::string& name)
    {
      DEBTRACE( "objtype_id: " << name )             
      _id=name;
    }
  void objtypeParser::base(const std::string& name)
    {
      DEBTRACE( "base_name: " << name )             
      if(currentProc->typeMap.count(name)==0)
      {
        //Check if the typecode is defined in the runtime
        YACS::ENGINE::TypeCode* t=theRuntime->getTypeCode(name);
        if(t==0)
        {
          std::stringstream msg;
          msg << "Type " << name << " does not exist" << " (" <<__FILE__ << ":" << __LINE__ << ")";
          throw Exception(msg.str());
        }
        else
        {
          currentProc->typeMap[name]=t;
          t->incrRef();
        }
      }
      if(currentProc->typeMap[name]->kind() != ENGINE::Objref)
        {
          std::stringstream msg;
          msg << "Type " << name << " is not an objref" ;
          msg << " (" <<__FILE__ << ":" << __LINE__ << ")";
          throw Exception(msg.str());
        }
      _ltc.push_back((ENGINE::TypeCodeObjref *)currentProc->typeMap[name]);
    }
  ENGINE::TypeCode * objtypeParser::post()
    {
      DEBTRACE( "objtype_post" )             
      ENGINE::TypeCode *t = currentProc->createInterfaceTc(_id,_name,_ltc);
      return t;
    }

  void membertypeParser::buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      required("name",attr);
      required("type",attr);
      for (int i = 0; attr[i]; i += 2)
      {
        if(std::string(attr[i]) == "name")name(attr[i+1]);
        if(std::string(attr[i]) == "type")type(attr[i+1]);
      }
    }
  void membertypeParser::name(const std::string& name){ _prop._name=name; }
  void membertypeParser::type(const std::string& name){ _prop._value=name; }
  myprop membertypeParser::post(){return _prop;}

  void structtypeParser::onStart(const XML_Char* el, const XML_Char** attr)
    {
      DEBTRACE( "structtypeParser::onStart: " << el )             
      std::string element(el);
      parser* pp=&parser::main_parser;
      if(element == "member")pp=&membertypeParser::memberParser;
      SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  void structtypeParser::onEnd(const char *el,parser* child)
    {
      DEBTRACE( "structtypeParser::onEnd: " << el )             
      std::string element(el);
      if(element == "member")member(((membertypeParser*)child)->post());
    }
  void structtypeParser::buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      required("name",attr);
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
          if(std::string(attr[i]) == "id")id(attr[i+1]);
        }
    }
  void structtypeParser::pre ()
    {
      _id="";
      _members.clear();
    }
  void structtypeParser::name(const std::string& name)
    {
      DEBTRACE( "structtype_name: " << name );
      _name=name;
    }
  void structtypeParser::id(const std::string& name)
    {
      DEBTRACE( "structtype_id: " << name );
      _id=name;
    }
  void structtypeParser::member (const myprop& prop)
    {
      DEBTRACE( "structtype_member: " << prop._name << prop._value );
      if(currentProc->typeMap.count(prop._value)==0)
      {
        //Check if the typecode is defined in the runtime
        YACS::ENGINE::TypeCode* t=theRuntime->getTypeCode(prop._value);
        if(t==0)
        {
          std::string msg="Unknown type " + prop._value + " for member " + prop._name + " in struct " + _name;
          throw Exception(msg);
        }
        else
        {
          currentProc->typeMap[prop._value]=t;
          t->incrRef();
        }
      }

      _members.push_back(prop);
    }
  ENGINE::TypeCode * structtypeParser::post()
    {
      DEBTRACE( "structtype_post" );
      ENGINE::TypeCodeStruct *t;
      if(currentProc->typeMap.count(_name)!=0)
        {
          //reuse a forward declaration
          ENGINE::TypeCode* tt=currentProc->typeMap[_name];
          if(tt->kind()==YACS::ENGINE::Struct)
            {
              t=(ENGINE::TypeCodeStruct*)tt;
            }
          else
            {
              std::string msg="Forward declaration must be a struct type but " + std::string(tt->name()) + " is not one" ;
              throw Exception(msg);
            }
        }
      else
        {
          t = (ENGINE::TypeCodeStruct*)currentProc->createStructTc(_id,_name);
        }
      std::vector<myprop>::const_iterator iter;
      for(iter=_members.begin();iter!=_members.end();iter++)
        {
          DEBTRACE("member: " << iter->_name << " " <<iter->_value);
          t->addMember(iter->_name,currentProc->typeMap[iter->_value]);
        }
      return t;
    }
}

