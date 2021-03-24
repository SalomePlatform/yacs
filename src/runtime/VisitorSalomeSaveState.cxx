// Copyright (C) 2006-2021  CEA/DEN, EDF R&D
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

#include "VisitorSalomeSaveState.hxx"
#include "TypeConversions.hxx"
#include "ForEachLoop.hxx"
#include "Proc.hxx"
#include "Executor.hxx"
#include "AutoLocker.hxx"

#include "YacsTrace.hxx"

using namespace YACS::ENGINE;

VisitorSalomeSaveState::VisitorSalomeSaveState(ComposedNode *root)
:VisitorSaveState(root)
{
}

VisitorSalomeSaveState::~VisitorSalomeSaveState()
{
}

void VisitorSalomeSaveState::visitForEachLoop(ForEachLoop *node)
{
  node->ComposedNode::accept(this);
  if (!_out) throw Exception("No file open for dump state");
  std::string name = _root->getName();
  if (static_cast<ComposedNode*>(node) != _root) name = _root->getChildName(node);
  DEBTRACE("VisitorSaveState::visitForEachLoop ------ " << name);
  _out << "  <node type='forEachLoop'>" << std::endl;
  _out << "    <name>" << name << "</name>" << std::endl;
  _out << "    <state>" << _nodeStateName[node->getState()] << "</state>" << std::endl;
//  VisitorSaveState::visitForEachLoop(node);
  std::list<InputPort *> setOfInputPort = node->getLocalInputPorts();
  std::list<InputPort *>::iterator iter;
  for(iter = setOfInputPort.begin(); iter != setOfInputPort.end(); iter++)
  {
    _out << "    <inputPort>" << std::endl;
    _out << "      <name>" << (*iter)->getName() << "</name>" << std::endl;
    try
      {
        _out << "      ";
        _out << (*iter)->dump();
      }
    catch (YACS::Exception &e)
      {
        DEBTRACE("caught YACS:Exception: " << e.what());
        _out << "<value><error><![CDATA[" << e.what() << "]]></error></value>" << std::endl;
      }
    _out << "    </inputPort>" << std::endl;
  }
  
  StatesForNode state = node->getState();
  if(YACS::LOADED == state ||
    YACS::ACTIVATED == state ||
    YACS::SUSPENDED == state ||
    YACS::EXECFAILED == state ||
    YACS::PAUSE == state ||
    YACS::TORECONNECT == state ||
    YACS::INTERNALERR == state ||
    YACS::FAILED == state ||
    YACS::ERROR == state)
  {
    ForEachLoopPassedData* processedData = node->getProcessedData();
    if(processedData)
    {
      const std::vector<unsigned int>& processedIndexes = processedData->getIds();
      std::vector<SequenceAny *>::const_iterator it_outputs;
      std::vector<std::string>::const_iterator it_names;
      
      for(it_outputs = processedData->getOutputs().begin(), it_names = processedData->getOutputNames().begin();
          it_names != processedData->getOutputNames().end();
          it_outputs++, it_names++)
      {
        _out << "    <loopOutputPort>" << std::endl;
        _out << "      <name>" << (*it_names) << "</name>" << std::endl;
        for(unsigned int i = 0; i < (*it_outputs)->size(); i++)
        {
          AnyPtr value = (*(*it_outputs))[i];
          _out << "      <sample><index>" << processedIndexes[i]<< "</index>";
          if(value)
            _out << convertNeutralXml(value->getType(), value);
          else
            _out << "<value>None</value>";
          _out << "      </sample>" << std::endl;
        }
        _out << "    </loopOutputPort>" << std::endl;
      }
      
      delete processedData;
      processedData = NULL;
    }
  }
  _out << "  </node>" << std::endl;
}

void YACS::ENGINE::schemaSaveState(Proc* proc,
                                  Executor* exec,
                                  const std::string& xmlSchemaFile)
{
  YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&(exec->getTheMutexForSchedulerUpdate()));
  VisitorSalomeSaveState vss(proc);
  vss.openFileDump(xmlSchemaFile);
  proc->accept(&vss);
  vss.closeFileDump();
}
