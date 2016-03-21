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

#include "VisitorSaveSchema.hxx"

#include "ElementaryNode.hxx"
#include "InlineNode.hxx"
#include "ServiceNode.hxx"
#include "ServerNode.hxx"
#include "ServiceInlineNode.hxx"
#include "Bloc.hxx"
#include "Proc.hxx"
#include "ForEachLoop.hxx"
#include "OptimizerLoop.hxx"
#include "Loop.hxx"
#include "ForLoop.hxx"
#include "WhileLoop.hxx"
#include "Switch.hxx"
#include "InputPort.hxx"
#include "TypeCode.hxx"
#include "HomogeneousPoolContainer.hxx"
#include "ComponentInstance.hxx"
#include "InputDataStreamPort.hxx"
#include "OutputDataStreamPort.hxx"
#include "Container.hxx"
#include "DataNode.hxx"

#include <cassert>
#include <iostream>
#include <string>
#include <set>

using namespace YACS::ENGINE;
using namespace std;

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

/*! \class YACS::ENGINE::VisitorSaveSchema
 *  \brief Base class for all visitors that save a schema.
 *
 *  Can be specialized in runtime.
 */

VisitorSaveSchema::VisitorSaveSchema(ComposedNode *root): _root(root), Visitor(root)
{
}

VisitorSaveSchema::~VisitorSaveSchema()
{
  if (_out)
    {
      _out << "</proc>" << endl;
      _out.close();
    }
}

void VisitorSaveSchema::openFileSchema(std::string xmlSchema) throw(YACS::Exception)
{
  _out.open(xmlSchema.c_str(), ios::out);
  if (!_out)
    {
      string what = "Impossible to open file for writing: " + xmlSchema;
      throw Exception(what);
    }
  _out << "<?xml version='1.0' encoding='iso-8859-1' ?>" << endl;
}

void VisitorSaveSchema::closeFileSchema()
{
  if (!_out) throw Exception("No file open for save schema");
  _out << "</proc>" << endl;
  _out.close();
}

void VisitorSaveSchema::visitBloc(Bloc *node)
{
  DEBTRACE("START visitBloc " << _root->getChildName(node));
  beginCase(node);
  int depth = depthNode(node);
  _out << indent(depth) << "<bloc name=\"" << node->getName() << "\"";
  if (node->getState() == YACS::DISABLED)
    _out << " state=\"disabled\">" << endl;
  else
    _out << ">" << endl;
  writeProperties(node);
  node->ComposedNode::accept(this);
  writeControls(node);
  writeSimpleDataLinks(node);
  writeSimpleStreamLinks(node);
  _out << indent(depth) << "</bloc>" << endl;
  endCase(node);
  DEBTRACE("END visitBloc " << _root->getChildName(node));
}

void VisitorSaveSchema::visitElementaryNode(ElementaryNode *node)
{
  DEBTRACE("START visitElementaryNode " << _root->getChildName(node));
  beginCase(node);
  writeProperties(node);
  writeInputPorts(node);
  writeInputDataStreamPorts(node);
  writeOutputPorts(node);
  writeOutputDataStreamPorts(node);
  endCase(node);
  DEBTRACE("END visitElementaryNode " << _root->getChildName(node));
}

void VisitorSaveSchema::visitForEachLoop(ForEachLoop *node)
{
  DEBTRACE("START visitForEachLoop " << _root->getChildName(node));
  beginCase(node);
  int depth = depthNode(node);

  _out << indent(depth) << "<foreach name=\"" << node->getName() << "\"";
  AnyInputPort *nbranch = static_cast<AnyInputPort*>(node->edGetNbOfBranchesPort());
  if (node->getState() == YACS::DISABLED)
    _out << " state=\"disabled\"";
  if (!nbranch->isEmpty())
    _out << " nbranch=\"" << nbranch->getIntValue() << "\"";
  if (node->edGetSamplePort())
    _out << " type=\"" << node->edGetSamplePort()->edGetType()->name() << "\"";
  _out << ">" << endl;
  
  writeProperties(node);
  node->DynParaLoop::accept(this);
  writeSimpleDataLinks(node);
  writeSimpleStreamLinks(node);
  _out << indent(depth) << "</foreach>" << endl;
  endCase(node);
  DEBTRACE("END visitForEachLoop " << _root->getChildName(node));
}

void VisitorSaveSchema::visitOptimizerLoop(OptimizerLoop *node)
{
  DEBTRACE("START visitOptimizerLoop " << _root->getChildName(node));
  beginCase(node);
  int depth = depthNode(node);

  _out << indent(depth) << "<optimizer name=\"" << node->getName() << "\"";
  AnyInputPort *nbranch = static_cast<AnyInputPort*>(node->edGetNbOfBranchesPort());
  if (node->getState() == YACS::DISABLED)
    _out << " state=\"disabled\"";
  if (!nbranch->isEmpty())
    _out << " nbranch=\"" << nbranch->getIntValue() << "\"";
  _out << " lib=\"" << node->getAlgLib() << "\"";
  _out << " entry=\"" << node->getSymbol() << "\"";
  _out << ">" << endl;

  writeProperties(node);
  node->DynParaLoop::accept(this);
  writeSimpleDataLinks(node);
  writeSimpleStreamLinks(node);
  _out << indent(depth) << "</optimizer>" << endl;
  endCase(node);
  DEBTRACE("END visitOptimizerLoop " << _root->getChildName(node));
}

