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

#ifndef _INLINEPARSERS_HXX_
#define _INLINEPARSERS_HXX_

#include "parserBase.hxx"
#include "containerParsers.hxx"
#include "dataParsers.hxx"
#include "portParsers.hxx"
#include "codeParsers.hxx"
#include "propertyParsers.hxx"

#include "Proc.hxx"
#include "TypeCode.hxx"
#include "InlineNode.hxx"
#include "Exception.hxx"
#include "Runtime.hxx"
#include "Container.hxx"
#include "ComponentInstance.hxx"
#include "OutputDataStreamPort.hxx"
#include "InputDataStreamPort.hxx"
#include "ComponentInstance.hxx"
#include "factory.hxx"

extern YACS::ENGINE::Proc* currentProc;
extern YACS::ENGINE::Runtime* theRuntime;

namespace YACS
{

static std::string t1[]={"script","function",""};

template <class T=YACS::ENGINE::InlineNode*>
struct inlinetypeParser:public nodetypeParser<T>
{
  static inlinetypeParser<T> inlineParser;

  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child)
    {
      DEBTRACE( "inlinetypeParser::onEnd: " << el )             
      std::string element(el);
      if(element == "kind")kind(((stringtypeParser*)child)->post());
      else if(element == "script")script(((codetypeParser*)child)->post());
      else if(element == "function")function(((functypeParser*)child)->post());
      else if(element == "property")this->property(((propertytypeParser*)child)->post());
      else if(element == "inport") inport(((inporttypeParser<myinport>*)child)->post());
      else if(element == "outport") outport(((outporttypeParser<myoutport>*)child)->post());
    }
  virtual void buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      this->required("name",attr);
      for (int i = 0; attr[i]; i += 2) 
      {
        if(std::string(attr[i]) == "name")this->name(attr[i+1]);
        if(std::string(attr[i]) == "state")this->state(attr[i+1]);
      }
    }
  virtual void pre ()
    {
      this->_node=0;
      _kind="";
      this->_state="";
      this->_container="";
    }
  virtual void kind (const std::string& name)
    {
      DEBTRACE( "inline_kind " << name )             
      _kind=name;
    }

  virtual void script (const myfunc& f){}
  virtual void function (const myfunc& f) {}

  virtual void inport (const myinport& p)
    {
      DEBTRACE( "inline_inport: " << p._name <<":"<<p._type)             
      if(this->_node==0)
        throw YACS::Exception("Node must be completely defined before defining its ports");

      if(currentProc->typeMap.count(p._type)==0)
      {
        //Check if the typecode is defined in the runtime
        YACS::ENGINE::TypeCode* t=theRuntime->getTypeCode(p._type);
        if(t==0)
        {
          std::string msg="Unknown InPort Type: ";
          msg=msg+p._type+" for node: "+this->_node->getName()+" port name: "+p._name;
          throw YACS::Exception(msg);
        }
        else
        {
          currentProc->typeMap[p._type]=t;
          t->incrRef();
        }
      }
      this->_node->edAddInputPort(p._name,currentProc->typeMap[p._type]);
    }
  virtual void outport (const myoutport& p)
    {
      DEBTRACE( "inline_outport: " << p._name <<":"<<p._type)             
      if(this->_node==0)
        throw YACS::Exception("Node must be completely defined before defining its ports");

      if(currentProc->typeMap.count(p._type)==0)
      {
        YACS::ENGINE::TypeCode* t=theRuntime->getTypeCode(p._type);
        if(t==0)
        {
          std::string msg="Unknown OutPort Type: ";
          msg=msg+p._type+" for node: "+this->_node->getName()+" port name: "+p._name;
          throw YACS::Exception(msg);
        }
        else
        {
          currentProc->typeMap[p._type]=t;
          t->incrRef();
        }
      }
      this->_node->edAddOutputPort(p._name,currentProc->typeMap[p._type]);
    }
  virtual T post()
    {
      DEBTRACE( "inline_post " << this->_node->getName() )             
      if(this->_state == "disabled")this->_node->exDisabledState();
      /*
      std::list<OutputPort *>::iterator iter;
      std::list<OutputPort *> s=_node->getSetOfOutputPort();
      for(iter=s.begin();iter!=s.end();iter++)
        {
          std::cerr << "port name: " << (*iter)->getName() << std::endl;
          std::cerr << "port kind: " << (*iter)->edGetType()->kind() << std::endl;
        }
        */
      return this->_node;
    }
  std::string _kind;
};

template <class T> inlinetypeParser<T> inlinetypeParser<T>::inlineParser;

template <>
void inlinetypeParser<YACS::ENGINE::InlineNode*>::script (const myfunc& f)
{
  DEBTRACE( "inline_script: " << f._code )             
  _node=theRuntime->createScriptNode(_kind,_name);
  _node->setScript(f._code);
}

template <>
void inlinetypeParser<YACS::ENGINE::InlineNode*>::function (const myfunc& f)
{
  DEBTRACE( "inline_function: " << f._code )             
  YACS::ENGINE::InlineFuncNode *fnode;
  fnode=theRuntime->createFuncNode(_kind,_name);
  fnode->setScript(f._code);
  fnode->setFname(f._name);
  _node=fnode;
}

template <class T>
void inlinetypeParser<T>::onStart(const XML_Char* el, const XML_Char** attr)

    {
      DEBTRACE( "inlinetypeParser::onStart: " << el )
      std::string element(el);
      parser* pp=&parser::main_parser;
      this->maxcount("kind",1,element);
      this->maxcount("script",1,element);
      this->maxcount("function",1,element);
      this->maxchoice(t1,1,element);
      if(element == "kind")pp=&stringtypeParser::stringParser;
      else if(element == "script")pp=&codetypeParser::codeParser;
      else if(element == "function")pp=&functypeParser::funcParser;
      else if(element == "property")pp=&propertytypeParser::propertyParser;
      else if(element == "inport")pp=&inporttypeParser<>::inportParser;
      else if(element == "outport")pp=&outporttypeParser<>::outportParser;
      this->SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }

} // end of namespace YACS

#endif
