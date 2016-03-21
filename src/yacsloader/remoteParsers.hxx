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

#ifndef _REMOTEPARSERS_HXX_
#define _REMOTEPARSERS_HXX_

#include "inlineParsers.hxx"

extern YACS::ENGINE::Proc* currentProc;
extern YACS::ENGINE::Runtime* theRuntime;

namespace YACS
{

template <class T=YACS::ENGINE::InlineNode*>
struct remotetypeParser:public inlinetypeParser<T>
{
  static remotetypeParser<T> remoteParser;

  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child)
    {
      DEBTRACE( "remotetypeParser::onEnd: " << el )             
      std::string element(el);
      if(element == "kind")this->kind(((stringtypeParser*)child)->post()); // inherited
      else if(element == "function")this->function(((functypeParser*)child)->post());
      else if(element == "script")this->script(((codetypeParser*)child)->post());
      else if(element == "load") this->load(((loadtypeParser*)child)->post());
      else if(element == "property")this->property(((propertytypeParser*)child)->post());
      else if(element == "inport") this->inport(((inporttypeParser<myinport>*)child)->post());
      else if(element == "outport") this->outport(((outporttypeParser<myoutport>*)child)->post());
    }

  virtual void load (const loadon& l)
    {
      DEBTRACE( "remotenode_load: " << l._container);
      this->_container=l._container;
    }

  void function (const myfunc& f)
    {
      DEBTRACE( "remote_function: " << f._code )
      YACS::ENGINE::InlineFuncNode *fnode;
      fnode=theRuntime->createFuncNode(this->_kind,this->_name);
      fnode->setScript(f._code);
      fnode->setFname(f._name);
      fnode->setExecutionMode("remote");
      this->_node=fnode;
    }

  void script (const myfunc& f)
    {
      DEBTRACE( "remote_script: " << f._code )
      YACS::ENGINE::InlineNode *node;
      node=theRuntime->createScriptNode(this->_kind,this->_name);
      node->setScript(f._code);
      node->setExecutionMode("remote");
      this->_node=node;
    }

  virtual T post()
    {
      DEBTRACE( "remote_post " << this->_node->getName() )
      if(this->_state == "disabled")this->_node->exDisabledState();

      //set the container
      if(currentProc->containerMap.count(this->_container) != 0)
        {
          // a container with name (this->_container) exists. Use it
          this->_node->setContainer(currentProc->containerMap[this->_container]);
        }
      else if(this->_container == "" && currentProc->containerMap.count("DefaultContainer") != 0)
        {
          // a container with name (this->_container) does not exist
          //a default container is defined : use it
          this->_node->setContainer(currentProc->containerMap["DefaultContainer"]);
        }
      else
        std::cerr << "WARNING: Unknown container and no DefaultContainer " << this->_container << " will be ignored" << std::endl;

      return this->_node;
    }

};

template <class T>
void remotetypeParser<T>::onStart(const XML_Char* el, const XML_Char** attr)
{
  DEBTRACE( "remotetypeParser::onStart: " << el )
  std::string element(el);
  parser* pp=&parser::main_parser;
  this->maxcount("kind",1,element);
  this->maxcount("function",1,element);
  this->maxcount("script",1,element);
  this->maxcount("load",1,element);
  this->maxchoice(t1,1,element);

  if(element == "kind")pp=&stringtypeParser::stringParser;
  else if(element == "load")pp=&loadtypeParser::loadParser;
  else if(element == "function")pp=&functypeParser::funcParser;
  else if(element == "script")pp=&codetypeParser::codeParser;
  else if(element == "property")pp=&propertytypeParser::propertyParser;
  else if(element == "inport")pp=&inporttypeParser<>::inportParser;
  else if(element == "outport")pp=&outporttypeParser<>::outportParser;

  this->SetUserDataAndPush(pp);
  pp->init();
  pp->pre();
  pp->buildAttr(attr);
}

template <class T> remotetypeParser<T> remotetypeParser<T>::remoteParser; // instanciate static class attribute

} // end of namespace YACS

#endif
