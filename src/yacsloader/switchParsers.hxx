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

#ifndef _SWITCHPARSERS_HXX_
#define _SWITCHPARSERS_HXX_

#include "parserBase.hxx"
#include "propertyParsers.hxx"
#include "nodeParsers.hxx"

#include "Loop.hxx"
#include "ForLoop.hxx"
#include "ForEachLoop.hxx"
#include "WhileLoop.hxx"
#include "Switch.hxx"
#include "Bloc.hxx"
#include "Proc.hxx"
#include "InlineNode.hxx"
#include "ServiceNode.hxx"
#include "ServiceInlineNode.hxx"
#include "Runtime.hxx"

#include "factory.hxx"

#include <sstream>
#include <string>
#include <vector>
#include <map>

extern YACS::ENGINE::Proc* currentProc;
extern YACS::ENGINE::Runtime* theRuntime;

namespace YACS
{

struct casetypeParser:parser
{
  static casetypeParser caseParser;
  void onStart(const XML_Char* el, const XML_Char** attr);
  void onEnd(const char *el,parser* child);
  virtual void buildAttr(const XML_Char** attr);
  virtual void pre ();
  virtual void id (const int& n);
  virtual void property (const myprop& prop);
  virtual void inline_ (ENGINE::InlineNode* const& n);
  virtual void sinline (ENGINE::ServiceInlineNode* const& n);
  virtual void service (ENGINE::ServiceNode* const& n);
  virtual void server (ENGINE::ServerNode* const& n);
  virtual void remote (ENGINE::InlineNode* const& n);
  virtual void node (ENGINE::InlineNode* const& n);
  virtual void forloop (ENGINE::ForLoop* const& n);
  virtual void foreach (ENGINE::ForEachLoop* const& n);
  virtual void optimizer (ENGINE::OptimizerLoop* const& n);
  virtual void while_ (ENGINE::WhileLoop* const& n);
  virtual void switch_ (ENGINE::Switch* const& n);
  virtual void bloc (ENGINE::Bloc* const& n);
  virtual std::pair<int,ENGINE::Node*> post();
  ENGINE::Node* _cnode;
  int _id;
  std::vector<int> _idStack;
};

struct defaultcasetypeParser:casetypeParser
{
  static defaultcasetypeParser defaultcaseParser;
  virtual void buildAttr(const XML_Char** attr);
  virtual void pre ();
};

struct switchtypeParser:parser
{
  static switchtypeParser switchParser;
  void onStart(const XML_Char* el, const XML_Char** attr);
  void onEnd(const char *el,parser* child);
  virtual void buildAttr(const XML_Char** attr);
  virtual void pre ();
  virtual void case_ (const std::pair<int,ENGINE::Node*>& p);
  virtual void default_ (const std::pair<int,ENGINE::Node*>& p);
  virtual void name (const std::string& name);
  virtual void state (const std::string& state);
  virtual void select (const int& s);
  virtual ENGINE::Switch* post ();
  // stack to store switches in case of switch in switch
  std::vector<ENGINE::Switch *> _cnodes;
  std::string _state;
};

}

#include "loopParsers.hxx"

namespace YACS
{

  static std::string switch_t3[]={"inline","sinline","service","server", "remote", "node","forloop","foreach","optimizer","while","switch","bloc",""};

