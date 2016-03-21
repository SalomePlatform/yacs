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

#ifndef _LOOPPARSER_HXX_
#define _LOOPPARSER_HXX_

#include "parserBase.hxx"

#include "Bloc.hxx"
#include "Proc.hxx"
#include "Logger.hxx"
#include "Exception.hxx"
#include "InlineNode.hxx"
#include "ServiceNode.hxx"
#include "ServiceInlineNode.hxx"
#include "OutputDataStreamPort.hxx"
#include "InputDataStreamPort.hxx"
#include "ForLoop.hxx"
#include "WhileLoop.hxx"
#include "Switch.hxx"
#include "ForEachLoop.hxx"
#include "OptimizerLoop.hxx"

#include <vector>
#include <string>
#include <sstream>

namespace YACS
{

template <class T=ENGINE::Loop*>
struct looptypeParser:parser
{
  virtual void onStart(const XML_Char* el, const XML_Char** attr);
  virtual void onEnd(const char *el,parser* child);
  virtual void buildAttr(const XML_Char** attr);
  virtual void pre ();
  virtual void name (const std::string& name);
  virtual void state (const std::string& name);
  virtual void property (const myprop& prop);
  virtual void inline_ (ENGINE::InlineNode* const& n);
  virtual void sinline (ENGINE::ServiceInlineNode* const& n);
  virtual void service (ENGINE::ServiceNode* const& n);
  virtual void server (ENGINE::ServerNode* const& n);
  virtual void remote (ENGINE::InlineNode* const& n);
  virtual void node (ENGINE::InlineNode* const& n);
  virtual void forloop (ENGINE::ForLoop* const& b);
  virtual void foreach (ENGINE::ForEachLoop* const& b);
  virtual void optimizer (ENGINE::OptimizerLoop* const& b);
  virtual void while_ (ENGINE::WhileLoop* const& b);
  virtual void switch_ (ENGINE::Switch* const& b);
  virtual void bloc (ENGINE::Bloc* const& b);
  virtual void datalink (const mylink& l);
  std::string _state;
  T _cnode;
  std::vector<T> _cnodes;
};

}

