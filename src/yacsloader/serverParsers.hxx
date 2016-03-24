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

#ifndef _SERVERPARSERS_HXX_
#define _SERVERPARSERS_HXX_

#include "ServerNode.hxx"

extern YACS::ENGINE::Proc* currentProc;
extern YACS::ENGINE::Runtime* theRuntime;

namespace YACS
{

static std::string t22[]={"ref","node","component","componentinstance",""};

template <class T=YACS::ENGINE::ServerNode*>
struct servertypeParser:public inlinetypeParser<T>
{
  static servertypeParser<T> serverParser;

  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child)
    {
      DEBTRACE( "servertypeParser::onEnd: " << el )             
      std::string element(el);
      if(element == "kind")this->kind(((stringtypeParser*)child)->post());
      else if(element == "loadcontainer") loadcontainer(((stringtypeParser*)child)->post());
      //else if(element == "loadcontainerinstance") containerinstance(((stringtypeParser*)child)->post());
      else if(element == "script") script(((codetypeParser*)child)->post());
      else if(element == "node") node(((stringtypeParser*)child)->post());
      else if(element == "method") method(((stringtypeParser*)child)->post());
      else if(element == "load") load(((loadtypeParser*)child)->post());
      else if(element == "property")this->property(((propertytypeParser*)child)->post());
      else if(element == "inport") this->inport(((inporttypeParser<myinport>*)child)->post());
      else if(element == "outport") this->outport(((outporttypeParser<myoutport>*)child)->post());
      else if(element == "instream") instream(((inporttypeParser<myinport>*)child)->post());
      else if(element == "outstream") outstream(((outporttypeParser<myoutport>*)child)->post());
    }

  /*virtual void containerinstance (const std::string& name)
    {
      DEBTRACE( "componentinstance: " << name )             
      if(currentProc->componentInstanceMap.count(name) == 0)
        throw YACS::Exception("Unknown ComponentInstance: "+name);
      this->_node=theRuntime->createCompoNode(this->_kind,this->_name);
      YACS::ENGINE::ComponentInstance* inst=currentProc->componentInstanceMap[name];
      this->_node->setComponent(inst);
      }*/

  virtual void loadcontainer (const std::string& name)
    {
      DEBTRACE( "server_loadcontainer: " << name )             
      this->_node=(YACS::ENGINE::ServerNode*)theRuntime->createFuncNode("DistPython",this->_name);
      std::map<std::string,YACS::ENGINE::Container *>::const_iterator it(currentProc->containerMap.find(name));
      if(it!=currentProc->containerMap.end())
        {
          this->_node->setContainer((*it).second);
        }
      else
        {
          YACS::ENGINE::Container *cont=currentProc->createContainer(this->_node->getEffectiveKindOfServer());
          cont->setName(name);
          this->_node->setContainer(cont);
          cont->decrRef();
        }
    }
  virtual void script (const myfunc& f)
  {
    DEBTRACE( "server_script: " << f._code )
    this->_node->setScript(f._code);
  }
  virtual void node (const std::string& name)
    {
      DEBTRACE( "server_node: " << name )             
      std::string fullname = currentProc->names.back()+name;
      if(currentProc->inlineMap.count(name) != 0)
        {
          //ServerNode with absolute name found
          YACS::ENGINE::ServerNode* n=(YACS::ENGINE::ServerNode*)currentProc->inlineMap[name];
          this->_node =n->createNode(this->_name);
        }
      else if(currentProc->inlineMap.count(fullname) != 0)
        {
          //ServerNode with relative name found
          //TODO: must be a short name (possible only in the same context)
          YACS::ENGINE::ServerNode* n=(YACS::ENGINE::ServerNode*)currentProc->inlineMap[fullname];
          this->_node =n->createNode(this->_name);
        }
      else
        {
          throw YACS::Exception("Unknown ServerNode");
        }
    }

  virtual void method (const std::string& name)
    {
      DEBTRACE( "server_method: " << name )             
      if(this->_node==0)
        throw YACS::Exception("ServerNode must be completely defined before defining its method");
      if(name == "")
      {
        this->logError("a server name must be a non empty string");
        return;
      }
      this->_node->setFname(name);
    }

  virtual void load (const loadon& l)
    {
      DEBTRACE( "server_load: " << l._container);
      this->_container=l._container;
    }