void VisitorSaveSchema::visitDynParaLoop(DynParaLoop *node)
{
  DEBTRACE("START visitDynParaLoop " << _root->getChildName(node));
  int depth = depthNode(node);
  if (node->getInitNode() != NULL)
    {
      _out << indent(depth+1) << "<initnode>" << endl;
      node->getInitNode()->accept(this);
      _out << indent(depth+1) << "</initnode>" << endl;
    }
  if (node->getExecNode() != NULL)
    {
      node->getExecNode()->accept(this);
    }
  if (node->getFinalizeNode() != NULL)
    {
      _out << indent(depth+1) << "<finalizenode>" << endl;
      node->getFinalizeNode()->accept(this);
      _out << indent(depth+1) << "</finalizenode>" << endl;
    }
  DEBTRACE("END visitDynParaLoop " << _root->getChildName(node));
}

void VisitorSaveSchema::visitForLoop(ForLoop *node)
{
  DEBTRACE("START visitForLoop " << _root->getChildName(node));
  beginCase(node);
  int depth = depthNode(node);
  AnyInputPort *nsteps = static_cast<AnyInputPort*>(node->edGetNbOfTimesInputPort());
  _out << indent(depth) << "<forloop name=\"" << node->getName() << "\"";
  if (node->getState() == YACS::DISABLED)
    _out << " state=\"disabled\"";
  if (nsteps->isEmpty())
    _out << ">" << endl;
  else   
    _out << " nsteps=\"" << nsteps->getIntValue() << "\">" << endl;
  writeProperties(node);
  node->ComposedNode::accept(this);
  writeSimpleDataLinks(node);
  writeSimpleStreamLinks(node);
  _out << indent(depth) << "</forloop>" << endl;
  endCase(node);
  DEBTRACE("END visitForLoop " << _root->getChildName(node));
}

void VisitorSaveSchema::visitInlineNode(InlineNode *node)
{
  DEBTRACE("START visitInlineNode " << _root->getChildName(node));
  beginCase(node);
  int depth = depthNode(node);
  if(node->getExecutionMode()==InlineNode::LOCAL_STR)
    _out << indent(depth) << "<inline name=\"" << node->getName() << "\"";
  else
    _out << indent(depth) << "<remote name=\"" << node->getName() << "\"";
  if (node->getState() == YACS::DISABLED)
    _out << " state=\"disabled\">" << endl;
  else
    _out << ">" << endl;
  _out << indent(depth+1) << "<script><code><![CDATA[";
  _out << node->getScript();
  _out << "]]></code></script>" << endl;

  //add load container if node is remote
  Container *cont = node->getContainer();
  if (cont)
    _out << indent(depth+1) << "<load container=\"" << cont->getName() << "\"/>" << endl;

  writeProperties(node);
  writeInputPorts(node);
  writeInputDataStreamPorts(node);
  writeOutputPorts(node);
  writeOutputDataStreamPorts(node);

  if(node->getExecutionMode()==InlineNode::LOCAL_STR)
    _out << indent(depth) << "</inline>" << endl;
  else
    _out << indent(depth) << "</remote>" << endl;

  endCase(node);
  DEBTRACE("END visitInlineNode " << _root->getChildName(node));
}

void VisitorSaveSchema::visitInlineFuncNode(InlineFuncNode *node)
{
  DEBTRACE("START visitInlineFuncNode " << _root->getChildName(node));
  beginCase(node);
  int depth = depthNode(node);
  if(node->getExecutionMode()==InlineNode::LOCAL_STR)
    _out << indent(depth) << "<inline name=\"" << node->getName() << "\"";
  else
    _out << indent(depth) << "<remote name=\"" << node->getName() << "\"";
  if (node->getState() == YACS::DISABLED)
    _out << " state=\"disabled\">" << endl;
  else
    _out << ">" << endl;
  _out << indent(depth+1) << "<function name=\"" << node->getFname() << "\">" << endl;
  _out << indent(depth+2) << "<code><![CDATA[";
  _out << node->getScript();
  _out << "]]></code>" << endl;
  _out << indent(depth+1) << "</function>" << endl;

  //add load container if node is remote
  Container *cont = node->getContainer();
  if (cont)
    _out << indent(depth+1) << "<load container=\"" << cont->getName() << "\"/>" << endl;

  writeProperties(node);
  writeInputPorts(node);
  writeInputDataStreamPorts(node);
  writeOutputPorts(node);
  writeOutputDataStreamPorts(node);

  if(node->getExecutionMode()==InlineNode::LOCAL_STR)
    _out << indent(depth) << "</inline>" << endl;
  else
    _out << indent(depth) << "</remote>" << endl;

  endCase(node);
  DEBTRACE("END visitInlineFuncNode " << _root->getChildName(node));
}

void VisitorSaveSchema::visitLoop(Loop *node)
{
  DEBTRACE("START visitLoop " << _root->getChildName(node));
  beginCase(node);
  int depth = depthNode(node);
  writeProperties(node);
  node->ComposedNode::accept(this);
  writeControls(node);
  writeSimpleDataLinks(node);
  writeSimpleStreamLinks(node);
  endCase(node);
  DEBTRACE("END visitLoop " << _root->getChildName(node));
}

void VisitorSaveSchema::visitProc(Proc *node)
{
  DEBTRACE("START visitProc " << node->getName());
  _out << "<proc name=\""<< node->getName() << "\">" << endl;
  beginCase(node);
  writeProperties(node);
  writeTypeCodes(node);
  writeContainers(node);
  writeComponentInstances(node);
  node->ComposedNode::accept(this);
  writeControls(node);
  writeSimpleDataLinks(node);
  writeSimpleStreamLinks(node);
  writeParameters(node);
  endCase(node);
  DEBTRACE("END visitProc " << node->getName());
}