  void casetypeParser::buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      this->required("id",attr);
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "id")id(atoi(attr[i+1]));
        }
    }
  void casetypeParser::pre ()
    {
      _cnode=0;
      _id=0;
    }
  void casetypeParser::id (const int& n)
    {
      DEBTRACE( "case_id: " << n )             
      _id=n;
      //store this level id
      _idStack.push_back(_id);
      //store this level name
      std::stringstream temp;
      if (_id <0) temp << "m" << -_id << "_";
      else temp << "p" << _id << "_";
      std::string fullname=currentProc->names.back()+temp.str();
      DEBTRACE( "case_fullname: " << fullname )             
      currentProc->names.push_back(fullname);
    }
  void casetypeParser::property (const myprop& prop)
    {
      DEBTRACE( "property_set: " << prop._name << prop._value )             
    }
  void casetypeParser::inline_ (ENGINE::InlineNode* const& n)
    {
      _cnode=n;
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->inlineMap[fullname]=n;
    }
  void casetypeParser::sinline (ENGINE::ServiceInlineNode* const& n)
    {
      _cnode=n;
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->serviceMap[fullname]=n;
    }
  void casetypeParser::service (ENGINE::ServiceNode* const& n)
    {
      _cnode=n;
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->serviceMap[fullname]=n;
    }
  void casetypeParser::server (ENGINE::ServerNode* const& n)
    {
      _cnode=n;
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->inlineMap[fullname]=n;
    }
  void casetypeParser::remote (ENGINE::InlineNode* const& n)
    {
      _cnode=n;
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->inlineMap[fullname]=n;
    }
  void casetypeParser::node (ENGINE::InlineNode* const& n)
    {
      _cnode=n;
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->inlineMap[fullname]=n;
    }
  void casetypeParser::forloop (ENGINE::ForLoop* const& n)
    {
      _cnode=n;
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
    }
  void casetypeParser::foreach (ENGINE::ForEachLoop* const& n)
    {
      _cnode=n;
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
      fullname += ".splitter";
      currentProc->nodeMap[fullname]=n->getChildByShortName("splitter");
    }
  void casetypeParser::optimizer (ENGINE::OptimizerLoop* const& n)
    {
      _cnode=n;
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
      //fullname += ".splitter";
      //currentProc->nodeMap[fullname]=n->getChildByShortName("splitter");
    }
  void casetypeParser::while_ (ENGINE::WhileLoop* const& n)
    {
      _cnode=n;
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
    }
  void casetypeParser::switch_ (ENGINE::Switch* const& n)
    {
      _cnode=n;
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
    }
  void casetypeParser::bloc (ENGINE::Bloc* const& n)
    {
      _cnode=n;
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
    }
  std::pair<int,ENGINE::Node*> casetypeParser::post()
    {
      DEBTRACE( "case_post" )             
      minchoice(switch_t3,1);
      //get back this level id
      _id=_idStack.back();
      _idStack.pop_back();
      //pop back this level name
      currentProc->names.pop_back();
      return std::pair<int,ENGINE::Node*>(_id,_cnode);
    }

  void defaultcasetypeParser::buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      for (int i = 0; attr[i]; i += 2) 
      {
        DEBTRACE( attr[i] << "=" << attr[i + 1] )             
      }
    }
  void defaultcasetypeParser::pre ()
    {
      _id=0;
      _cnode=0;
      //store this level id
      _idStack.push_back(_id);
      //store this level name
      std::string fullname=currentProc->names.back()+"default_";
      DEBTRACE( "case_fullname: " << fullname )             
      currentProc->names.push_back(fullname);
    }

  void switchtypeParser::buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      this->required("name",attr);
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
          if(std::string(attr[i]) == "state")state(attr[i+1]);
          if(std::string(attr[i]) == "select")select(atoi(attr[i+1]));
        }
    }
  void switchtypeParser::pre (){_state="";}
  void switchtypeParser::case_ (const std::pair<int,ENGINE::Node*>& p)
    {
      ENGINE::Switch* s=_cnodes.back();
      s->edSetNode(p.first,p.second);
    }
  void switchtypeParser::default_ (const std::pair<int,ENGINE::Node*>& p)
    {
      ENGINE::Switch* s=_cnodes.back();
      s->edSetDefaultNode(p.second);
    }
  void switchtypeParser::name (const std::string& name)
    {
      ENGINE::Switch* s;
      std::string fullname=currentProc->names.back()+name;
      DEBTRACE( "switch_fullname: " << fullname )             
      s=theRuntime->createSwitch(name);
      _cnodes.push_back(s);
      currentProc->names.push_back(fullname+'.');
    }
  void switchtypeParser::state (const std::string& state)
    {
      //state is an attribute (no order). It can be defined before name
      //To be improved
      ENGINE::Switch* s=_cnodes.back();
      if(_state == "disabled")
        {
          DEBTRACE( "Switch disabled: " << s->getName())             
          s->exDisabledState();
        }
    }
  void switchtypeParser::select (const int& s)
    {
      //select is an attribute
      ENGINE::Switch* sw=_cnodes.back();
      ENGINE::InputPort *p=sw->edGetConditionPort();
      p->edInit(s);
    }
  ENGINE::Switch* switchtypeParser::post ()
    {
      DEBTRACE( "switch_post: " )             
      ENGINE::Switch* sw=_cnodes.back();
      //pop back current level name and node
      _cnodes.pop_back();
      currentProc->names.pop_back();
      return sw;
    }

