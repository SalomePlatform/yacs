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

#include "LoadState.hxx"
#include "Proc.hxx"
#include "Node.hxx"
#include "ForLoop.hxx"
#include "WhileLoop.hxx"
#include "Switch.hxx"
#include "InGate.hxx"
#include "Runtime.hxx"
#include "InputPort.hxx"
#include "ElementaryNode.hxx"

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdarg>
#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

XMLReadState    stateParser::_state;
std::string     stateParser::_what;
std::stack<XMLReadState> stateParser::_stackState;
Proc* stateParser::_p;
Runtime* stateParser::_runtime;
std::map<std::string, YACS::StatesForNode> stateParser::_nodeStateValue;
std::map<std::string, YACS::StatesForNode> stateParser::_nodeStates;

// ----------------------------------------------------------------------------
void stateParser::setProc(Proc* p)
{
  _p= p;
}

void stateParser::setRuntime(Runtime* runtime)
{
  _runtime = runtime;
}

void stateParser::init(const xmlChar** p, xmlParserBase* father)
{
  DEBTRACE("stateParser::init()");
  _state = XMLNOCONTEXT;
  _father = father;
  _stackState.push(_state);
  _nodeStateValue["READY"] =YACS::READY;
  _nodeStateValue["TOLOAD"] =YACS::TOLOAD;
  _nodeStateValue["LOADED"] =YACS::LOADED;
  _nodeStateValue["TOACTIVATE"] =YACS::TOACTIVATE;
  _nodeStateValue["ACTIVATED"] =YACS::ACTIVATED;
  _nodeStateValue["DESACTIVATED"] =YACS::DESACTIVATED;
  _nodeStateValue["DONE"] =YACS::DONE;
  _nodeStateValue["SUSPENDED"] =YACS::SUSPENDED;
  _nodeStateValue["LOADFAILED"] =YACS::LOADFAILED;
  _nodeStateValue["EXECFAILED"] =YACS::EXECFAILED;
  _nodeStateValue["PAUSE"] =YACS::PAUSE;
  _nodeStateValue["INTERNALERR"] =YACS::INTERNALERR;
  _nodeStateValue["DISABLED"] =YACS::DISABLED;
  _nodeStateValue["FAILED"] =YACS::FAILED;
  _nodeStateValue["ERROR"] =YACS::ERROR;
}


void stateParser::onStart (const XML_Char* elem, const xmlChar** p)
{
  DEBTRACE("stateParser::onStart");
  string element(elem);
  stateParser *parser = 0;
  if (element == "graphState") parser = new graphParser();
  else
    { 
      _what = "expected <graphState>, got <" + element + ">";
      _state = XMLFATALERROR;
      stopParse(_what);
    }
  if (parser)
    {
      _stackParser.push(parser);
      XML_SetUserData(_xmlParser, parser);
      parser->init(p);
    }
}


void stateParser::onEnd   (const XML_Char* name)
{
  _stackState.pop();
  _state = _stackState.top();
  //cerr << "end " << name << " " << _stackParser.size() << " " << _state << endl;
}


void stateParser::charData(std::string data)
{
  //cerr << "data " << data << endl;
}

// ----------------------------------------------------------------------------

void graphParser::init(const xmlChar** p, xmlParserBase* father)
{
  //  DEBTRACE("graphParser::init()");
  _state = XMLINGRAPH;
  _father = father;
  _stackState.push(_state);
  if (p) getAttributes(p);
}

void graphParser::onStart (const XML_Char* elem, const xmlChar** p)
{
  string element(elem);
  stateParser *parser = 0;
  if (element == "node") parser = new nodeParser();
  else
    { 
      _what = "expected <node>, got <" + element + ">";
      _state = XMLFATALERROR;
      stopParse(_what);
    }
  if (parser)
    {
      _stackParser.push(parser);
      XML_SetUserData(_xmlParser, parser);
      parser->init(p, this);
    }
}