void VisitorSaveSchema::visitServiceNode(ServiceNode *node)
{
  DEBTRACE("START visitServiceNode " << _root->getChildName(node));
  beginCase(node);
  int depth = depthNode(node);
  _out << indent(depth) << "<service name=\"" << node->getName() << "\"";
  if (node->getState() == YACS::DISABLED)
    _out << " state=\"disabled\">" << endl;
  else
    _out << ">" << endl;
  if (node->getKind() == "xmlsh")
    {
      _out << indent(depth+1) << "<kind>xmlsh</kind>" << endl;
      _out << indent(depth+1) << "<ref>" << node->getRef() << "</ref>" << endl;
    }
  else
    {
      ComponentInstance *compo = node->getComponent();
      if (compo && !compo->isAnonymous())
        {
          _out << indent(depth+1) << "<componentinstance>" << compo->getInstanceName() << "</componentinstance>" << endl;
        }
      else if (compo && (_componentInstanceMap.find(compo) == _componentInstanceMap.end()))
        {
          _out << indent(depth+1) << compo->getFileRepr() << endl;
          _componentInstanceMap[compo] = _root->getChildName(node);
          Container *cont = compo->getContainer();
          if (cont)
            {
              map<string, Container*>::const_iterator it;
              for (it = _containerMap.begin(); it != _containerMap.end(); ++it)
                {
                  if (it->second == cont) break;
                }
              if (it != _containerMap.end())
                _out << indent(depth+1) << "<load container=\"" << it->first << "\"/>" << endl;
            }
        }
      else if(compo)
        {
          _out << indent(depth+1) << "<node>" << _componentInstanceMap[compo] << "</node>" << endl;
        }
      else
        {
          _out << indent(depth+1) << "<component>" << "UNKNOWN" << "</component>" << endl;
        }
    }
  _out << indent(depth+1) << "<method>" << node->getMethod() << "</method>" << endl;

  writeProperties(node);
  writeInputPorts(node);
  writeInputDataStreamPorts(node);
  writeOutputPorts(node);
  writeOutputDataStreamPorts(node);
  _out << indent(depth) << "</service>" << endl;
  endCase(node);
  DEBTRACE("END visitServiceNode " << _root->getChildName(node));
}

void VisitorSaveSchema::visitServerNode(ServerNode *node)
{
  DEBTRACE("START visitServerNode " << _root->getChildName(node));
  beginCase(node);
  int depth = depthNode(node);
  _out << indent(depth) << "<server name=\"" << node->getName() << "\"";
  if (node->getState() == YACS::DISABLED)
    _out << " state=\"disabled\">" << endl;
  else
    _out << ">" << endl;
  Container *cont = node->getContainer();
  map<string, Container*>::const_iterator it;
  for (it = _containerMap.begin(); it != _containerMap.end(); ++it)
    {
      if (it->second == cont) break;
    }
  if (it != _containerMap.end())
    _out << indent(depth+1) << "<loadcontainer>" << it->first << "</loadcontainer>" << endl;
  /*else
    {
      _out << indent(depth+1) << "<node>" << _contnentInstanceMap[cont] << "</node>" << endl;
      }*/
  _out << indent(depth+1) << "<method>" << node->getFname() << "</method>" << endl;
  _out << indent(depth+2) << "<script><code><![CDATA[";
  _out << node->getScript();
  _out << "]]></code></script>" << endl;
  //_out << indent(depth+1) << "</function>" << endl;
  writeProperties(node);
  writeInputPorts(node);
  writeInputDataStreamPorts(node);
  writeOutputPorts(node);
  writeOutputDataStreamPorts(node);
  _out << indent(depth) << "</server>" << endl;
  endCase(node);
  DEBTRACE("END visitServerNode " << _root->getChildName(node));
}

void VisitorSaveSchema::visitServiceInlineNode(ServiceInlineNode *node)
{
  DEBTRACE("START visitServiceInlineNode " << _root->getChildName(node));
  beginCase(node);
  int depth = depthNode(node);
  _out << indent(depth) << "<serviceInline name=\"" << node->getName() << "\"";
  if (node->getState() == YACS::DISABLED)
    _out << " state=\"disabled\">" << endl;
  else
    _out << ">" << endl;
  
  ComponentInstance *compo = node->getComponent();
  if (compo)
    _out << indent(depth+1) << compo->getFileRepr() << endl;

  _out << indent(depth+1) << "<function name=\"" << node->getMethod() << "\">" << endl;
  _out << indent(depth+2) << "<code><![CDATA[";
  _out << node->getScript();
  _out << "]]></code>" << endl;
  _out << indent(depth+1) << "</function>" << endl;
  writeProperties(node);
  writeInputPorts(node);
  writeOutputPorts(node);
  _out << indent(depth) << "</serviceInline>" << endl;
  endCase(node);
  DEBTRACE("END visitServiceInlineNode " << _root->getChildName(node));
}

void VisitorSaveSchema::visitPresetNode(DataNode *node)
{
  DEBTRACE("START visitPresetNode " << _root->getChildName(node));
  DEBTRACE("END visitPresetNode " << _root->getChildName(node));
}

void VisitorSaveSchema::visitOutNode(DataNode *node)
{
  DEBTRACE("START visitOutNode " << _root->getChildName(node));
  DEBTRACE("END visitOutNode " << _root->getChildName(node));
}

void VisitorSaveSchema::visitStudyInNode(DataNode *node)
{
  DEBTRACE("START visitStudyInNode " << _root->getChildName(node));
  DEBTRACE("END visitStudyInNode " << _root->getChildName(node));
}

void VisitorSaveSchema::visitStudyOutNode(DataNode *node)
{
  DEBTRACE("START visitStudyOutNode " << _root->getChildName(node));
  DEBTRACE("END visitStudyOutNode " << _root->getChildName(node));
}


