//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "Proc.hxx"
#include "ElementaryNode.hxx"
#include "Runtime.hxx"
#include "Container.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "TypeCode.hxx"
#include "Logger.hxx"
#include "Visitor.hxx"
#include "VisitorSaveSchema.hxx"
#include "VisitorSaveState.hxx"
#include <sstream>
#include <set>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;

Proc::Proc(const std::string& name):Bloc(name),_edition(false)
{
  Runtime *theRuntime=getRuntime();
  DEBTRACE("theRuntime->_tc_double->ref: " << theRuntime->_tc_double->getRefCnt());
  DEBTRACE("theRuntime->_tc_int->ref: " << theRuntime->_tc_int->getRefCnt());
  DEBTRACE("theRuntime->_tc_string->ref: " << theRuntime->_tc_string->getRefCnt());
  DEBTRACE("theRuntime->_tc_bool->ref: " << theRuntime->_tc_bool->getRefCnt());
  DEBTRACE("theRuntime->_tc_file->ref: " << theRuntime->_tc_file->getRefCnt());
  theRuntime->_tc_double->incrRef();
  theRuntime->_tc_string->incrRef();
  theRuntime->_tc_int->incrRef();
  theRuntime->_tc_bool->incrRef();
  theRuntime->_tc_file->incrRef();
  typeMap["double"]=theRuntime->_tc_double;
  typeMap["string"]=theRuntime->_tc_string;
  typeMap["int"]=theRuntime->_tc_int;
  typeMap["bool"]=theRuntime->_tc_bool;
  typeMap["file"]=theRuntime->_tc_file;
}

Proc::~Proc()
{
  DEBTRACE("Proc::~Proc");
  //for the moment all nodes are owned, so no need to manage their destruction
  //nodeMap, inlineMap, serviceMap will be cleared automatically
  //but we need to destroy TypeCodes
  std::map<std::string, TypeCode *>::iterator pt;
  for(pt=typeMap.begin();pt!=typeMap.end();pt++)
    ((*pt).second)->decrRef();

  //get rid of containers in container map
  std::map<std::string, Container*>::const_iterator it;
  for(it=containerMap.begin();it!=containerMap.end();it++)
    ((*it).second)->decrRef();

  //get rid of loggers in logger map
  std::map<std::string, Logger*>::const_iterator lt;
  for(lt=_loggers.begin();lt!=_loggers.end();lt++)
    delete (*lt).second;
}

void Proc::writeDot(std::ostream &os) const
{
  os << "digraph " << getQualifiedName() << " {\n" ;
  os << "node [ style=\"filled\" ];\n" ;
  os << "compound=true;";
  os << "states [label=< <TABLE> <TR> <TD BGCOLOR=\"pink\" > Ready</TD> <TD BGCOLOR=\"magenta\" > Toload</TD> </TR> <TR> <TD BGCOLOR=\"magenta\" > Loaded</TD> <TD BGCOLOR=\"purple\" > Toactivate</TD> </TR> <TR> <TD BGCOLOR=\"blue\" > Activated</TD> <TD BGCOLOR=\"green\" > Done</TD> </TR> <TR> <TD BGCOLOR=\"red\" > Error</TD> <TD BGCOLOR=\"orange\" > Failed</TD> </TR> <TR> <TD BGCOLOR=\"grey\" > Disabled</TD> <TD BGCOLOR=\"white\" > Pause</TD> </TR> </TABLE>> \n shape = plaintext \n style = invis \n ];\n";

  Bloc::writeDot(os);
  os << "}\n" ;
}

std::ostream& operator<< (std::ostream& os, const Proc& p)
{
  os << "Proc" ;
  return os;
}

TypeCode *Proc::createType(const std::string& name, const std::string& kind)
{
  TypeCode* t;
  if(kind=="double")
    t=getRuntime()->_tc_double;
  else if(kind=="string")
    t=getRuntime()->_tc_string;
  else if(kind=="int")
    t=getRuntime()->_tc_int;
  else if(kind=="bool")
    t=getRuntime()->_tc_bool;
  else
    throw Exception("Unknown kind");

  t->incrRef();
  return t;
}