void graphParser::onEnd   (const XML_Char* name)
{
  std::map<std::string, YACS::StatesForNode>::const_iterator it;
  for (it = _nodeStates.begin(); it != _nodeStates.end(); it++)
    {
      Node *node =0;
      string nodeName = it->first;
      DEBTRACE("nodeName = " << nodeName);
      if(_p->getName() == nodeName)
        node = _p;
      else
        node = _p->getChildByName(nodeName);

      InGate* inGate = node->getInGate();
      list<OutGate*> backlinks = inGate->getBackLinks();
      for (list<OutGate*>::iterator io = backlinks.begin(); io != backlinks.end(); io++)
        {
          Node* fromNode = (*io)->getNode();
          string fromName;
          if (fromNode == _p) fromName = fromNode->getName();
          else fromName = _p->getChildName(fromNode);
          if (_nodeStates[fromName] == YACS::DONE)
            {
              DEBTRACE("   fromNode = " << fromName);
              inGate->setPrecursorDone(*io);
            }
        }
    }
  stateParser::onEnd(name);
}

// ----------------------------------------------------------------------------

class outputParser: public stateParser
{
public:
  virtual void init(const xmlChar** p, xmlParserBase* father=0)
  {
    //DEBTRACE("outputParser::init()");
    _state = XMLNOCONTEXT;
    _father = father;
    YASSERT( dynamic_cast<nodeParser*> (father));
    _stackState.push(_state);
    if (p) getAttributes(p);
  }
  virtual void onStart (const XML_Char* elem, const xmlChar** p)
  {
    //DEBTRACE("outputParser::onStart" << elem);
    string element(elem);
    stateParser *parser = 0;
    if      (element == "name")      parser = new attrParser();
    else if (element == "value")     parser = new valueParser();
    else
    { 
      _what = "expected name or value, got <" + element + ">";
      _state = XMLFATALERROR;
      stopParse(_what);
    }
    if (parser)
    {
      _stackParser.push(parser);
      XML_SetUserData(_xmlParser, parser);
      parser->init(p, this);
    }
  }
  virtual void onEnd   (const XML_Char* name)
  {
    //DEBTRACE("outputParser::onEnd" << elem);
    //DEBTRACE("portName: " << _mapAttrib["name"] << "value: " << _data );
    stateParser::onEnd(name);
  }
  virtual void addData(std::string /*value*/)
  {
    //DEBTRACE("outputParser::addData" << elem);
  }
};

// ----------------------------------------------------------------------------

void nodeParser::init(const xmlChar** p, xmlParserBase* father)
{
  DEBTRACE("nodeParser::init()");
  _state = XMLINNODE;
  _father = father;
  _stackState.push(_state);
  if (p) getAttributes(p);
}


void nodeParser::onStart (const XML_Char* elem, const xmlChar** p)
{
  DEBTRACE("nodeParser::onStart" << elem);
  string element(elem);
  stateParser *parser = 0;
  if (element == "inputPort")      parser = new portParser();
  else if (element == "name")      parser = new attrParser();
  else if (element == "state")     parser = new attrParser();
  else if (element == "nsteps")    parser = new attrParser();
  else if (element == "nbdone")    parser = new attrParser();
  else if (element == "condition") parser = new attrParser();
  else if (element == "outputPort") parser = new outputParser();
  else
    { 
      _what = "expected name, state or inputPort, got <" + element + ">";
      _state = XMLFATALERROR;
      stopParse(_what);
    }
  if (parser)
    {
      _stackParser.push(parser);
      XML_SetUserData(_xmlParser, parser);
      parser->init(p, this);
    }
}