void VisitorSaveSchema::visitSwitch(Switch *node)
{
  DEBTRACE("START visitSwitch " << _root->getChildName(node));
  beginCase(node);
  int depth = depthNode(node);
  AnyInputPort *condition = static_cast<AnyInputPort*>(node->edGetConditionPort());
  _out << indent(depth) << "<switch name=\"" << node->getName() << "\"";
  if (node->getState() == YACS::DISABLED)
    _out << " state=\"disabled\"";
  if (condition->isEmpty())
    _out << ">" << endl;
  else   
    _out << " select=\"" << condition->getIntValue() << "\">" << endl;
  writeProperties(node);
  node->ComposedNode::accept(this);
  writeControls(node);
  writeSimpleDataLinks(node);
  writeSimpleStreamLinks(node);
  _out << indent(depth) << "</switch>" << endl;
  endCase(node);
  DEBTRACE("END visitSwitch " << _root->getChildName(node));
}

void VisitorSaveSchema::visitWhileLoop(WhileLoop *node)
{
  DEBTRACE("START visitWhileLoop " << _root->getChildName(node));
  beginCase(node);
  int depth = depthNode(node);
  _out << indent(depth) << "<while name=\"" << node->getName() << "\"";
  if (node->getState() == YACS::DISABLED)
    _out << " state=\"disabled\">" << endl;
  else
    _out << ">" << endl;
  writeProperties(node);
  node->ComposedNode::accept(this);
  writeSimpleDataLinks(node);
  writeSimpleStreamLinks(node);
  _out << indent(depth) << "</while>" << endl;
  endCase(node);
  DEBTRACE("END visitWhileLoop " << _root->getChildName(node));
}


void VisitorSaveSchema::writeProperties(Node *node)
{
  int depth = depthNode(node)+1;
  map<string,string> properties = getNodeProperties(node);
  map<string,string>::const_iterator it;
  for(it = properties.begin(); it != properties.end(); ++it)
    {
      _out << indent(depth) << "<property name=\"" << it->first
           << "\" value=\"" << it->second << "\"/>" << endl;
    }
}

void VisitorSaveSchema::dumpTypeCode(TypeCode* type, std::set<std::string>& typeNames,std::map<std::string, TypeCode*>& typeMap,int depth)
{
  DynType kind = type->kind();
  string typeName = type->name();
  if (typeNames.find(typeName) != typeNames.end())
    return;
  switch(kind)
    {
    case YACS::ENGINE::Double:
      {
        typeNames.insert(typeName);
        _out << indent(depth) << "<type name=\"" << typeName << "\" kind=\"double\"/>" << endl;
        break;
      }
    case YACS::ENGINE::Int:
      {
        typeNames.insert(typeName);
        _out << indent(depth) << "<type name=\"" << typeName << "\" kind=\"int\"/>" << endl;
        break;
      }
    case YACS::ENGINE::String:
      {
        typeNames.insert(typeName);
        _out << indent(depth) << "<type name=\"" << typeName << "\" kind=\"string\"/>" << endl;
        break;
      }
    case YACS::ENGINE::Bool:
      {
        typeNames.insert(typeName);
        _out << indent(depth) << "<type name=\"" << typeName << "\" kind=\"bool\"/>" << endl;
        break;
      }
    case YACS::ENGINE::Objref:
      {
        TypeCodeObjref *objref = dynamic_cast<TypeCodeObjref*>(type);
        std::list<TypeCodeObjref*> listOfBases = getListOfBases(objref);
        //try to dump base classes
        for(std::list<TypeCodeObjref*>::iterator il=listOfBases.begin(); il != listOfBases.end(); ++il)
          {
            if (typeNames.find((*il)->name()) == typeNames.end())
              dumpTypeCode((*il),typeNames,typeMap,depth);
          }
        //effective dump
        typeNames.insert(typeName);
        _out << indent(depth) << "<objref name=\"" << typeName << "\" id=\""
             << objref->id() << "\"";
        if (listOfBases.empty())
          _out << "/>" << endl;
        else
          {
            _out << ">" << endl;
            for(std::list<TypeCodeObjref*>::iterator il=listOfBases.begin(); il != listOfBases.end(); ++il)
              {
                _out << indent(depth+1) << "<base>";
                _out << (*il)->name();
                _out << "</base>" << endl;
              }
            _out << indent(depth) << "</objref>" << endl;
          }
        break;
      }
    case YACS::ENGINE::Sequence:
      {
        TypeCode* content = (TypeCode*)type->contentType();
        if (typeNames.find(content->name()) == typeNames.end())
          {
            //content type not dumped
            dumpTypeCode(content,typeNames,typeMap,depth);
          }
        typeNames.insert(typeName);
        _out << indent(depth) << "<sequence name=\"" << typeName << "\" content=\""
             << content->name() <<  "\"/>" << endl;
        break;
      }
    case YACS::ENGINE::Array:
      {
        TypeCode* content = (TypeCode*)type->contentType();
        if (typeNames.find(content->name()) == typeNames.end())
          {
            //content type not dumped
            dumpTypeCode(content,typeNames,typeMap,depth);
          }
        typeNames.insert(typeName);
        _out << indent(depth) << "<array name=\"" << typeName << "\" content=\""
             << content->name() <<  "\"/>" << endl;
        break;
      }
    case YACS::ENGINE::Struct:
      {
        TypeCodeStruct* tcStruct = dynamic_cast<TypeCodeStruct*>(type);
        YASSERT(tcStruct);
        int mbCnt = tcStruct->memberCount();
        for (int i=0; i<mbCnt; i++)
          {
            TypeCode* member = tcStruct->memberType(i);
            if (typeNames.find(member->name()) == typeNames.end())
              {
                //content type not dumped
                dumpTypeCode(member,typeNames,typeMap,depth);
              }            
          }
        typeNames.insert(typeName);
        _out << indent(depth) << "<struct name=\"" << typeName << "\">" << endl;
        for (int i=0; i<mbCnt; i++)
          {
            TypeCode* member = tcStruct->memberType(i);
            _out << indent(depth+1) << "<member name=\"" <<tcStruct->memberName(i) << "\" type=\"" << member->name() << "\"/>" << endl;
          }
        _out << indent(depth) << "</struct>" << endl;
        break;
      }
    default:
      {
        string what = "wrong TypeCode: "; 
        throw Exception(what);
      }
    }
}

