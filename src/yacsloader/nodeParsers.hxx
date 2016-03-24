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

#ifndef _NODEPARSERS_HXX_
#define _NODEPARSERS_HXX_

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

/*! \brief Class for node parser.
 *
 *  This class is a base class for other parsers
 */
template <class T=YACS::ENGINE::InlineNode*>
struct nodetypeParser: parser
{
  static nodetypeParser<T> nodeParser;

  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      DEBTRACE( "nodetypeParser::onStart: " << el )             
      std::string element(el);
      parser* pp=&parser::main_parser;
      this->SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }
  virtual void onEnd(const char *el,parser* child)
    {
      DEBTRACE( "nodetypeParser::onEnd: " << el )             
      std::string element(el);
    }
  virtual void buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      required("name",attr);
      required("type",attr);
      for (int i = 0; attr[i]; i += 2) 
      {
        if(std::string(attr[i]) == "name")name(attr[i+1]);
        if(std::string(attr[i]) == "state")state(attr[i+1]);
        if(std::string(attr[i]) == "type")type(attr[i+1]);
      }
    }
  virtual void pre()
    {
      _node=0;
      _container="";
    }
  virtual void name (const std::string& name)
    {
      DEBTRACE( "inline_name: " << name )             
      _name=name;
    }
  virtual void state (const std::string& name)
    {
      _state=name;
    }
  virtual void type (const std::string& name)
    {
      DEBTRACE( "node_type " << name )             
      _type=name;
    }
  virtual void property (const myprop& prop);
  virtual T post();
  std::string _type;
  std::string _name;
  std::string _state;
  std::string _container;
  T _node;
};

template <class T> nodetypeParser<T> nodetypeParser<T>::nodeParser;

template <class T>
void nodetypeParser<T>::property (const myprop& prop)
{
   if(this->_node==0)
     throw YACS::Exception("Node must be completely defined before setting its properties");
  _node->setProperty(prop._name,prop._value);
}

template <class T>
T nodetypeParser<T>::post()
{
  return _node;
}

template <>
YACS::ENGINE::InlineNode* nodetypeParser<YACS::ENGINE::InlineNode*>::post ()
{
  std::string fullname = currentProc->names.back()+_type;
  if(currentProc->inlineMap.count(_type) != 0)
    {
      //InlineNode type with absolute name found
      YACS::ENGINE::InlineNode* n=currentProc->inlineMap[_type];
      _node=n->cloneNode(_name);
    }
  else if(currentProc->inlineMap.count(fullname) != 0)
    {
      //InlineNode type with relative name found
      YACS::ENGINE::InlineNode* n=currentProc->inlineMap[fullname];
      _node=n->cloneNode(_name);
    }
  else
    {
      throw YACS::Exception("Unknown InlineNode type");
    }
  if(_state == "disabled")_node->exDisabledState();
  DEBTRACE( "node_post " << _node->getName() )             
  return _node;
}

} // end of namespace YACS

#endif