void nodeParser::onEnd   (const XML_Char* name)
{
  string nodeName = _mapAttrib["name"];
  string nodeType = _mapAttrib["type"];
  string nodeState = _mapAttrib["state"];
  DEBTRACE( "nodeName: " << nodeName << " nodeType: " << nodeType << " nodeState: " << nodeState );
//   for (std::map< std::string, Node * >::iterator it=_p->nodeMap.begin(); it != _p->nodeMap.end(); it++)
//     cerr << "nodeMap: " << it->first << endl;
  _nodeStates[nodeName] = _nodeStateValue[nodeState];
  Node *node =0;
  if(_p->getName() == nodeName)
    node=_p;
  else 
    node = _p->getChildByName(nodeName);

  YASSERT(_nodeStateValue.find(nodeState) != _nodeStateValue.end());
  YACS::ENGINE::StateLoader(node, _nodeStateValue[nodeState]);

  if (nodeType == "forLoop")
    {
      if (_mapAttrib.find("nsteps") == _mapAttrib.end())
        {
          _what = "no attribute nsteps in forLoop " + _mapAttrib["name"];
          _state = XMLFATALERROR;
          stopParse(_what);
        }
      int nsteps =  atoi(_mapAttrib["nsteps"].c_str());

      if (_mapAttrib.find("nbdone") == _mapAttrib.end())
        {
          _what = "no attribute nbdone in forLoop " + _mapAttrib["name"];
          _state = XMLFATALERROR;
          stopParse(_what);
        }
      int nbdone =  atoi(_mapAttrib["nbdone"].c_str());
      DEBTRACE("nsteps = " << nsteps << ", nbdone = " << nbdone);

      ForLoop* loop = dynamic_cast<ForLoop*>(node);
      if (!loop)
        {
          _what = "node is not a ForLoop: " + _mapAttrib["name"];
          _state = XMLFATALERROR;
          stopParse(_what);
        }
      loop->edGetNbOfTimesInputPort()->edInit(nsteps);
      YACS::ENGINE::NbDoneLoader(loop, nbdone);
    }

  else if (nodeType == "whileLoop")
    {
      if (_mapAttrib.find("nbdone") == _mapAttrib.end())
        {
          _what = "no attribute nbdone in forLoop " + _mapAttrib["name"];
          _state = XMLFATALERROR;
          stopParse(_what);
        }
      int nbdone =  atoi(_mapAttrib["nbdone"].c_str());

      if (_mapAttrib.find("condition") == _mapAttrib.end())
        {
          _what = "no attribute condition in forLoop " + _mapAttrib["name"];
          _state = XMLFATALERROR;
          stopParse(_what);
        }
      bool condition =  atoi(_mapAttrib["condition"].c_str());
      DEBTRACE("condition = " << condition << ", nbdone = " << nbdone);

      WhileLoop* loop = dynamic_cast<WhileLoop*>(node);
      if (!loop)
        {
          _what = "node is not a WhileLoop: " + _mapAttrib["name"];
          _state = XMLFATALERROR;
          stopParse(_what);
        }
      loop->edGetConditionPort()->edInit(condition);
      YACS::ENGINE::NbDoneLoader(loop, nbdone);
    }

  else if (nodeType == "switch")
    {
      if (_mapAttrib.find("condition") == _mapAttrib.end())
        {
          _what = "no attribute condition in switch " + _mapAttrib["name"];
          _state = XMLFATALERROR;
          stopParse(_what);
        }
      int condition =  atoi(_mapAttrib["condition"].c_str());
      DEBTRACE("condition = " << condition);

      Switch* mySwitch = dynamic_cast<Switch*>(node);
      if (!mySwitch)
        {
          _what = "node is not a Switch: " + _mapAttrib["name"];
          _state = XMLFATALERROR;
          stopParse(_what);
        }
      mySwitch->edGetConditionPort()->edInit(condition);
    }

  stateParser::onEnd(name);
}

// ----------------------------------------------------------------------------

void attrParser::init(const xmlChar** p, xmlParserBase* father)
{
  DEBTRACE("attrParser::init()");
  //_state = XMLINNODE;
  _father = father;
  _stackState.push(_state); // keep current state
  if (p) getAttributes(p);
}


void attrParser::onStart (const XML_Char* elem, const xmlChar** p)
{
  string element(elem);
  _what = "expected nothing, got <" + element + ">";
  _state = XMLFATALERROR;
  stopParse(_what);
}

void attrParser::charData(std::string data)
{
  _attrValue = data;
}

void attrParser::onEnd   (const XML_Char* name)
{
  // cerr << "end attrParser " << name << " " << _stackParser.size() << endl;
  YASSERT(_father);
  _father->setAttribute((char*)name, _attrValue);
  stateParser::onEnd(name);
}

// ----------------------------------------------------------------------------