//! Create an object reference TypeCode 
/*!
 * \param id: the TypeCode repository id
 * \param name: the TypeCode name
 * \param ltc: a liste of object reference TypeCode to use as base types for this type
 * \return the created TypeCode
 */
TypeCode *Proc::createInterfaceTc(const std::string& id, const std::string& name,
                                  std::list<TypeCodeObjref *> ltc)
{
  return TypeCode::interfaceTc(id.c_str(),name.c_str(),ltc);
}

//! Create a sequence TypeCode 
/*!
 * \param id: the TypeCode repository id ("" for normal use)
 * \param name: the TypeCode name
 * \param content: the element TypeCode 
 * \return the created TypeCode
 */
TypeCode * Proc::createSequenceTc (const std::string& id, const std::string& name,
                                   TypeCode *content)
{
  return TypeCode::sequenceTc(id.c_str(),name.c_str(),content);
}

TypeCode * Proc::createStructTc (const std::string& id, const std::string& name)
{
  return TypeCode::structTc(id.c_str(),name.c_str());
}

TypeCode * Proc::getTypeCode (const std::string& name)
{
  TypeCode* aTC=0;
  if(typeMap.count(name)==0)
    aTC=getRuntime()->getTypeCode(name);
  else
    aTC=typeMap[name];

  if(!aTC)
    {
      std::stringstream msg;
      msg << "Type " << name << " does not exist" ;
      msg << " (" <<__FILE__ << ":" << __LINE__ << ")";
      throw Exception(msg.str());
    }

  return aTC;
}

void Proc::setTypeCode (const std::string& name,TypeCode *t)
{
  if(typeMap.count(name)!=0)
    typeMap[name]->decrRef();
  typeMap[name]=t;
}


void Proc::accept(Visitor *visitor)
{
  visitor->visitProc(this);
}

void Proc::setName(const std::string& name)
{
  _name = name;
}

YACS::StatesForNode Proc::getNodeState(int numId)
{
  if(YACS::ENGINE::Node::idMap.count(numId) == 0)
    {
      cerr << "Unknown node id " << numId << endl;
      return YACS::UNDEFINED;
    }
  YACS::ENGINE::Node* node = YACS::ENGINE::Node::idMap[numId];
  YACS::StatesForNode state = node->getEffectiveState();
  return state;
}

std::string Proc::getXMLState(int numId)
{
  if(YACS::ENGINE::Node::idMap.count(numId) == 0)
    {
      cerr << "Unknown node id " << numId << endl;
      return "<state>unknown</state>";
    }
  YACS::ENGINE::Node* node = YACS::ENGINE::Node::idMap[numId];
  stringstream msg;
  msg << "<state>" << node->getEffectiveState() << "</state>";
  msg << "<name>" << node->getQualifiedName() << "</name>";
  msg << "<id>" << numId << "</id>";
  return msg.str();
}

std::string Proc::getInPortValue(int nodeNumId, std::string portName)
{
  DEBTRACE("Proc::getInPortValue " << nodeNumId << " " << portName);
  stringstream msg;
  if(YACS::ENGINE::Node::idMap.count(nodeNumId) == 0)
    {
      msg << "<value><error>unknown node id: " << nodeNumId << "</error></value>";
      return msg.str();
    }
  try
    {
      YACS::ENGINE::Node* node = YACS::ENGINE::Node::idMap[nodeNumId];
      InputPort * inputPort = node->getInputPort(portName);
      return inputPort->getAsString();
    }
  catch(YACS::Exception& ex)
    {
      DEBTRACE("Proc::getInPortValue " << ex.what());
      msg << "<value><error>" << ex.what() << "</error></value>";
      return msg.str();
    }
}