namespace YACS
{
  // A loop can contain forloop, whileloop or foreachloop
  // We must respect the order : definition of loop, then while, for, .. and then onStart, onEnd for loop !!!
  //
  static std::string t3[]={"inline","sinline","service","server", "remote", "node","forloop","foreach","optimizer","while","switch","bloc",""};

template <class T>
void looptypeParser<T>::buildAttr(const XML_Char** attr)
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
template <class T>
void looptypeParser<T>::pre ()
    {
      _state="";
      _cnode=0;
    }
template <class T>
void looptypeParser<T>::name (const std::string& name)
    {
      DEBTRACE( "bloc_name: " << name );
    }
template <class T>
void looptypeParser<T>::state (const std::string& name)
    {
      DEBTRACE( "bloc_state: " << name );
      _state=name;
    }
template <class T>
void looptypeParser<T>::property (const myprop& prop)
    {
      DEBTRACE( "property_set" << prop._name << prop._value );
    }

template <class T>
void looptypeParser<T>::inline_ (ENGINE::InlineNode* const& n)
    {
      DEBTRACE( "loop_inline" << n->getName() );
      _cnode->edSetNode(n);
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->inlineMap[fullname]=n;
    }
template <class T>
void looptypeParser<T>::sinline (ENGINE::ServiceInlineNode* const& n)
    {
      DEBTRACE( "loop_sinline" << n->getName() )             
      _cnode->edSetNode(n);
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->serviceMap[fullname]=n;
    }
template <class T>
void looptypeParser<T>::service (ENGINE::ServiceNode* const& n)
    {
      DEBTRACE( "loop_service" << n->getName() )             
      _cnode->edSetNode(n);
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->serviceMap[fullname]=n;
    }
template <class T>
void looptypeParser<T>::server (ENGINE::ServerNode* const& n)
    {
      DEBTRACE( "loop_server" << n->getName() )             
      _cnode->edSetNode(n);
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->inlineMap[fullname]=n;
    }

template <class T>
void looptypeParser<T>::remote (YACS::ENGINE::InlineNode* const& n)
    {
      DEBTRACE( "loop_remote: " << n->getName() )
      _cnode->edSetNode(n);
      std::string fullname = currentProc->names.back()+n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->inlineMap[fullname]=n;
    }

template <class T>
void looptypeParser<T>::node (ENGINE::InlineNode* const& n)
    {
      DEBTRACE( "loop_node" << n->getName() )             
      _cnode->edSetNode(n);
      std::string fullname=currentProc->names.back()+ n->getName();
      currentProc->nodeMap[fullname]=n;
      currentProc->inlineMap[fullname]=n;
    }
template <class T>
void looptypeParser<T>::forloop (ENGINE::ForLoop* const& b)
    {
      DEBTRACE( "loop_forloop" << b->getName() )             
      _cnode->edSetNode(b);
      std::string fullname=currentProc->names.back()+ b->getName();
      currentProc->nodeMap[fullname]=b;
    }
template <class T>
void looptypeParser<T>::foreach (ENGINE::ForEachLoop* const& b)
    {
      DEBTRACE("loop_foreach" << b->getName())
      _cnode->edSetNode(b);
      std::string fullname=currentProc->names.back()+ b->getName();
      currentProc->nodeMap[fullname]=b;
      fullname += ".splitter";
      currentProc->nodeMap[fullname]=b->getChildByShortName("splitter");
    }
template <class T>
void looptypeParser<T>::optimizer (ENGINE::OptimizerLoop* const& b)
  {
    DEBTRACE( "loop_optimizer: " << b->getName() );
    _cnode->edSetNode(b);
    std::string fullname = currentProc->names.back()+b->getName();
    currentProc->nodeMap[fullname]=b;
    //fullname += ".splitter";
    //currentProc->nodeMap[fullname]=b->getChildByShortName("splitter");
  }
template <class T>
void looptypeParser<T>::while_ (ENGINE::WhileLoop* const& b)
    {
      DEBTRACE( "loop_while: " << b->getName() )             
      _cnode->edSetNode(b);
      std::string fullname=currentProc->names.back()+ b->getName();
      currentProc->nodeMap[fullname]=b;
    }
template <class T>
void looptypeParser<T>::switch_ (ENGINE::Switch* const& b)
    {
      DEBTRACE( "loop_switch: " << b->getName() )             
      _cnode->edSetNode(b);
      std::string fullname=currentProc->names.back()+ b->getName();
      currentProc->nodeMap[fullname]=b;
    }
template <class T>
void looptypeParser<T>::bloc (ENGINE::Bloc* const& b)
    {
      DEBTRACE( "loop_bloc " << b->getName() )             
      _cnode->edSetNode(b);
      std::string fullname=currentProc->names.back()+ b->getName();
      currentProc->nodeMap[fullname]=b;
    }

template <class T>
void looptypeParser<T>::datalink (const mylink& l)
    {
      DEBTRACE( "loop_datalink: " << l.fromnode() << l.fromport() << l.tonode() << l.toport())
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
            _cnode->edAddDFLink(currentProc->nodeMap[fromname]->getOutputPort(l.fromport()),
                                currentProc->nodeMap[toname]->getInputPort(l.toport()));
          else
            _cnode->edAddLink(currentProc->nodeMap[fromname]->getOutputPort(l.fromport()),
                              currentProc->nodeMap[toname]->getInputPort(l.toport()));
        }
      catch(Exception& e)
        {
            this->logError(e.what());
        }
    }


}

namespace YACS
{

