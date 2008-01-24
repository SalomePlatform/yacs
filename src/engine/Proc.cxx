#include "Proc.hxx"
#include "ElementaryNode.hxx"
#include "Runtime.hxx"
#include "Container.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "Logger.hxx"
#include "Visitor.hxx"
#include <sstream>
#include <set>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;

Proc::Proc(const std::string& name):Bloc(name)
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

TypeCode *Proc::createInterfaceTc(const std::string& id, const std::string& name,
                                  std::list<TypeCodeObjref *> ltc)
{
  return TypeCode::interfaceTc(id.c_str(),name.c_str(),ltc);
}

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
  if(typeMap.count(name)==0)
    {
      std::stringstream msg;
      msg << "Type " << name << " does not exist" ;
      msg << " (" <<__FILE__ << ":" << __LINE__ << ")";
      throw Exception(msg.str());
    }
  return typeMap[name];
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
      return inputPort->dump();
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
      return outputPort->dump();
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
  if ( YACS::ENGINE::ElementaryNode *elemNode = dynamic_cast<YACS::ENGINE::ElementaryNode*>(node))
    return elemNode->getErrorDetails();
  else
    {
      msg << "node id " << nodeNumId << " is not an ElementaryNode";
      return msg.str();
    }
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