void switchtypeParser::onStart(const XML_Char* el, const XML_Char** attr)
{
  DEBTRACE( "switchtypeParser::onStart: " << el )
  std::string element(el);
  this->maxcount("default",1,element);
  parser* pp=&parser::main_parser;
  if(element == "case")pp=&casetypeParser::caseParser;
  else if(element == "default")pp=&defaultcasetypeParser::defaultcaseParser;
  SetUserDataAndPush(pp);
  pp->init();
  pp->pre();
  pp->buildAttr(attr);
}

void switchtypeParser::onEnd(const char *el,parser* child)
{
  DEBTRACE( "switchtypeParser::onEnd: " << el )
  std::string element(el);
  if(element == "case")case_(((casetypeParser*)child)->post());
  else if(element == "default")default_(((defaultcasetypeParser*)child)->post());
}

void casetypeParser::onStart(const XML_Char* el, const XML_Char** attr)
{
  DEBTRACE( "casetypeParser::onStart: " << el )
  std::string element(el);
  this->maxcount("inline",1,element);
  this->maxcount("sinline",1,element);
  this->maxcount("service",1,element);
  this->maxcount("server",1,element);
  this->maxcount("remote",1,element);
  this->maxcount("node",1,element);
  this->maxcount("forloop",1,element);
  this->maxcount("foreach",1,element);
  this->maxcount("optimizer",1,element);
  this->maxcount("while",1,element);
  this->maxcount("switch",1,element);
  this->maxcount("bloc",1,element);
  this->maxchoice(switch_t3,1,element);
  parser* pp=&parser::main_parser;
  if(element == "property")pp=&propertytypeParser::propertyParser;
  else if(element == "inline")pp=&inlinetypeParser<>::inlineParser;
  else if(element == "sinline")pp=&sinlinetypeParser<>::sinlineParser;
  else if(element == "service")pp=&servicetypeParser<>::serviceParser;
  else if(element == "server")pp=&servertypeParser<>::serverParser;
  else if(element == "remote")pp=&remotetypeParser<>::remoteParser;
  else if(element == "node")pp=&nodetypeParser<>::nodeParser;
  else if(element == "forloop")pp=&forlooptypeParser<>::forloopParser;
  else if(element == "foreach")pp=&foreachlooptypeParser<>::foreachloopParser;
  else if(element == "optimizer")pp=&optimizerlooptypeParser<>::optimizerloopParser;
  else if(element == "while")pp=&whilelooptypeParser<>::whileloopParser;
  else if(element == "switch")pp=&switchtypeParser::switchParser;
  else if(element == "bloc")pp=&bloctypeParser<>::blocParser;
  SetUserDataAndPush(pp);
  pp->init();
  pp->pre();
  pp->buildAttr(attr);
}

void casetypeParser::onEnd(const char *el,parser* child)
{
  DEBTRACE( "casetypeParser::onEnd: " << el )
  std::string element(el);
  if(element == "property")property(((propertytypeParser*)child)->post());
  else if(element == "inline")inline_(((inlinetypeParser<>*)child)->post());
  else if(element == "sinline")sinline(((sinlinetypeParser<>*)child)->post());
  else if(element == "service")service(((servicetypeParser<>*)child)->post());
  else if(element == "server")server(((servertypeParser<>*)child)->post());
  else if(element == "remote")remote(((remotetypeParser<>*)child)->post());
  else if(element == "node")node(((nodetypeParser<>*)child)->post());
  else if(element == "forloop")forloop(((forlooptypeParser<>*)child)->post());
  else if(element == "foreach")foreach(((foreachlooptypeParser<>*)child)->post());
  else if(element == "optimizer")optimizer(((optimizerlooptypeParser<>*)child)->post());
  else if(element == "while")while_(((whilelooptypeParser<>*)child)->post());
  else if(element == "switch")switch_(((switchtypeParser*)child)->post());
  else if(element == "bloc")bloc(((bloctypeParser<>*)child)->post());
}

}

#endif