  // While loop specialization

template <class T=ENGINE::WhileLoop*>
struct whilelooptypeParser:looptypeParser<T>
{
  static whilelooptypeParser<T> whileloopParser;
  virtual void name (const std::string& name);
  virtual T post();
};

template <class T> whilelooptypeParser<T> whilelooptypeParser<T>::whileloopParser;

template <class T>
void whilelooptypeParser<T>::name (const std::string& name)
    {
      DEBTRACE( "while_name: " << name )             
      std::string fullname=currentProc->names.back()+name;
      this->_cnode=theRuntime->createWhileLoop(name);
      currentProc->nodeMap[fullname]=this->_cnode;
      this->_cnodes.push_back(this->_cnode);
      currentProc->names.push_back(fullname+'.');
    }

template <class T>
T whilelooptypeParser<T>::post()
    {
      DEBTRACE( "while_post" << this->_cnode->getName() )             
      ENGINE::InputPort *cond=this->_cnode->edGetConditionPort();
      cond->edInit(true);
      T b=this->_cnode;
      this->_cnodes.pop_back();
      currentProc->names.pop_back();
      this->_cnode=this->_cnodes.empty() ? 0 : this->_cnodes.back();
      return b;
    }
}




namespace YACS
{
  // For loop specialization

template <class T=ENGINE::ForLoop*>
struct forlooptypeParser:looptypeParser<T>
{
  static forlooptypeParser<T> forloopParser;
  virtual void buildAttr(const XML_Char** attr);
  virtual void name (const std::string& name);
  virtual void nsteps (const int& n);
  virtual T post();
  int _nsteps;
};

template <class T> forlooptypeParser<T> forlooptypeParser<T>::forloopParser;

template <class T>
void forlooptypeParser<T>::buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      this->required("name",attr);
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
          if(std::string(attr[i]) == "state")this->state(attr[i+1]);
          if(std::string(attr[i]) == "nsteps")nsteps(atoi(attr[i+1]));
        }
    }

template <class T>
  void forlooptypeParser<T>::name (const std::string& name)
    {
      DEBTRACE( "forloop_name: " << name );
      std::string fullname=currentProc->names.back()+name;
      this->_cnode=theRuntime->createForLoop(name);
      currentProc->nodeMap[fullname]=this->_cnode;
      this->_cnodes.push_back(this->_cnode);
      currentProc->names.push_back(fullname+'.');
      _nsteps=0;
    }

template <class T>
void forlooptypeParser<T>::nsteps (const int& n)
    {
      DEBTRACE( "forloop_nsteps: " << n )             
      if(!this->_cnode)
          throw Exception("Node name must be defined before nsteps");
      ENGINE::InputPort *iNbTimes=this->_cnode->edGetNbOfTimesInputPort();
      iNbTimes->edInit(n);
    }

template <class T>
   T forlooptypeParser<T>::post()
    {
      DEBTRACE( "forloop_post" )             
      T b=this->_cnode;
      this->_cnodes.pop_back();
      currentProc->names.pop_back();
      this->_cnode=this->_cnodes.empty() ? 0 : this->_cnodes.back();
      return b;
    }

}

namespace YACS
{
// pseudo composed node, used to store the init or finalize node of a DynParaLoop
class PseudoComposedNode
{
public:
  void edSetNode(ENGINE::Node * node) { _node = node; }
  ENGINE::Node * getNode() { return _node; }

  // Those two methods should never be called
  bool edAddLink(ENGINE::OutPort *start, ENGINE::InPort *end) throw(Exception) { YASSERT(false); }
  bool edAddDFLink(ENGINE::OutPort *start, ENGINE::InPort *end) throw(Exception) { YASSERT(false); }

protected:
  ENGINE::Node * _node;
};

// pseudo composed node parser specialization for DynParaLoop init and finalize nodes
template <class T=PseudoComposedNode*>
struct pseudocomposednodetypeParser:looptypeParser<T>
{
  static pseudocomposednodetypeParser<T> pseudocomposednodeParser;

  virtual void buildAttr(const XML_Char** attr)
    {
      this->_cnode = new PseudoComposedNode();
      this->_cnodes.push_back(this->_cnode);
    }

  virtual T post()
    {
      DEBTRACE("pseudocomposednode_post" << this->_cnode->getNode()->getName())
      T b = this->_cnode;
      this->_cnodes.pop_back();
      this->_cnode=this->_cnodes.empty() ? 0 : this->_cnodes.back();
      return b;
    }

