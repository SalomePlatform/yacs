
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
  writeProperties(node);
  writeDataNodeParameters(node);
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
  writeProperties(node);
  writeOutNodeParameters(node);
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
  writeProperties(node);
  writeStudyInNodeParameters(node);
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
  writeProperties(node);
  writeStudyOutNodeParameters(node);
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
      assert(outp);
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
      assert(outp);
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
      assert(inp);
      _out << inp->getData() << "\"/>" << endl;
    }
}
