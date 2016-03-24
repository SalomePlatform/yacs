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
#include "PresetNode.hxx"
#include "Visitor.hxx"

#include <iostream>
#include <set>
#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

/*! \class YACS::ENGINE::PresetNode
 *  \brief Class for data in node
 *
 * \ingroup Nodes
 */

const char PresetNode::IMPL_NAME[]="XML";

PresetNode::PresetNode(const std::string& name)
  : DataNode(name)
{
  _implementation=IMPL_NAME;
}

PresetNode::PresetNode(const PresetNode& other, ComposedNode *father)
  : DataNode(other, father)
{
}

OutputPort* PresetNode::createOutputPort(const std::string& outputPortName, TypeCode* type)
{
  return new OutputPresetPort(outputPortName, this, type);
}

void PresetNode::execute()
{
  DEBTRACE("+++++++ PresetNode::execute +++++++++++");
  list<OutputPort *>::const_iterator iter;
  for(iter = _setOfOutputPort.begin(); iter != _setOfOutputPort.end(); iter++)
    {
      OutputPresetPort *outp = dynamic_cast<OutputPresetPort *>(*iter);
      YASSERT(outp);
      string data = outp->getData();
      DEBTRACE("data: " << data );
      outp->put(data.c_str());
    }
  DEBTRACE("+++++++ end PresetNode::execute +++++++++++" );
}

void PresetNode::accept(Visitor *visitor)
{
  visitor->visitPresetNode(this);
}

void PresetNode::setData(OutputPort* port, const std::string& data)
{
  OutputPresetPort *outp = dynamic_cast<OutputPresetPort *>(port);
  outp->setData(data);
}

void PresetNode::checkBasicConsistency() const throw(YACS::Exception)
{
  DEBTRACE("PresetNode::checkBasicConsistency");
  if (! _setOfInputPort.empty())
    {
      string what = "PresetNode ";
      what += getName();
      what += " only accepts OutputPresetPorts, no InputPorts";
      throw Exception(what);
    }

  list<OutputPort *>::const_iterator iter;
  for(iter=_setOfOutputPort.begin();iter!=_setOfOutputPort.end();iter++)
    {
      OutputPresetPort *preset = dynamic_cast<OutputPresetPort*>(*iter);
      if (!preset)
        {
          string what("Output port: ");
          what += (*iter)->getName();
          what += " is not an OutputPresetPort. PresetNode ";
          what += getName();
          what += "only accepts OutputPresetPorts";
          throw Exception(what);
        }
      preset->checkBasicConsistency();
    }
}

Node *PresetNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new PresetNode(*this,father);
}