void VisitorSaveSchema::writeTypeCodes(Proc *proc)
{
  int depth = depthNode(proc)+1;
  map<string, TypeCode*> typeMap = getTypeCodeMap(proc);
  map<string, TypeCode*>::const_iterator it;
  set<string> typeNames;

  // --- force definition of simple types first

  for (it = typeMap.begin(); it != typeMap.end(); it++)
    {
      dumpTypeCode(it->second,typeNames,typeMap,depth);
    }
}

void VisitorSaveSchema::writeComponentInstances(Proc *proc)
{
  int depth = depthNode(proc)+1;
  std::map<std::string, ComponentInstance*>::const_iterator it;
  for (it = proc->componentInstanceMap.begin(); it != proc->componentInstanceMap.end(); it++)
    {
      string name = it->first;
      ComponentInstance* inst=it->second;
      if(!inst->isAnonymous())
        {
          _out << indent(depth) << "<componentinstance name=\"" << inst->getInstanceName() << "\">" << endl;
          _out << indent(depth+1) << "<component>" << inst->getCompoName() << "</component>" << endl;

          Container *cont = inst->getContainer();
          if (cont)
            _out << indent(depth+1) << "<load container=\"" << cont->getName() << "\"/>" << endl;

          std::map<std::string, std::string> properties = inst->getProperties();
          std::map<std::string, std::string>::const_iterator itm;
          for(itm = properties.begin(); itm != properties.end(); ++itm)
            _out << indent(depth+1) << "<property name=\"" << (*itm).first
                 << "\" value=\"" << (*itm).second << "\"/>" << endl;

          _out << indent(depth) << "</componentinstance>" << endl;
        }
    }
}

void VisitorSaveSchema::writeContainers(Proc *proc)
{
  int depth = depthNode(proc)+1;
  _containerMap = getContainerMap(proc);
  map<string, Container*>::const_iterator it; 
  for (it = _containerMap.begin(); it != _containerMap.end(); it++)
    {
      string name = it->first;
      _out << indent(depth) << "<container name=\"" << name << "\">" << endl;
      _out << indent(depth+1) << "<property name=\"" << Container::KIND_ENTRY << "\" value=\"" << it->second->getKind() << "\"/>" << endl;
      _out << indent(depth+1) << "<property name=\"" << Container::AOC_ENTRY << "\" value=\"" << (int)it->second->isAttachedOnCloning() << "\"/>" << endl;
      map<string, string> properties = (it->second)->getProperties();
      map<string, string>::iterator itm;
      for(itm = properties.begin(); itm != properties.end(); ++itm)
        {
          if((*itm).first==Container::KIND_ENTRY)
            continue;
          if((*itm).first==Container::AOC_ENTRY)
            continue;
          if((*itm).first!=HomogeneousPoolContainer::INITIALIZE_SCRIPT_KEY)
            {
              _out << indent(depth+1) << "<property name=\"" << (*itm).first << "\" value=\"" << (*itm).second << "\"/>" << endl;
            }
          else
            {
              _out << indent(depth+1) << "<initializescriptkey><code><![CDATA[" << (*itm).second << "]]></code></initializescriptkey>" << endl;
            }
        }
      _out << indent(depth) << "</container>"  << endl;
    }
}


void VisitorSaveSchema::writeInputPorts(Node *node)
{
  int depth = depthNode(node)+1;
  list<InputPort*> listOfInputPorts = node->getSetOfInputPort();
  for (list<InputPort*>::iterator it = listOfInputPorts.begin(); it != listOfInputPorts.end(); ++it)
    {
      _out << indent(depth) << "<inport name=\"" << (*it)->getName() << "\" type=\"" 
           << (*it)->edGetType()->name() << "\"/>" << endl;
    }
}

void VisitorSaveSchema::writeInputDataStreamPorts(Node *node)
{
  int depth = depthNode(node)+1;
  list<InputDataStreamPort*> listOfInputPorts = node->getSetOfInputDataStreamPort();
  for (list<InputDataStreamPort*>::iterator it = listOfInputPorts.begin(); it != listOfInputPorts.end(); ++it)
    {
      std::map<std::string,std::string> aPropMap = (*it)->getProperties();
      if ( aPropMap.empty() )
        _out << indent(depth) << "<instream name=\"" << (*it)->getName() << "\" type=\"" 
             << (*it)->edGetType()->name() << "\"/>" << endl;
      else
        {
          _out << indent(depth) << "<instream name=\"" << (*it)->getName() << "\" type=\"" 
               << (*it)->edGetType()->name() << "\">" << endl;
          for (std::map<std::string,std::string>::iterator itP = aPropMap.begin(); itP != aPropMap.end(); itP++)
            _out << indent(depth+1) << "<property name=\"" << (*itP).first << "\" value=\"" 
                 << (*itP).second << "\"/>" << endl;
          _out << indent(depth) << "</instream>" << endl;
        }
    }
}

void VisitorSaveSchema::writeOutputPorts(Node *node)
{
  int depth = depthNode(node)+1;
  list<OutputPort*> listOfOutputPorts = node->getSetOfOutputPort();
  for (list<OutputPort*>::iterator it = listOfOutputPorts.begin(); it != listOfOutputPorts.end(); ++it)
    {
      _out << indent(depth) << "<outport name=\"" << (*it)->getName() << "\" type=\"" 
           << (*it)->edGetType()->name() << "\"/>" << endl;
    }
}