  virtual void datalink(const mylink & l)
    {
      throw YACS::Exception("Unexpected datalink element in DynParaLoop init or finalize node");
    }

};

template <class T> pseudocomposednodetypeParser<T> pseudocomposednodetypeParser<T>::pseudocomposednodeParser;

}

namespace YACS
{
// dynparaloop specialization

template <class T=ENGINE::DynParaLoop*>
struct dynparalooptypeParser:looptypeParser<T>
{
  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      DEBTRACE( "dynparalooptypeParser::onStart: " << el )
      std::string element(el);
      this->maxcount("initnode",1,element);
      this->maxcount("finalizenode",1,element);
      if (element == "initnode" || element == "finalizenode")
        {
          parser* pp = &pseudocomposednodetypeParser<>::pseudocomposednodeParser;
          this->SetUserDataAndPush(pp);
          pp->init();
          pp->pre();
          pp->buildAttr(attr);
        }
      else
        {
          this->looptypeParser<T>::onStart(el, attr);
        }
    }

  virtual void onEnd(const char *el, parser* child)
    {
      DEBTRACE( "dynparalooptypeParser::onEnd: " << el )
      std::string element(el);
      if (element == "initnode") initnode(((pseudocomposednodetypeParser<>*)child)->post());
      else if (element == "finalizenode") finalizenode(((pseudocomposednodetypeParser<>*)child)->post());
      else this->looptypeParser<T>::onEnd(el, child);
    }

  virtual void initnode(PseudoComposedNode * const& n)
    {
      DEBTRACE( "dynparaloop_initnode: " << n->getNode()->getName() )
      this->_cnode->edSetInitNode(n->getNode());
      delete n;
    }

  virtual void finalizenode(PseudoComposedNode * const& n)
    {
      DEBTRACE( "dynparaloop_finalizenode: " << n->getNode()->getName() )
      this->_cnode->edSetFinalizeNode(n->getNode());
      delete n;
    }
};

}

namespace YACS
{
  // Foreach loop specialization

template <class T=ENGINE::ForEachLoop*>
struct foreachlooptypeParser:dynparalooptypeParser<T>
{
  static foreachlooptypeParser<T> foreachloopParser;

  virtual void buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      this->required("name",attr);
      this->required("type",attr);
      for (int i = 0; attr[i]; i += 2) 
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
          if(std::string(attr[i]) == "state")this->state(attr[i+1]);
          if(std::string(attr[i]) == "nbranch")nbranch(atoi(attr[i+1]));
          if(std::string(attr[i]) == "type")datatype(attr[i+1]);
        }
      postAttr();
    }
  virtual void pre ()
    {
      _nbranch=0;
      this->looptypeParser<T>::pre();
    }
  virtual void name (const std::string& name)
    {
      DEBTRACE("foreach_name: " << name)
      _name=name;
      _fullname=currentProc->names.back()+name;
    }
  virtual void nbranch (const int& n)
    {
      DEBTRACE("foreach_nbranch: " << n )
      _nbranch=n;
    }
  virtual void datatype (const std::string& type)
    {
      DEBTRACE("foreach_datatype: "<< type)
      _datatype=type;
    }
  virtual void postAttr()
    {
      if(currentProc->typeMap.count(_datatype)==0)
      {
        //Check if the typecode is defined in the runtime
        YACS::ENGINE::TypeCode* t=theRuntime->getTypeCode(_datatype);
        if(t==0)
        {
          std::stringstream msg;
          msg << "Type "<< _datatype <<" does not exist"<<" ("<<__FILE__<<":"<<__LINE__<< ")";
          throw Exception(msg.str());
        }
        else
        {
          currentProc->typeMap[_datatype]=t;
          t->incrRef();
        }
      }
      this->_cnode=theRuntime->createForEachLoop(_name,currentProc->typeMap[_datatype]);
      //set number of branches
      if(_nbranch > 0)this->_cnode->edGetNbOfBranchesPort()->edInit(_nbranch);
      this->_cnodes.push_back(this->_cnode);
      currentProc->names.push_back(_fullname + '.');
    }
  virtual T post()
    {
      DEBTRACE("foreach_post" << this->_cnode->getName())
      T b=this->_cnode;
      this->_cnodes.pop_back();
      currentProc->names.pop_back();
      this->_cnode=this->_cnodes.empty() ? 0 : this->_cnodes.back();
      return b;
    }
  int _nbranch;
  std::string _fullname;
  std::string _name;
  std::string _datatype;
};

