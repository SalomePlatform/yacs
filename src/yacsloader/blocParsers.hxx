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

#ifndef _BLOCPARSER_HXX_
#define _BLOCPARSER_HXX_

#include "parserBase.hxx"
#include "linkParsers.hxx"
#include "xmlrpcParsers.hxx"
#include "nodeParsers.hxx"
#include "inlineParsers.hxx"
#include "remoteParsers.hxx"
#include "serverParsers.hxx"
#include "sinlineParsers.hxx"
#include "serviceParsers.hxx"


#include "Proc.hxx"
#include "Logger.hxx"
#include "Runtime.hxx"
#include "ForLoop.hxx"
#include "ForEachLoop.hxx"
#include "OptimizerLoop.hxx"
#include "WhileLoop.hxx"
#include "Switch.hxx"
#include "Bloc.hxx"
#include "Exception.hxx"
#include "InlineNode.hxx"
#include "DataNode.hxx"
#include "ServiceNode.hxx"
#include "ServiceInlineNode.hxx"
#include "OutputDataStreamPort.hxx"
#include "InputDataStreamPort.hxx"

#include <vector>

extern YACS::ENGINE::Proc* currentProc;
extern YACS::ENGINE::Runtime* theRuntime;

namespace YACS
{

template <class T=YACS::ENGINE::Bloc*>
struct bloctypeParser:parser
{
  static bloctypeParser<T> blocParser;