  virtual void instream (const myinport& p)
    {
      DEBTRACE( "server_instream" )             
      DEBTRACE( p._type )             
      DEBTRACE( p._name )             
      if(this->_node==0)
        throw YACS::Exception("ServerNode must be completely defined before defining its ports");

      if(currentProc->typeMap.count(p._type)==0)
      {
        YACS::ENGINE::TypeCode* t=theRuntime->getTypeCode(p._type);
        if(t==0)
        {
          std::string msg="Unknown InStreamPort Type: ";
          msg=msg+p._type+" for node: "+this->_node->getName()+" port name: "+p._name;
          throw YACS::Exception(msg);
        }
        else
        {
          currentProc->typeMap[p._type]=t;
          t->incrRef();
        }
      }
      YACS::ENGINE::InputDataStreamPort* port;
      port=this->_node->edAddInputDataStreamPort(p._name,currentProc->typeMap[p._type]);
      // Set all properties for this port
      std::map<std::string, std::string>::const_iterator pt;
      for(pt=p._props.begin();pt!=p._props.end();pt++)
        port->setProperty((*pt).first,(*pt).second);
    }
  virtual void outstream (const myoutport& p)
    {
      DEBTRACE( "server_outstream" )             
      DEBTRACE( p._type )             
      DEBTRACE( p._name )             
      if(this->_node==0)
        throw YACS::Exception("ServerNode must be completely defined before defining its ports");

      if(currentProc->typeMap.count(p._type)==0)
      {
        YACS::ENGINE::TypeCode* t=theRuntime->getTypeCode(p._type);
        if(t==0)
        {
          std::string msg="Unknown OutStreamPort Type: ";
          msg=msg+p._type+" for node: "+this->_node->getName()+" port name: "+p._name;
          throw YACS::Exception(msg);
        }
        else
        {
          currentProc->typeMap[p._type]=t;
          t->incrRef();
        }
      }
      YACS::ENGINE::OutputDataStreamPort* port;
      port=this->_node->edAddOutputDataStreamPort(p._name,currentProc->typeMap[p._type]);
      // Set all properties for this port
      std::map<std::string, std::string>::const_iterator pt;
      for(pt=p._props.begin();pt!=p._props.end();pt++)
        port->setProperty((*pt).first,(*pt).second);
    }
  virtual T post()
    {
      DEBTRACE( "server_post " << this->_node->getName() )             
      this->mincount("method",1);
      if(this->_state == "disabled")this->_node->exDisabledState();

      // If the server node has no component instance don't go further return the node
      if(!this->_node->getComponent())
        return this->_node;
      
      // when container is not defined by <load container="xxx"/> but indirectly by <node>xxx</node>
      // this->_container is not set, and there is no need to setContainer 
      // so stop here and return the node
      if(this->_node->getComponent()->getContainer())
        return this->_node;

      //If the component instance is anonymous set the container
      // with the load directive or with the DefaultContainer
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
              catch(YACS::Exception)
                {}
            }
          else
            std::cerr << "WARNING: Unknown container " << this->_container << " ignored" << std::endl;
        }
      return this->_node;
    }
};

template <class T> servertypeParser<T> servertypeParser<T>::serverParser;


template <class T>
void servertypeParser<T>::onStart(const XML_Char* el, const XML_Char** attr)
    {
      DEBTRACE( "servertypeParser::onStart: " << el )
      std::string element(el);
      parser* pp=&parser::main_parser;
      this->maxcount("kind",1,element);
      this->maxcount("ref",1,element);
      this->maxcount("node",1,element);
      this->maxcount("component",1,element);
      this->maxcount("componentinstance",1,element);
      this->maxcount("loadcontainer",1,element);
      this->maxcount("script",1,element);
      this->maxcount("method",1,element);
      this->maxcount("load",1,element);
      this->maxchoice(t22,1,element);
      if(element == "kind")pp=&stringtypeParser::stringParser;
      else if(element == "ref")pp=&stringtypeParser::stringParser;
      else if(element == "component")pp=&stringtypeParser::stringParser;
      else if(element == "componentinstance")pp=&stringtypeParser::stringParser;
      else if(element == "loadcontainer")pp=&stringtypeParser::stringParser;
      else if(element == "script")pp=&codetypeParser::codeParser;
      else if(element == "node")pp=&stringtypeParser::stringParser;
      else if(element == "method")pp=&stringtypeParser::stringParser;
      else if(element == "load")pp=&loadtypeParser::loadParser;
      else if(element == "property")pp=&propertytypeParser::propertyParser;
      else if(element == "inport")pp=&inporttypeParser<>::inportParser;
      else if(element == "outport")pp=&outporttypeParser<>::outportParser;
      else if(element == "instream")pp=&inporttypeParser<>::inportParser;
      else if(element == "outstream")pp=&outporttypeParser<>::outportParser;
      this->SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }

} // end of namespace YACS

#endif
