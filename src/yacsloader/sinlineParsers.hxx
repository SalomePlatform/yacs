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

#ifndef _SINLINEPARSERS_HXX_
#define _SINLINEPARSERS_HXX_

#include "Proc.hxx"
#include "ServiceInlineNode.hxx"
#include "Runtime.hxx"

extern YACS::ENGINE::Proc* currentProc;
extern YACS::ENGINE::Runtime* theRuntime;

namespace YACS
{

/*! \brief Class for parsing ServiceInlineNode description
 *
 *  
 */
template <class T=YACS::ENGINE::ServiceInlineNode*>
struct sinlinetypeParser:public inlinetypeParser<T>
{
  static sinlinetypeParser<T> sinlineParser;

  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child)
    {
      DEBTRACE( "sinlinetypeParser::onEnd: " << el )             
      std::string element(el);
      if(element == "kind")this->kind(((stringtypeParser*)child)->post());
      else if(element == "function")this->function(((functypeParser*)child)->post());
      else if(element == "load") load(((loadtypeParser*)child)->post());
      else if(element == "property")this->property(((propertytypeParser*)child)->post());
      else if(element == "inport") this->inport(((inporttypeParser<myinport>*)child)->post());
      else if(element == "outport") this->outport(((outporttypeParser<myoutport>*)child)->post());
    }
  //virtual void service (const myfunc& f) {}
  virtual void load (const loadon& l)
    {
      DEBTRACE( "sinline_load: " )             
      this->_container=l._container;
    }
  virtual T post()
    {
      DEBTRACE( "sinline_post " << this->_node->getName() );
      if(this->_state == "disabled")this->_node->exDisabledState();

      if(!this->_node->getComponent())
        throw YACS::Exception("A service inline node must have a component instance");

      // If the component instance has already a container don't go further
      if(this->_node->getComponent()->getContainer())
        return this->_node;

      // Only for anonymous component instance set a container
      if(this->_node->getComponent()->isAnonymous())
        {
          if(currentProc->containerMap.count(this->_container) != 0)
            this->_node->getComponent()->setContainer(currentProc->containerMap[this->_container]);
          else if(this->_container == "" && currentProc->containerMap.count("DefaultContainer") != 0)
            {
              //a default container is defined : use it if supported
              try
                {
                  currentProc->containerMap["DefaultContainer"]->checkCapabilityToDealWith(this->_node->getComponent());
                  this->_node->getComponent()->setContainer(currentProc->containerMap["DefaultContainer"]);
                }
              catch(YACS::Exception){}
            }
          else
            std::cerr << "WARNING: Unknown container " << this->_container << " ignored" << std::endl;
        }

      return this->_node;
    }
};
template <class T> sinlinetypeParser<T> sinlinetypeParser<T>::sinlineParser;

// sinline ????
template <>
void inlinetypeParser<YACS::ENGINE::ServiceInlineNode*>::function (const myfunc& f)
{
  DEBTRACE( "sinline_function: " << f._code )             
  YACS::ENGINE::ServiceInlineNode *fnode;
  fnode=theRuntime->createSInlineNode(_kind,_name);
  fnode->setScript(f._code);
  fnode->setMethod(f._name);
  // TODO: update with currentProc->createComponentInstance() method
  fnode->setComponent(theRuntime->createComponentInstance("PyCompo","SalomePy"));
  _node=fnode;
}

template <class T>
void sinlinetypeParser<T>::onStart(const XML_Char* el, const XML_Char** attr)
    {
      DEBTRACE( "sinlinetypeParser::onStart: " << el )
      std::string element(el);
      parser* pp=&parser::main_parser;
      this->maxcount("kind",1,element);
      this->maxcount("function",1,element);
      this->maxcount("load",1,element);
      if(element == "kind")pp=&stringtypeParser::stringParser;
      else if(element == "function")pp=&functypeParser::funcParser;
      else if(element == "load")pp=&loadtypeParser::loadParser;
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