  bloctypeParser():parser()
  {
    _orders["property"]=0;
    _orders["inline"]=2;
    _orders["service"]=2;
    _orders["server"]=2;
    _orders["remote"]=2;
    _orders["sinline"]=2;
    _orders["node"]=2;
    _orders["datanode"]=2;
    _orders["outnode"]=2;
    _orders["forloop"]=2;
    _orders["foreach"]=2;
    _orders["optimizer"]=2;
    _orders["while"]=2;
    _orders["switch"]=2;
    _orders["bloc"]=2;
    _orders["control"]=3;
    _orders["datalink"]=3;
    _orders["stream"]=3;
    _orders["parameter"]=3;
  }
  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child);
  virtual void buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      this->required("name",attr);
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
          if(std::string(attr[i]) == "state")state(attr[i+1]);
        }
    }
  void name (const std::string& name)
    {
    }
  virtual void state (const std::string& name){
      DEBTRACE( "bloc_state: " << name )             
      _state=name;
      if(_state == "disabled")
        {
          DEBTRACE( "Bloc disabled: " << _bloc->getName())             
          _bloc->exDisabledState();
        }
    }
  virtual void property (const myprop& prop)
    {
      DEBTRACE( "property_set: " << prop._name << prop._value )             
      _bloc->setProperty(prop._name,prop._value);
    }
  virtual void inline_ (YACS::ENGINE::InlineNode* const& n)
    {
      DEBTRACE( "bloc_pynode_set: " << n->getName() )             
      _bloc->edAddChild(n);
      std::string fullname = currentProc->names.back()+n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->inlineMap[fullname]=n;
    }
  virtual void sinline (YACS::ENGINE::ServiceInlineNode* const& n)
    {
      DEBTRACE( "bloc_sinline: " << n->getName() )             
      _bloc->edAddChild(n);
      std::string fullname = currentProc->names.back()+n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->serviceMap[fullname]=n;
    }
  virtual void service (YACS::ENGINE::ServiceNode* const& n)
    {
      DEBTRACE( "bloc_service_set: " << n->getName() )             
      _bloc->edAddChild(n);
      std::string fullname = currentProc->names.back()+n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->serviceMap[fullname]=n;
    }
  virtual void remote (YACS::ENGINE::InlineNode* const& n)
    {
      DEBTRACE( "bloc_remote_set: " << n->getName() )             
      _bloc->edAddChild(n);
      std::string fullname = currentProc->names.back()+n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->inlineMap[fullname]=n;
    }
  virtual void server (YACS::ENGINE::ServerNode* const& n)
    {
      DEBTRACE( "bloc_server_set: " << n->getName() )             
      _bloc->edAddChild(n);
      std::string fullname = currentProc->names.back()+n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->inlineMap[fullname]=n;
    }
  virtual void node (YACS::ENGINE::InlineNode* const& n)
    {
      DEBTRACE( "bloc_node_set: " << n->getName() )             
      _bloc->edAddChild(n);
      std::string fullname = currentProc->names.back()+n->getName();
      DEBTRACE( "bloc_node_set fullname = "  << fullname )             
      currentProc->nodeMap[fullname]=n;
      currentProc->inlineMap[fullname]=n;
    }
  virtual void forloop (YACS::ENGINE::ForLoop* const& b)
    {
      DEBTRACE( "bloc_forloop_set: " << b->getName() )             
      _bloc->edAddChild(b);
      std::string fullname = currentProc->names.back()+b->getName();
      currentProc->nodeMap[fullname]=b;
    }
  virtual void optimizer (YACS::ENGINE::OptimizerLoop* const& b)
    {
      DEBTRACE( "bloc_optimizer_set: " << b->getName() );
      _bloc->edAddChild(b);
      std::string fullname = currentProc->names.back()+b->getName();
      currentProc->nodeMap[fullname]=b;
      //fullname += ".splitter";
      //currentProc->nodeMap[fullname]=b->getChildByShortName("splitter");
    }
  virtual void foreach (YACS::ENGINE::ForEachLoop* const& b)
    {
      DEBTRACE( "bloc_foreach_set: " << b->getName() )             
      _bloc->edAddChild(b);
      std::string fullname = currentProc->names.back()+b->getName();
      currentProc->nodeMap[fullname]=b;
      fullname += ".splitter";
      currentProc->nodeMap[fullname]=b->getChildByShortName("splitter");
    }
  virtual void while_ (YACS::ENGINE::WhileLoop* const& b)
    {
      DEBTRACE( "bloc_while_set: " << b->getName() )             
      _bloc->edAddChild(b);
      std::string fullname = currentProc->names.back()+b->getName();
      currentProc->nodeMap[fullname]=b;
    }
  virtual void switch_ (YACS::ENGINE::Switch* const& b)
    {
      DEBTRACE( "bloc_switch_set: " << b->getName() )             
      _bloc->edAddChild(b);
      std::string fullname = currentProc->names.back()+b->getName();
      currentProc->nodeMap[fullname]=b;
    }
  virtual void bloc (YACS::ENGINE::Bloc* const& b)
    {
      DEBTRACE( "bloc_bloc_set: " << b->getName() )             
      _bloc->edAddChild(b);
      std::string fullname=currentProc->names.back()+ b->getName();
      currentProc->nodeMap[fullname]=b;
    }
  virtual void control (const mycontrol& l)
    {
      DEBTRACE( "bloc_control_set: " << l.fromnode() << " "<< l.tonode() )             
      std::string msg;

      if(currentProc->nodeMap.count(currentProc->names.back()+l.fromnode()) == 0)
        {
          msg="from node " + l.fromnode() + " does not exist in control link: ";
          msg=msg+l.fromnode()+"->"+l.tonode();
          msg=msg+ " context: "+currentProc->names.back();
          this->logError(msg);
          return;
        }
      if(currentProc->nodeMap.count(currentProc->names.back()+l.tonode()) == 0)
        {
          msg="to node " + l.tonode() + " does not exist in control link: ";
          msg=msg+l.fromnode()+"->"+l.tonode();
          msg=msg+ " context: "+currentProc->names.back();
          this->logError(msg);
          return;
        }
      // We only link local nodes
      try
        {
          _bloc->edAddCFLink(currentProc->nodeMap[currentProc->names.back()+l.fromnode()],
                             currentProc->nodeMap[currentProc->names.back()+l.tonode()]);
        }
      catch(YACS::Exception& e)
        {
          this->logError(e.what());
        }
    }
  virtual void datalink (const mylink& l)
    {
      DEBTRACE( "bloc_datalink_set: "<<l.fromnode()<<"("<<l.fromport()<<")->"<<l.tonode()<<"("<<l.toport()<<")")
      std::string msg;

      //Try only relative name for from node
      std::string fromname = currentProc->names.back()+l.fromnode();
      if(currentProc->nodeMap.count(fromname) == 0)
        {
          msg="from node " + l.fromnode() + " does not exist in data link: ";
          msg=msg+l.fromnode()+"("+l.fromport()+")->"+l.tonode()+"("+l.toport()+")";
          this->logError(msg);
          return;
        }
      //Try relative name for to node and then absolute one
      std::string toname = currentProc->names.back()+l.tonode();
      if(currentProc->nodeMap.count(toname) == 0)
        {
          //It's not a relative name. Try an absolute one (I think it's not possible)
          toname=l.tonode();
          if(currentProc->nodeMap.count(toname) == 0)
            {
              // The TO node does not exist -> error
              msg="to node " + l.tonode() + " does not exist in data link: ";
              msg=msg+l.fromnode()+"("+l.fromport()+")->"+l.tonode()+"("+l.toport()+")";
              this->logError(msg);
              return;
            }
        }
      // We only link local node and other nodes (relative or absolute name in this order)
      DEBTRACE(fromname <<":"<<l.fromport()<<toname<<":"<<l.toport());
      try
        {
          if (l.withControl())
            _bloc->edAddDFLink(currentProc->nodeMap[fromname]->getOutputPort(l.fromport()),
                               currentProc->nodeMap[toname]->getInputPort(l.toport()));
          else
            _bloc->edAddLink(currentProc->nodeMap[fromname]->getOutputPort(l.fromport()),
                             currentProc->nodeMap[toname]->getInputPort(l.toport()));
        }
      catch(YACS::Exception& e)
        {
          this->logError(e.what());
        }
    }
  virtual void stream (const mystream& l)
    {
      DEBTRACE( "bloc_stream_set: " << l.fromnode() << l.fromport() << l.tonode() << l.toport() )
      std::string msg;
      std::string fromname = currentProc->names.back()+l.fromnode();
      std::string toname = currentProc->names.back()+l.tonode();
      //only relative names
      if(currentProc->nodeMap.count(fromname) == 0)
        {
            msg="from node " + l.fromnode() + " does not exist in stream link: ";
            msg=msg+l.fromnode()+"("+l.fromport()+")->"+l.tonode()+"("+l.toport()+")";
            this->logError(msg);
            return;
        }
      if(currentProc->nodeMap.count(toname) == 0)
        {
            msg="to node " + l.tonode() + " does not exist in stream link: ";
            msg=msg+l.fromnode()+"("+l.fromport()+")->"+l.tonode()+"("+l.toport()+")";
            this->logError(msg);
            return;
        }
      YACS::ENGINE::OutputDataStreamPort* pout=currentProc->nodeMap[fromname]->getOutputDataStreamPort(l.fromport());
      YACS::ENGINE::InputDataStreamPort* pin=currentProc->nodeMap[toname]->getInputDataStreamPort(l.toport());
      _bloc->edAddLink(pout,pin);
      // Set all properties for this link
      std::map<std::string, std::string>::const_iterator pt;
      for(pt=l._props.begin();pt!=l._props.end();pt++)
        {
          pin->setProperty((*pt).first,(*pt).second);
          pout->setProperty((*pt).first,(*pt).second);
        }
    }
  virtual void parameter (const myparam& param)
    {
      DEBTRACE( "++++++++++++++++++++Parameter+++++++++++++++++++++" );
      std::string msg;
      std::string toname = currentProc->names.back()+param._tonode;
      if(currentProc->nodeMap.count(toname) == 0)
        {
          msg="to node " + param._tonode + " does not exist in parameter: ";
          msg=msg+"->"+param._tonode+"("+param._toport+")";
          this->logError(msg);
          return;
        }
      YACS::ENGINE::InputPort* inport=currentProc->nodeMap[toname]->getInputPort(param._toport);
      //We don't know the parameter type. So we try to initialize the port
      //with the value. If it's not the right type, edInit throws an exception
      try
        {
          inport->edInit("XML",param._value.c_str());
        }
      catch(YACS::Exception& e)
        {
          this->logError(e.what());
        }
      DEBTRACE( "++++++++++++++++++++End parameter+++++++++++++++++++++" );
    }

  virtual void preset (ENGINE::DataNode* const& n);
  virtual void outnode (ENGINE::DataNode* const& n);

  T post()
    {
      DEBTRACE( "bloc_post" )             
      currentProc->names.pop_back();
      T b=_bloc;
      _blocs.pop_back();
      if(_blocs.empty())
        _bloc=NULL;
      else
        _bloc=_blocs.back();
      return b;
    }

  T _bloc;
  std::string _state;
  std::vector<YACS::ENGINE::Bloc *> _blocs;
};