std::string Proc::getOutPortValue(int nodeNumId, std::string portName)
{
  DEBTRACE("Proc::getOutPortValue " << nodeNumId << " " << portName);
  stringstream msg;
  if(YACS::ENGINE::Node::idMap.count(nodeNumId) == 0)
    {
      msg << "<value><error>unknown node id: " << nodeNumId << "</error></value>";
      return msg.str();
    }
  try
    {
      YACS::ENGINE::Node* node = YACS::ENGINE::Node::idMap[nodeNumId];
      OutputPort * outputPort = node->getOutputPort(portName);
      return outputPort->getAsString();
    }
  catch(YACS::Exception& ex)
    {
      DEBTRACE("Proc::getOutPortValue " << ex.what());
      msg << "<value><error>" << ex.what() << "</error></value>";
      return msg.str();
    }
}

std::string Proc::getNodeErrorDetails(int nodeNumId)
{
  DEBTRACE("Proc::getNodeErrorDetails " << nodeNumId);
  stringstream msg;
  if(YACS::ENGINE::Node::idMap.count(nodeNumId) == 0)
    {
      msg << "Unknown node id " << nodeNumId;
      return msg.str();
    }
  YACS::ENGINE::Node* node = YACS::ENGINE::Node::idMap[nodeNumId];
  return node->getErrorDetails();
}

std::string Proc::getNodeErrorReport(int nodeNumId)
{
  DEBTRACE("Proc::getNodeErrorReport " << nodeNumId);
  stringstream msg;
  if(YACS::ENGINE::Node::idMap.count(nodeNumId) == 0)
    {
      msg << "Unknown node id " << nodeNumId;
      return msg.str();
    }
  YACS::ENGINE::Node* node = YACS::ENGINE::Node::idMap[nodeNumId];
  return node->getErrorReport();
}

std::string Proc::getNodeContainerLog(int nodeNumId)
{
  DEBTRACE("Proc::getNodeContainerLog " << nodeNumId);
  stringstream msg;
  if(YACS::ENGINE::Node::idMap.count(nodeNumId) == 0)
    {
      msg << "Unknown node id " << nodeNumId;
      return msg.str();
    }
  YACS::ENGINE::Node* node = YACS::ENGINE::Node::idMap[nodeNumId];
  return node->getContainerLog();
}

std::list<int> Proc::getNumIds()
{
  list<YACS::ENGINE::Node *> nodes = getAllRecursiveConstituents();
  int len = nodes.size();
  list<int> numids;
  for( list<YACS::ENGINE::Node *>::const_iterator iter = nodes.begin();
       iter != nodes.end(); iter++)
    {
      numids.push_back((*iter)->getNumId());
    }
  numids.push_back(this->getNumId());
  return numids;
}

std::list<std::string> Proc::getIds()
{
  list<YACS::ENGINE::Node *> nodes = getAllRecursiveConstituents();
  int len = nodes.size();
  list<string> ids;
  for( list<YACS::ENGINE::Node *>::const_iterator iter = nodes.begin();
       iter != nodes.end(); iter++)
    {
      ids.push_back(getChildName(*iter));
    }
  ids.push_back("_root_");
  return ids;
}

Logger *Proc::getLogger(const std::string& name)
{
  Logger* logger;
  LoggerMap::const_iterator it = _loggers.find(name);

  if (it != _loggers.end())
  {
    logger = it->second;
  }
  else
  {
    logger = new Logger(name);
    _loggers[name]=logger;
  }
  return logger;
}

void Proc::setEdition(bool edition)
{
  DEBTRACE("Proc::setEdition: " << edition);
  _edition=edition;
  if(_edition)
    edUpdateState();
}
//! Sets Proc in modified state and update state if in edition mode
/*!
 *
 */
void Proc::modified()
{
  DEBTRACE("Proc::modified() " << _edition);
  _modified=1;
  if(_edition)
    edUpdateState();
}

//! Save Proc in XML schema file
/*!
 * \param xmlSchemaFile: the file name
 */
void Proc::saveSchema(std::string xmlSchemaFile)
{
  VisitorSaveSchema vss(this);
  vss.openFileSchema(xmlSchemaFile);
  accept(&vss);
  vss.closeFileSchema();
}

//! Save Proc state in XML state file
/*!
 * \param xmlStateFile: the file name
 */
void Proc::saveState(std::string xmlStateFile)
{
  VisitorSaveState vst(this);
  vst.openFileDump(xmlStateFile);
  accept(&vst);
  vst.closeFileDump();
}

