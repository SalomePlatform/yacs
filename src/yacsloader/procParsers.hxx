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

#ifndef _PROCPARSER_HXX_
#define _PROCPARSER_HXX_

#include "blocParsers.hxx"
#include "typeParsers.hxx"
#include "containerParsers.hxx"
#include "componentinstanceParsers.hxx"
#include "nodeParsers.hxx"

#include "Proc.hxx"
#include "Container.hxx"
#include "TypeCode.hxx"

extern YACS::ENGINE::Proc* currentProc;

namespace YACS
{

template <class T=YACS::ENGINE::Proc*>
struct proctypeParser: bloctypeParser<T>
{

  static proctypeParser<T> procParser;

  proctypeParser():bloctypeParser<T>()
  {
    this->_orders["type"]=1;
    this->_orders["sequence"]=1;
    this->_orders["objref"]=1;
  }
  void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child);
  virtual void buildAttr(const XML_Char** attr)
    {
	  if (!attr)
		return;
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "state")
            this->state(attr[i+1]);
          if(std::string(attr[i]) == "name")
            name(attr[i+1]);
        }
    }
  virtual void pre()
    {
        std::string name("proc");
        currentProc=theRuntime->createProc(name);
        this->_bloc=currentProc;
        currentProc->names.push_back("");
    }

  virtual void name(const std::string& name)
    {
      currentProc->setName(name);
    }

  virtual void type(const mytype& t)
    {
        DEBTRACE( "type_set" );
        YACS::ENGINE::TypeCode* tt=currentProc->createType(t._name,t._kind);
        tt->decrRef();
    }
  virtual void sequence(ENGINE::TypeCode* const& t)
    {
        DEBTRACE( "sequence_set" );
        t->decrRef();
    }
  virtual void objref(ENGINE::TypeCode* const& t)
    {
        DEBTRACE( "objref_set" );
        t->decrRef();
    }
  virtual void struct_(ENGINE::TypeCode* const& t)
    {
        DEBTRACE( "struct_set" );
        t->decrRef();
    }

  virtual void componentinstance(const mycomponentinstance& t)
    {
      DEBTRACE( "componentinstance: " << t._name );
      YACS::ENGINE::ComponentInstance* inst=currentProc->createComponentInstance(t._component,t._name,t._kind);

      // Set all properties for this component instance
      std::map<std::string, std::string>::const_iterator pt;
      for(pt=t._props.begin();pt!=t._props.end();pt++)
        inst->setProperty((*pt).first,(*pt).second);

      //associate a container to the component instance
      if(currentProc->containerMap.count(t._container) != 0)
        {
          inst->setContainer(currentProc->containerMap[t._container]);
        }
      else if(t._container == "")
        {
          if(currentProc->containerMap.count("DefaultContainer") != 0)
          {
            //a default container is defined : use it if supported
            try
            {
              currentProc->containerMap["DefaultContainer"]->checkCapabilityToDealWith(inst);
              inst->setContainer(currentProc->containerMap["DefaultContainer"]);
            }
            catch(YACS::Exception){}
          }
        }
      else
        {
          std::cerr << "WARNING: Unknown container " << t._container << " ignored" << std::endl;
        }

      inst->decrRef();
    }

  virtual void container(const mycontainer& t)
    {
      DEBTRACE( "container_set: " << t._name )             
      std::vector<machine>::const_iterator iter;
      for(iter=t._machs.begin();iter!=t._machs.end();iter++)
        {
          DEBTRACE( "machine name: " << (*iter)._name )             
        }

      if(currentProc->containerMap.count(t._name) != 0 && t._name != "DefaultContainer")
        {
          std::cerr << "Warning: container " << t._name << " already defined. It will be ignored" << std::endl;
        }
      else
        {
          // Set all properties for this container
          std::string kindOfContainer;
          std::map<std::string, std::string>::const_iterator pt(t._props.find(std::string(ENGINE::Container::KIND_ENTRY)));
          if(pt!=t._props.end())
            kindOfContainer=pt->second;
          YACS::ENGINE::Container *cont(currentProc->createContainer(t._name,kindOfContainer));
          for(pt=t._props.begin();pt!=t._props.end();pt++)
            if((*pt).second!=ENGINE::Container::KIND_ENTRY)
              cont->setProperty((*pt).first,(*pt).second);
          cont->decrRef();
        }
    }

  T post() { return this->_bloc; }
};

template <class T> proctypeParser<T> proctypeParser<T>::procParser;

}

