
#include "VisitorSaveSalomeSchema.hxx"
#include "DataNode.hxx"
#include "PresetNode.hxx"
#include "OutNode.hxx"
#include "StudyNodes.hxx"
#include "ComposedNode.hxx"
#include "PresetPorts.hxx"
#include "StudyPorts.hxx"

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
  _out << indent(depth) << "</datanode>" << endl;
  endCase(node);
  DEBTRACE("END visitStudyInNode " << _root->getChildName(node));
}

void VisitorSaveSalomeSchema::visitStudyOutNode(DataNode *node)
{
  DEBTRACE("START visitStudyOutNode " << _root->getChildName(node));
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
           << (*it)->edGetType()->name() << "\">" << endl;
      _out << indent(depth) << "</parameter>" << endl;
//       InputPresetPort *inp = dynamic_cast<InputPresetPort*>(*it);
//       assert(inp);
    }
}