template <class T> foreachlooptypeParser<T> foreachlooptypeParser<T>::foreachloopParser;

}

namespace YACS
{
  // optimizer loop specialization

template <class T=ENGINE::OptimizerLoop*>
struct optimizerlooptypeParser:dynparalooptypeParser<T>
{
  static optimizerlooptypeParser<T> optimizerloopParser;

  virtual void buildAttr(const XML_Char** attr)
    {
      if (!attr)
        return;
      this->required("name",attr);
      this->required("lib",attr);
      this->required("entry",attr);
      for (int i = 0; attr[i]; i += 2)
        {
          if(std::string(attr[i]) == "name")name(attr[i+1]);
          if(std::string(attr[i]) == "state")this->state(attr[i+1]);
          if(std::string(attr[i]) == "nbranch")nbranch(atoi(attr[i+1]));
          if(std::string(attr[i]) == "lib")lib(attr[i+1]);
          if(std::string(attr[i]) == "entry")entry(attr[i+1]);
          if(std::string(attr[i]) == "kind")kind(attr[i+1]);
        }
      postAttr();
    }
  virtual void pre ()
    {
      _nbranch=0;
      this->looptypeParser<T>::pre();
    }
  virtual void name (const std::string& name)
    {
      DEBTRACE("optimizer_name: " << name)
      _name=name;
      _fullname=currentProc->names.back()+name;
    }
  virtual void lib (const std::string& name)
    {
      _lib=name;
    }
  virtual void entry (const std::string& name)
    {
      _entry=name;
    }
  virtual void nbranch (const int& n)
    {
      DEBTRACE("optimizer_nbranch: " << n )
      _nbranch=n;
    }
  virtual void kind (const std::string& name)
    {
      _kind=name;
    }
  virtual void postAttr()
    {
      this->_cnode=theRuntime->createOptimizerLoop(_name,_lib,_entry,true,_kind, currentProc);
      //set number of branches
      if(_nbranch > 0)this->_cnode->edGetNbOfBranchesPort()->edInit(_nbranch);
      this->_cnodes.push_back(this->_cnode);
      currentProc->names.push_back(_fullname + '.');
    }
  virtual T post()
    {
      DEBTRACE("optimizer_post" << this->_cnode->getName())
      T b=this->_cnode;
      this->_cnodes.pop_back();
      currentProc->names.pop_back();
      this->_cnode=this->_cnodes.empty() ? 0 : this->_cnodes.back();
      return b;
    }

  int _nbranch;
  std::string _fullname;
  std::string _name;
  std::string _entry;
  std::string _kind;
  std::string _lib;

};

template <class T> optimizerlooptypeParser<T> optimizerlooptypeParser<T>::optimizerloopParser;

}

#include "blocParsers.hxx"
#include "switchParsers.hxx"

namespace YACS
{

template <class T>
void looptypeParser<T>::onStart(const XML_Char* el, const XML_Char** attr)
{
  DEBTRACE( "looptypeParser::onStart: " << el )
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
  this->maxchoice(t3,1,element);
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
  else if(element == "datalink")pp=&linktypeParser<>::linkParser;
  this->SetUserDataAndPush(pp);
  pp->init();
  pp->pre();
  pp->buildAttr(attr);
}

template <class T>
void looptypeParser<T>::onEnd(const char *el,parser* child)
{
  DEBTRACE( "looptypeParser::onEnd: " << el )
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

  else if(element == "datalink") datalink(((linktypeParser<>*)child)->post());
}

}
#endif
