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

#include "PresetPorts.hxx"
#include "VisitorSaveSalomeSchema.hxx"
#include "DataNode.hxx"
#include "PresetNode.hxx"
#include "OutNode.hxx"
#include "StudyNodes.hxx"
#include "ComposedNode.hxx"
#include "StudyPorts.hxx"
#include "TypeCode.hxx"

#include <cassert>
#include <iostream>
#include <string>
#include <set>


using namespace YACS::ENGINE;
using namespace std;

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

VisitorSaveSalomeSchema::VisitorSaveSalomeSchema(ComposedNode *root)
  : VisitorSaveSchema(root)
{
}

VisitorSaveSalomeSchema::~VisitorSaveSalomeSchema()
{
}

void VisitorSaveSalomeSchema::visitPresetNode(DataNode *node)
{
  DEBTRACE("START visitPresetNode " << _root->getChildName(node));
  beginCase(node);
  int depth = depthNode(node);
  _out << indent(depth) << "<datanode name=\"" << node->getName() << "\"";
  string ref = node->getRef();
  if (!ref.empty()) _out << " ref=\"" << ref << "\"";
  if (node->getState() == YACS::DISABLED)
    _out << " state=\"disabled\">" << endl;
  else
    _out << ">" << endl;
  writeDataNodeParameters(node);
  writeProperties(node);
  _out << indent(depth) << "</datanode>" << endl;
  endCase(node);
  DEBTRACE("END visitPresetNode " << _root->getChildName(node));
}

void VisitorSaveSalomeSchema::visitOutNode(DataNode *node)
{
  DEBTRACE("START visitOutNode " << _root->getChildName(node));
  beginCase(node);
  int depth = depthNode(node);
  _out << indent(depth) << "<outnode name=\"" << node->getName() << "\"";
  string ref = node->getRef();
  if (!ref.empty()) _out << " ref=\"" << ref << "\"";
  if (node->getState() == YACS::DISABLED)
    _out << " state=\"disabled\">" << endl;
  else
    _out << ">" << endl;
  writeOutNodeParameters(node);
  writeProperties(node);
  _out << indent(depth) << "</outnode>" << endl;
  endCase(node);
  DEBTRACE("END visitOutNode " << _root->getChildName(node));
}

void VisitorSaveSalomeSchema::visitStudyInNode(DataNode *node)
{
  DEBTRACE("START visitStudyInNode " << _root->getChildName(node));
  beginCase(node);
  int depth = depthNode(node);
  _out << indent(depth) << "<datanode name=\"" << node->getName() << "\" kind=\"study\"";
  string ref = node->getRef();
  if (!ref.empty()) _out << " ref=\"" << ref << "\"";
  if (node->getState() == YACS::DISABLED)
    _out << " state=\"disabled\">" << endl;
  else
    _out << ">" << endl;
  writeStudyInNodeParameters(node);
  writeProperties(node);
  _out << indent(depth) << "</datanode>" << endl;
  endCase(node);
  DEBTRACE("END visitStudyInNode " << _root->getChildName(node));
}

void VisitorSaveSalomeSchema::visitStudyOutNode(DataNode *node)
{
  DEBTRACE("START visitStudyOutNode " << _root->getChildName(node));
  beginCase(node);
  int depth = depthNode(node);
  _out << indent(depth) << "<outnode name=\"" << node->getName() << "\" kind=\"study\"";
  string ref = node->getRef();
  if (!ref.empty()) _out << " ref=\"" << ref << "\"";
  if (node->getState() == YACS::DISABLED)
    _out << " state=\"disabled\">" << endl;
  else
    _out << ">" << endl;
  writeStudyOutNodeParameters(node);
  writeProperties(node);
  _out << indent(depth) << "</outnode>" << endl;
  endCase(node);
  DEBTRACE("END visitStudyOutNode " << _root->getChildName(node));
}

void VisitorSaveSalomeSchema::writeDataNodeParameters(DataNode *node)
{
  int depth = depthNode(node)+1;
  list<OutputPort*> listOfOutputPorts = node->getSetOfOutputPort();
  for (list<OutputPort*>::iterator it = listOfOutputPorts.begin(); it != listOfOutputPorts.end(); ++it)
    {
      _out << indent(depth) << "<parameter name=\"" << (*it)->getName() << "\" type=\"" 
           << (*it)->edGetType()->name() << "\">" << endl;
      OutputPresetPort *outp = dynamic_cast<OutputPresetPort*>(*it);
      YASSERT(outp);
      _out << indent(depth+1) << outp->getData();
      _out << indent(depth) << "</parameter>" << endl;
    }
}

void VisitorSaveSalomeSchema::writeStudyInNodeParameters(DataNode *node)
{
  int depth = depthNode(node)+1;
  list<OutputPort*> listOfOutputPorts = node->getSetOfOutputPort();
  for (list<OutputPort*>::iterator it = listOfOutputPorts.begin(); it != listOfOutputPorts.end(); ++it)
    {
      _out << indent(depth) << "<parameter name=\"" << (*it)->getName() << "\" type=\"" 
           << (*it)->edGetType()->name() << "\" ref=\"";
      OutputStudyPort *outp = dynamic_cast<OutputStudyPort*>(*it);
      YASSERT(outp);
      _out << outp->getData() << "\"/>" << endl;
    }
}

void VisitorSaveSalomeSchema::writeOutNodeParameters(DataNode *node)
{
  int depth = depthNode(node)+1;
  list<InputPort*> listOfInputPorts = node->getSetOfInputPort();
  for (list<InputPort*>::iterator it = listOfInputPorts.begin(); it != listOfInputPorts.end(); ++it)
    {
      _out << indent(depth) << "<parameter name=\"" << (*it)->getName() << "\" type=\"" 
           << (*it)->edGetType()->name() << "\"/>" << endl;
    }
}

void VisitorSaveSalomeSchema::writeStudyOutNodeParameters(DataNode *node)
{
  int depth = depthNode(node)+1;
  list<InputPort*> listOfInputPorts = node->getSetOfInputPort();
  for (list<InputPort*>::iterator it = listOfInputPorts.begin(); it != listOfInputPorts.end(); ++it)
    {
      _out << indent(depth) << "<parameter name=\"" << (*it)->getName() << "\" type=\"" 
           << (*it)->edGetType()->name() << "\" ref=\"";
      InputStudyPort *inp = dynamic_cast<InputStudyPort*>(*it);
      YASSERT(inp);
      _out << inp->getData() << "\"/>" << endl;
    }
}