void VisitorSaveSchema::writeOutputDataStreamPorts(Node *node)
{
  int depth = depthNode(node)+1;
  list<OutputDataStreamPort*> listOfOutputPorts = node->getSetOfOutputDataStreamPort();
  for (list<OutputDataStreamPort*>::iterator it = listOfOutputPorts.begin(); it != listOfOutputPorts.end(); ++it)
    {
      std::map<std::string,std::string> aPropMap = (*it)->getProperties();
      if ( aPropMap.empty() )
        _out << indent(depth) << "<outstream name=\"" << (*it)->getName() << "\" type=\"" 
             << (*it)->edGetType()->name() << "\"/>" << endl;
      else
        {
          _out << indent(depth) << "<outstream name=\"" << (*it)->getName() << "\" type=\"" 
               << (*it)->edGetType()->name() << "\">" << endl;
          for (std::map<std::string,std::string>::iterator itP = aPropMap.begin(); itP != aPropMap.end(); itP++)
            _out << indent(depth+1) << "<property name=\"" << (*itP).first << "\" value=\"" 
                 << (*itP).second << "\"/>" << endl;
          _out << indent(depth) << "</outstream>" << endl;
        }
    }
}

void VisitorSaveSchema::writeControls(ComposedNode *node)
{
  int depth = depthNode(node)+1;
  list<Node*> setOfChildren =  node->edGetDirectDescendants();
  for (list<Node*>::iterator ic = setOfChildren.begin(); ic != setOfChildren.end(); ++ic)
    {
      // --- Control links from direct descendant to nodes inside the bloc
      list<InGate*> setOfInGates = (*ic)->getOutGate()->edSetInGate();
      for (list<InGate*>::iterator ig = setOfInGates.begin(); ig != setOfInGates.end(); ++ig)
        {
          Node *to = (*ig)->getNode();
          if (node->isInMyDescendance(to))
            {
              Node *from = (*ic);
              _out << indent(depth) << "<control> <fromnode>" << node->getChildName(from) << "</fromnode> ";
              _out << "<tonode>" << node->getChildName(to) << "</tonode> </control>" << endl;
            }
        }
      // --- Control links from nodes inside the bloc to direct descendant
      //     avoid links between direct descendants (already done above)
      list<OutGate*> listOfOutGates = (*ic)->getInGate()->getBackLinks();
      for (list<OutGate*>::iterator ig = listOfOutGates.begin(); ig != listOfOutGates.end(); ++ig)
        {
          Node *from = (*ig)->getNode();
          if ((node->isInMyDescendance(from)) && (from->getFather()->getNumId() != node->getNumId()))
            {
              Node *to = *ic;
              _out << indent(depth) << "<control> <fromnode>" << node->getChildName(from) << "</fromnode> ";
              _out << "<tonode>" << node->getChildName(to) << "</tonode> </control>" << endl;
            }
        }
    }
}

/*!
 * Write simple data links from and to direct children of node (grand children already done).
 * First, for all output ports of direct children, write links where the input port is inside
 * the node scope. Keep in memory the links where the input port is outside the node scope.
 * Second, retreive links where the output port is inside the scope, using the links kept in memory
 * and not yet written.
 */