void portParser::init(const xmlChar** p, xmlParserBase* father)
{
  DEBTRACE("portParser::init()");
  _state = XMLINPORT;
  _father = father;
  YASSERT( dynamic_cast<nodeParser*> (father));
  _stackState.push(_state);
  if (p) getAttributes(p);
}


void portParser::onStart (const XML_Char* elem, const xmlChar** p)
{
  DEBTRACE("portParser::onStart" << elem);
  string element(elem);
  stateParser *parser = 0;
  if      (element == "name")      parser = new attrParser();
  else if (element == "value")     parser = new valueParser();
  else
    { 
      _what = "expected name or value, got <" + element + ">";
      _state = XMLFATALERROR;
      stopParse(_what);
    }
  if (parser)
    {
      _stackParser.push(parser);
      XML_SetUserData(_xmlParser, parser);
      parser->init(p, this);
    }
}

void portParser::addData(std::string value)
{
  _data = value;
}

void portParser::onEnd   (const XML_Char* name)
{
  DEBTRACE("portName: " << _mapAttrib["name"] << "value: " << _data );
  string nodeName = _father->getAttribute("name");
  string nodeType = _father->getAttribute("type");
  Node *node = _p->getChildByName(nodeName);
  if (nodeType == "elementaryNode")
    {
      ElementaryNode* eNode = dynamic_cast<ElementaryNode*>(node);
      YASSERT(eNode);
      InputPort *port = eNode->getInputPort(_mapAttrib["name"]);
      if(_data != "")
        port->edInit("XML",_data.c_str());
    }
  else if (nodeType == "forLoop")
    {
      string what="no way to set a port value on port " +  _mapAttrib["name"];
      what += " in node " + nodeName + " of type " + nodeType;
      throw Exception(what);
    }
  else if (nodeType == "whileLoop")
    {
      string what="no way to set a port value on port " +  _mapAttrib["name"];
      what += " in node " + nodeName + " of type " + nodeType;
      throw Exception(what);
    }
  else if (nodeType == "switch")
    {
      string what="no way to set a port value on port " +  _mapAttrib["name"];
      what += " in node " + nodeName + " of type " + nodeType;
      throw Exception(what);
    }
  else if (nodeType == "foreachLoop")
    {
      string what="no way to set a port value on port " +  _mapAttrib["name"];
      what += " in node " + nodeName + " of type " + nodeType;
      throw Exception(what);
    }
  else 
    {
      string what="no way to set a port value on port " +  _mapAttrib["name"];
      what += " in node " + nodeName + " of type " + nodeType;
      throw Exception(what);
    }

  stateParser::onEnd(name);
}

// ----------------------------------------------------------------------------

void valueParser::init(const xmlChar** p, xmlParserBase* father)
{
  DEBTRACE("valueParser::init()");
  _state = XMLINVALUE;
  _father = father;
  _stackState.push(_state);
  if (p) getAttributes(p);
}


void valueParser::onStart (const XML_Char* elem, const xmlChar** p)
{
  string element(elem);
  DEBTRACE("value type " << element );
  stateParser *parser = 0;
  if      (element == "data")      parser = new dataParser();
  else if (element == "array")     parser = new arrayParser();
  else                             parser = new simpleTypeParser();
  if (parser)
    {
      _stackParser.push(parser);
      XML_SetUserData(_xmlParser, parser);
      parser->init(p, this);
    }
}

void valueParser::addData(std::string value)
{
  _data = "<value>" + value + "</value>";
}

void valueParser::onEnd   (const XML_Char* name)
{
  DEBTRACE( _data );
  _father->addData(_data);
  string elem = (char *) name;
  //if (elem == "value" || elem == "data" || elem == "array")
  stateParser::onEnd(name);
  //else YASSERT(0); //DEBTRACE("valueParser::onEnd " << elem);
}

// ----------------------------------------------------------------------------

void arrayParser::init(const xmlChar** p, xmlParserBase* father)
{
  DEBTRACE("arrayParser::init()");
  _state = XMLINVALUE;
  _father = father;
  _stackState.push(_state);
  if (p) getAttributes(p);
}