namespace YACS
{
template <class T>
void proctypeParser<T>::onStart(const XML_Char* el, const XML_Char** attr)
  {
    DEBTRACE( "proctypeParser::onStart: " << el )
    std::string element(el);
    this->checkOrder(element);
    parser* pp=&parser::main_parser;
    if(element == "property")pp=&propertytypeParser::propertyParser;
    else if(element == "type")pp=&typetypeParser::typeParser;
    else if(element == "sequence")pp=&seqtypeParser::seqParser;
    else if(element == "objref")pp=&objtypeParser::objParser;
    else if(element == "struct")pp=&structtypeParser::structParser;
    else if(element == "container")pp=&containertypeParser::containerParser;
    else if(element == "componentinstance")pp=&componentinstancetypeParser::componentinstanceParser;

    else if(element == "inline")pp=&inlinetypeParser<>::inlineParser;
    else if(element == "sinline")pp=&sinlinetypeParser<>::sinlineParser;
    else if(element == "service")pp=&servicetypeParser<>::serviceParser;
    else if(element == "server")pp=&servertypeParser<>::serverParser;
    else if(element == "remote")pp=&remotetypeParser<>::remoteParser;
    else if(element == "node")pp=&nodetypeParser<>::nodeParser;
    else if(element == "datanode")pp=&presettypeParser<>::presetParser;
    else if(element == "outnode")pp=&outnodetypeParser<>::outnodeParser;

    else if(element == "bloc")pp=&bloctypeParser<>::blocParser;
    else if(element == "forloop")pp=&forlooptypeParser<>::forloopParser;
    else if(element == "foreach")pp=&foreachlooptypeParser<>::foreachloopParser;
    else if(element == "optimizer")pp=&optimizerlooptypeParser<>::optimizerloopParser;
    else if(element == "while")pp=&whilelooptypeParser<>::whileloopParser;
    else if(element == "switch")pp=&switchtypeParser::switchParser;

    else if(element == "control")pp=&controltypeParser<>::controlParser;
    else if(element == "datalink")pp=&linktypeParser<>::linkParser;
    else if(element == "stream")pp=&streamtypeParser<>::streamParser;
    else if(element == "parameter")pp=&parametertypeParser::parameterParser;
    else
      {
        // OCC: san -- Allow external parsers for handling of unknown elements
        // and attributes. This capability is used by YACS GUI to read
        // graph presentation data
        if ( this->_defaultParsersMap )
          {
            if((this->_defaultParsersMap)->count(element) != 0)
              {
                pp=(*(this->_defaultParsersMap))[element];
              }
            else
              {
                std::cerr << "There is no parser for this element type. It will be ignored! " << element << std::endl;
              }
          }
      }
    this->SetUserDataAndPush(pp);
    pp->init();
    pp->pre();
    pp->buildAttr(attr);
  }

template <class T>
void proctypeParser<T>::onEnd(const char *el,parser* child)
    {
      DEBTRACE( "proctypeParser::onEnd: " << el )
      std::string element(el);
      if(element == "property")this->property(((propertytypeParser*)child)->post());
      else if(element == "type")type(((typetypeParser*)child)->post());
      else if(element == "sequence")sequence(((seqtypeParser*)child)->post());
      else if(element == "objref")objref(((objtypeParser*)child)->post());
      else if(element == "struct")struct_(((structtypeParser*)child)->post());
      else if(element == "container")container(((containertypeParser*)child)->post());
      else if(element == "componentinstance")componentinstance(((componentinstancetypeParser*)child)->post());

      else if(element == "inline")this->inline_(((inlinetypeParser<>*)child)->post());
      else if(element == "sinline")this->sinline(((sinlinetypeParser<>*)child)->post());
      else if(element == "service")this->service(((servicetypeParser<>*)child)->post());
      else if(element == "server")this->server(((servertypeParser<>*)child)->post());
      else if(element == "remote")this->remote(((remotetypeParser<>*)child)->post());
      else if(element == "node")this->node(((nodetypeParser<>*)child)->post());
      else if(element == "datanode")this->preset(((presettypeParser<>*)child)->post());
      else if(element == "outnode")this->outnode(((outnodetypeParser<>*)child)->post());

      else if(element == "bloc")this->bloc(((bloctypeParser<>*)child)->post());
      else if(element == "forloop")this->forloop(((forlooptypeParser<>*)child)->post());
      else if(element == "foreach")this->foreach(((foreachlooptypeParser<>*)child)->post());
      else if(element == "optimizer")this->optimizer(((optimizerlooptypeParser<>*)child)->post());
      else if(element == "while")this->while_(((whilelooptypeParser<>*)child)->post());
      else if(element == "switch")this->switch_(((switchtypeParser*)child)->post());
 
      else if(element == "control") this->control(((controltypeParser<>*)child)->post());
      else if(element == "datalink") this->datalink(((linktypeParser<>*)child)->post());
      else if(element == "stream") this->stream(((streamtypeParser<>*)child)->post());
      else if(element == "parameter") this->parameter(((parametertypeParser*)child)->post());
    }

}

#endif