void VisitorSaveSchema::writeSimpleDataLinks(ComposedNode *node)
{
  int depth = depthNode(node)+1;
  list<Node*> setOfChildren =  node->edGetDirectDescendants();

  list<Node*> setOfChildrenPlusSplitters = setOfChildren;

  for (list<Node*>::iterator ic = setOfChildren.begin(); ic != setOfChildren.end(); ++ic)
    // add "splitter" node of ForEachLoop nodes to the set of children
    if ( dynamic_cast<ForEachLoop*>( *ic ) )
      {
        Node *nodeToInsert=(*ic)->getChildByName(ForEachLoop::NAME_OF_SPLITTERNODE);
        if(find(setOfChildrenPlusSplitters.begin(),setOfChildrenPlusSplitters.end(),nodeToInsert)==setOfChildrenPlusSplitters.end())
          setOfChildrenPlusSplitters.push_back(nodeToInsert);
      }
  
  // --- first pass,  write links where the input port is inside the node scope. Keep in memory others.
  
  for (list<Node*>::iterator ic = setOfChildrenPlusSplitters.begin(); ic != setOfChildrenPlusSplitters.end(); ++ic)
    {
      Node* from = *ic;
      list<OutputPort*> listOP = from->getLocalOutputPorts();
      for (list<OutputPort*>::iterator io = listOP.begin(); io != listOP.end(); ++io)
        {
          OutputPort* anOP = *io;
          set<InPort*> setIP = anOP->edSetInPort();
          for (set<InPort*>::iterator iip = setIP.begin(); iip != setIP.end(); ++iip)
            {
              InPort *anIP = *iip;
              Node* to = anIP->getNode();
              DEBTRACE("from " << from->getName() << " outputPort " << anOP->getName()
                       << " to " << to->getName() << " inputPort " << anIP->getName() );
              Node* child = node->isInMyDescendance(to);
              if (child && (child->getNumId() != node->getNumId())
                  && (from->getNumId() != to->getNumId()))
                {
                  DEBTRACE( "BINGO!" );

                  string fromName;
                  if ( dynamic_cast<SplitterNode*>(from) && dynamic_cast<ForEachLoop*>(from->getFather()) )
                    fromName = from->getFather()->getName();
                  else
                    fromName = node->getChildName(from);

                  string childName;
                  if ( dynamic_cast<SplitterNode*>(to) && dynamic_cast<ForEachLoop*>(to->getFather()) )
                    childName = node->getChildName(to->getFather());
                  else
                    childName = node->getChildName(to);
                  _out << indent(depth) << "<datalink control=\"false\">" << endl;
                  _out << indent(depth+1) << "<fromnode>" << fromName << "</fromnode> ";
                  _out << "<fromport>" << anOP->getName() << "</fromport>" << endl;
                  _out << indent(depth+1) << "<tonode>" << childName << "</tonode> ";
                  _out << "<toport>" << anIP->getName() << "</toport>" << endl;
                  _out << indent(depth) << "</datalink>" << endl;
                }
              else
                { // --- store info to create the link later, given the input port
                  DEBTRACE( "For later" );
                  struct DataLinkInfo aLink = { from, to, anOP, anIP, false };
                  _mapOfDLtoCreate.insert(make_pair(anIP->getNumId(), aLink));
                }
            }
        }
    }

  // --- second pass, retreive links where the output port is inside the scope.

  if (!dynamic_cast<Switch*>(node)) // xml parser does not take into account datalinks in a switch context
    {
      std::multimap<int, DataLinkInfo>::iterator pos;
      for (pos = _mapOfDLtoCreate.begin(); pos != _mapOfDLtoCreate.end(); ++pos)
        {
          Node* to = (pos->second).to;
          Node* child = node->isInMyDescendance(to);
          if (child && (child->getNumId() != node->getNumId()))
            {
              InPort* anIP = (pos->second).inp;
              int portId = anIP->getNumId();
              Node* from = (pos->second).from;
              child = node->isInMyDescendance(from);
              if (child && (child->getNumId() != node->getNumId()))
                if((from->getNumId() != to->getNumId()) || dynamic_cast<Loop*>(node))
                  {
                    string childName = node->getChildName(from);
                    OutPort *anOP = (pos->second).outp;
                    (pos->second).toDelete = true;
                    _out << indent(depth) << "<datalink control=\"false\">" << endl;
                    _out << indent(depth+1) << "<fromnode>" << childName << "</fromnode> ";
                    _out << "<fromport>" << anOP->getName() << "</fromport>" << endl;
                    _out << indent(depth+1) << "<tonode>" << node->getChildName(to) << "</tonode> ";
                    _out << "<toport>" << anIP->getName() << "</toport>" << endl;
                    _out << indent(depth) << "</datalink>" << endl;
                  }
            }
        }

      // --- remove the link written from the multimap
      
      for (pos = _mapOfDLtoCreate.begin(); pos != _mapOfDLtoCreate.end();)
        {
          if ((pos->second).toDelete)
            _mapOfDLtoCreate.erase(pos++); // be careful not to saw off the branch on which you are sitting!
          else
            ++pos;
        }
    }
}

void VisitorSaveSchema::writeSimpleStreamLinks(ComposedNode *node)
{
  int depth = depthNode(node)+1;
  list<Node*> setOfChildren =  node->edGetDirectDescendants();

  // --- first pass,  write links where the input port is inside the node scope. Keep in memory others.

  for (list<Node*>::iterator ic = setOfChildren.begin(); ic != setOfChildren.end(); ++ic)
    {
      Node* from = *ic;
      if ( dynamic_cast<ComposedNode*>(from) ) continue;
      list<OutputDataStreamPort*> listOP = from->getSetOfOutputDataStreamPort();
      for (list<OutputDataStreamPort*>::iterator io = listOP.begin(); io != listOP.end(); ++io)
        {
          OutputDataStreamPort* anOP = *io;
          set<InPort*> setIP = anOP->edSetInPort();
          for (set<InPort*>::iterator iip = setIP.begin(); iip != setIP.end(); ++iip)
            {
              InPort *anIP = *iip;
              Node* to = anIP->getNode();
              DEBTRACE("from " << from->getName() << " outputPort " << anOP->getName()
                       << " to " << to->getName() << " inputPort " << anIP->getName() );
              Node* child = node->isInMyDescendance(to);
              if (child && (child->getNumId() != node->getNumId())
                  && (from->getNumId() != to->getNumId()))
                {
                  DEBTRACE( "BINGO!" );
                  string childName = node->getChildName(to);
                  _out << indent(depth) << "<stream>" << endl;
                  _out << indent(depth+1) << "<fromnode>" << node->getChildName(from) << "</fromnode> ";
                  _out << "<fromport>" << anOP->getName() << "</fromport>" << endl;
                  _out << indent(depth+1) << "<tonode>" << childName << "</tonode> ";
                  _out << "<toport>" << anIP->getName() << "</toport>" << endl;

                  std::map<std::string,std::string> aPropMap = dynamic_cast<InputDataStreamPort*>(anIP)->getProperties();
                  for (std::map<std::string,std::string>::iterator itP = aPropMap.begin(); itP != aPropMap.end(); itP++)
                    {
                      string notAlinkProperty = "DependencyType";
                      if (notAlinkProperty != (*itP).first)
                        _out << indent(depth+1) << "<property name=\"" << (*itP).first << "\" value=\"" 
                             << (*itP).second << "\"/>" << endl;
                    }
                  _out << indent(depth) << "</stream>" << endl;
                }
              else
                { // --- store info to create the link later, given the input port
                  DEBTRACE("For later" );
                  struct StreamLinkInfo aLink = { from, to, anOP, anIP, false };
                  _mapOfSLtoCreate.insert(make_pair(anIP->getNumId(), aLink));
                }
            }
        }
    }

  // --- second pass, retreive links where the output port is inside the scope.

  if (!dynamic_cast<Switch*>(node)) // xml parser does not take into account datalinks in a switch context
    {
      std::multimap<int, StreamLinkInfo>::iterator pos;
      for (pos = _mapOfSLtoCreate.begin(); pos != _mapOfSLtoCreate.end(); ++pos)
        {
          Node* to = (pos->second).to;
          Node* child = node->isInMyDescendance(to);
          if (child && (child->getNumId() != node->getNumId()))
            {
              InPort* anIP = (pos->second).inp;
              int portId = anIP->getNumId();
              Node* from = (pos->second).from;
              child = node->isInMyDescendance(from);
              if (child && (child->getNumId() != node->getNumId()))
                if((from->getNumId() != to->getNumId()) || dynamic_cast<Loop*>(node))
                  {
                    string childName = node->getChildName(from);
                    OutputDataStreamPort *anOP = (pos->second).outp;
                    (pos->second).toDelete = true;
                    _out << indent(depth) << "<stream>" << endl;
                    _out << indent(depth+1) << "<fromnode>" << childName << "</fromnode> ";
                    _out << "<fromport>" << anOP->getName() << "</fromport>" << endl;
                    _out << indent(depth+1) << "<tonode>" << node->getChildName(to) << "</tonode> ";
                    _out << "<toport>" << anIP->getName() << "</toport>" << endl;

                    std::map<std::string,std::string> aPropMap = dynamic_cast<InputDataStreamPort*>(anIP)->getProperties();
                    for (std::map<std::string,std::string>::iterator itP = aPropMap.begin(); itP != aPropMap.end(); itP++)
                      _out << indent(depth+1) << "<property name=\"" << (*itP).first << "\" value=\"" 
                           << (*itP).second << "\"/>" << endl;

                    _out << indent(depth) << "</stream>" << endl;
                  }
            }
        }

      // --- remove the link written from the multimap
      
      for (pos = _mapOfSLtoCreate.begin(); pos != _mapOfSLtoCreate.end();)
        {
          if ((pos->second).toDelete)
            _mapOfSLtoCreate.erase(pos++); // be careful not to saw off the branch on which you are sitting!
          else
            ++pos;
        }
    }
}