void arrayParser::onStart (const XML_Char* elem, const xmlChar** p)
{
  string element(elem);
  DEBTRACE("array type " << element);
  stateParser *parser = 0;
  if      (element == "data")      parser = new dataParser();
  else
    { 
      _what = "expected data, got <" + element + ">";
      _state = XMLFATALERROR;
      stopParse(_what);
    }
  if (parser)
    {
      _stackParser.push(parser);
      XML_SetUserData(_xmlParser, parser);
      parser->init(p, this);
    }
}

void arrayParser::addData(std::string value)
{
  string val = "<array>" + value + "</array>";
  _data = val;
}


void arrayParser::onEnd   (const XML_Char* name)
{
  // cerr << "arrayParser::onEnd " << name << endl;
  // cerr << _data << endl;
  _father->addData(_data);
  stateParser::onEnd(name);
}

// ----------------------------------------------------------------------------

void dataParser::init(const xmlChar** p, xmlParserBase* father)
{
  DEBTRACE("dataParser::init()");
  _state = XMLINVALUE;
  _father = father;
  _stackState.push(_state);
  if (p) getAttributes(p);
}


void dataParser::onStart (const XML_Char* elem, const xmlChar** p)
{
  string element(elem);
  DEBTRACE("data type " << element );
  stateParser *parser = 0;
  if      (element == "value")      parser = new valueParser();
  else
    { 
      _what = "expected value, got <" + element + ">";
      _state = XMLFATALERROR;
      stopParse(_what);
    }
  if (parser)
    {
      _stackParser.push(parser);
      XML_SetUserData(_xmlParser, parser);
      parser->init(p, this);
    }
}

void dataParser::addData(std::string value)
{
  _dataList.push_back(value);
}

void dataParser::onEnd   (const XML_Char* name)
{
  // cerr << "dataParser::onEnd " << name << endl;
  string val = "<data>";
  while (!_dataList.empty())
    {
      val += _dataList.front();
      _dataList.pop_front();
    }
  val += "</data>";
  // cerr << val << endl;
  _father->addData(val);
  stateParser::onEnd(name);
}

// ----------------------------------------------------------------------------

void simpleTypeParser::init(const xmlChar** p, xmlParserBase* father)
{
  DEBTRACE("simpleTypeParser::init()");
  _state = XMLINVALUE;
  _father = father;
  _stackState.push(_state);
  if (p) getAttributes(p);
}

void simpleTypeParser::onStart (const XML_Char* elem, const xmlChar** p)
{
  string element(elem);
  _what = "expected nothing, got <" + element + ">";
  _state = XMLFATALERROR;
  stopParse(_what);
}

void simpleTypeParser::onEnd   (const XML_Char* name)
{
  string val = string("<") + (char*) name + ">" + _data + "</"  + (char*) name +">";
  DEBTRACE( val );
  _father->addData(val);
  stateParser::onEnd(name);
}

void simpleTypeParser::charData(std::string data)
{
  _data = _data + data;
}



// ----------------------------------------------------------------------------

stateLoader::stateLoader(xmlParserBase* parser,
                         YACS::ENGINE::Proc* p) : xmlReader(parser)
{
  _runtime = getRuntime();
  _p = p;
}

void stateLoader::parse(std::string xmlState)
{
  DEBTRACE("stateLoader::parse");
  stateParser *parser = dynamic_cast<stateParser*> (_rootParser);
  parser->setProc(_p);
  parser->setRuntime(_runtime);

  xmlReader::parse(xmlState);

  DEBTRACE(parser->_state);
  switch (parser->_state)
    {
    case XMLNOCONTEXT:
    case XMLDONE:
      {
        DEBTRACE("parse OK");
        break;
      }
    case XMLFATALERROR:
      {
        string what = "Abort Parse: " + parser->_what;
        throw Exception(what);
        break;
      }
    default:
      {
        string what = "Abort Parse: unknown execution problem";
        throw Exception(what);
        break;
      }
    }
}

void YACS::ENGINE::loadState(YACS::ENGINE::Proc *p,const std::string& xmlStateFile)
{
  DEBTRACE("YACS::ENGINE::loadState");
  p->init();
  p->exUpdateState();
  stateParser* rootParser = new stateParser();
  stateLoader myStateLoader(rootParser, p);
  myStateLoader.parse(xmlStateFile);
}