template <class T> bloctypeParser<T> bloctypeParser<T>::blocParser;

template <class T>
void bloctypeParser<T>::preset (ENGINE::DataNode* const& n)
{
  DEBTRACE("bloc_preset_set: " << n->getName() );
  _bloc->edAddChild(n);
  std::string fullname = currentProc->names.back()+n->getName();
  currentProc->nodeMap[fullname]=n;
}

template <class T>
void bloctypeParser<T>::outnode (ENGINE::DataNode* const& n)
{
  DEBTRACE("bloc_outnode: " << n->getName() );
  _bloc->edAddChild(n);
  std::string fullname = currentProc->names.back()+n->getName();
  currentProc->nodeMap[fullname]=n;
}

template <>
void bloctypeParser<YACS::ENGINE::Bloc*>::name (const std::string& name)
{
  DEBTRACE( "bloc_name: " << name )             
  std::string fullname=currentProc->names.back()+name;
  _bloc=theRuntime->createBloc(name);
  _blocs.push_back(_bloc);
  currentProc->names.push_back(fullname+'.');
}

}

#include "loopParsers.hxx"
#include "switchParsers.hxx"
#include "presetParsers.hxx"
#include "outputParsers.hxx"

namespace YACS
{

template <class T>
void bloctypeParser<T>::onStart(const XML_Char* el, const XML_Char** attr)
{
  DEBTRACE( "bloctypeParser::onStart: " << el )             
  std::string element(el);
  checkOrder(element);
  parser* pp=&parser::main_parser;
  if(element == "property")pp=&propertytypeParser::propertyParser;

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

  this->SetUserDataAndPush(pp);
  pp->init();
  pp->pre();
  pp->buildAttr(attr);
}

template <class T>
void bloctypeParser<T>::onEnd(const char *el,parser* child)
{
  DEBTRACE( "bloctypeParser::onEnd: " << el )             
  std::string element(el);
  if(element == "property")property(((propertytypeParser*)child)->post());
  else if(element == "inline")inline_(((inlinetypeParser<>*)child)->post());
  else if(element == "sinline")sinline(((sinlinetypeParser<>*)child)->post());
  else if(element == "service")service(((servicetypeParser<>*)child)->post());
  else if(element == "server")server(((servertypeParser<>*)child)->post());
  else if(element == "remote")remote(((remotetypeParser<>*)child)->post());
  else if(element == "node")node(((nodetypeParser<>*)child)->post());
  else if(element == "datanode")preset(((presettypeParser<>*)child)->post());
  else if(element == "outnode")outnode(((outnodetypeParser<>*)child)->post());

  else if(element == "bloc")bloc(((bloctypeParser<>*)child)->post());
  else if(element == "forloop")forloop(((forlooptypeParser<>*)child)->post());
  else if(element == "optimizer")optimizer(((optimizerlooptypeParser<>*)child)->post());
  else if(element == "foreach")foreach(((foreachlooptypeParser<>*)child)->post());
  else if(element == "while")while_(((whilelooptypeParser<>*)child)->post());
  else if(element == "switch")switch_(((switchtypeParser*)child)->post());

  else if(element == "control") control(((controltypeParser<>*)child)->post());
  else if(element == "datalink") datalink(((linktypeParser<>*)child)->post());
  else if(element == "stream") stream(((streamtypeParser<>*)child)->post());
  else if(element == "parameter") parameter(((parametertypeParser*)child)->post());
}

}

#endif