std::set<Node*> VisitorSaveSchema::getAllNodes(ComposedNode *node)
{
  set<Node *> ret;
  list< Node *> setOfNode = node->edGetDirectDescendants();
  for(list<Node *>::iterator iter=setOfNode.begin();iter!=setOfNode.end();iter++)
    {
      if ( dynamic_cast<ComposedNode*> (*iter) )
        {
          set<Node *> myCurrentSet = getAllNodes(dynamic_cast<ComposedNode*> (*iter));
          ret.insert(myCurrentSet.begin(),myCurrentSet.end());
          ret.insert(*iter);
        }
      else
        {
          list<ElementaryNode *> myCurrentSet=(*iter)->getRecursiveConstituents();
          ret.insert(myCurrentSet.begin(),myCurrentSet.end());
          ret.insert(*iter);
        }
    }
  return ret;
}

void VisitorSaveSchema::writeParameters(Proc *proc)
{
  //  set<Node*> nodeSet = proc->getAllRecursiveConstituents();
  set<Node*> nodeSet = getAllNodes(proc);
  for (set<Node*>::iterator iter = nodeSet.begin(); iter != nodeSet.end(); ++iter)
    {
      //       ElementaryNode *node = dynamic_cast<ElementaryNode*>(*iter);
      //       if (node)
      //         writeParametersNode(proc,node);
      writeParametersNode(proc,(*iter));
    }
}

void VisitorSaveSchema::writeParametersNode(ComposedNode *proc, Node *node)
{
  int depth = 1;
  list<InputPort *> setOfInputPort = node->getLocalInputPorts();
  list<InputPort *>::iterator iter;
  for(iter = setOfInputPort.begin(); iter != setOfInputPort.end(); ++iter)
    {
      if (!(*iter)->isEmpty())
        {
          _out << indent(depth) << "<parameter>" << endl;
          _out << indent(depth+1) << "<tonode>" << proc->getChildName(node) << "</tonode>";
          _out << "<toport>" << (*iter)->getName() << "</toport>" <<endl;
          try
            {
              _out << indent(depth+1) << (*iter)->dump();
            }
          catch (YACS::Exception &e)
            {
              _out << "<value><error><![CDATA[" << e.what() << "]]></error></value>" << endl;
            }
          _out << indent(depth) << "</parameter>" << endl;
        }
    }
}

void VisitorSaveSchema::beginCase(Node* node)
{
  Switch *myFather =dynamic_cast<Switch*>(node->getFather());
  if (myFather)
    {
      int depth = depthNode(node) -1;
      int myCase = myFather->getRankOfNode(node);
      if (myCase == Switch::ID_FOR_DEFAULT_NODE)
        _out << indent(depth) << "<default>" << endl;
      else
        _out << indent(depth) << "<case id=\"" << myCase << "\">" << endl;
    }
}

void VisitorSaveSchema::endCase(Node* node)
{
  Switch *myFather =dynamic_cast<Switch*>(node->getFather());
  if (myFather)
    {
      int depth = depthNode(node) -1;
      int myCase = myFather->getRankOfNode(node);
      if (myCase == Switch::ID_FOR_DEFAULT_NODE)
        _out << indent(depth) << "</default>" << endl;
      else
        _out << indent(depth) << "</case>" << endl;
    }
}

int VisitorSaveSchema::depthNode(Node* node)
{
  int depth = 0;
  ComposedNode *father = node->getFather();
  while (father)
    {
      depth +=1;
      if (dynamic_cast<Switch*>(father)) depth +=1;
      if (father->getNumId() == _root->getNumId()) break;
      father = father->getFather();
    }
  return depth;
}

SchemaSave::SchemaSave(Proc* proc): _p(proc)
{
  YASSERT(_p);
}

void SchemaSave::save(std::string xmlSchemaFile)
{
  VisitorSaveSchema vss(_p);
  vss.openFileSchema(xmlSchemaFile);
  _p->accept(&vss);
  vss.closeFileSchema();
}
