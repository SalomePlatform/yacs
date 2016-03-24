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
#include "OutNode.hxx"
#include "Visitor.hxx"

#include <iostream>
#include <fstream>
#include <list>
#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

namespace YACS
{
  namespace ENGINE
    {

/*! \class YACS::ENGINE::OutNode
 *  \brief Class for data out node
 *
 * \ingroup Nodes
 */

const char OutNode::IMPL_NAME[]="XML";

OutNode::OutNode(const std::string& name)
  : DataNode(name)
{
  _implementation=IMPL_NAME;
}

OutNode::OutNode(const OutNode& other, ComposedNode *father)
  : DataNode(other, father)
{
}

InputPort* OutNode::createInputPort(const std::string& inputPortName, TypeCode* type)
{
  return new InputPresetPort(inputPortName, this, type);
}

void OutNode::dump(std::ostream &out)
{
  std::list<InputPort *>::const_iterator iter;
  for(iter = _setOfInputPort.begin(); iter != _setOfInputPort.end(); iter++)
    {
      InputPresetPort *inp = dynamic_cast<InputPresetPort *>(*iter);
      if(inp->getData() != "")
        {
          //save the file in the given reference
          std::string xmlValue=inp->dump();
          std::string::size_type i=xmlValue.find_first_of('/',0);
          xmlValue=xmlValue.substr(i);
          i=xmlValue.find_first_of('<',0);
          std::ifstream fin(xmlValue.substr(0,i).c_str());
          std::ofstream fout(inp->getData().c_str());
          fout << fin.rdbuf(); // Dumps file contents to file
          out << "<value><objref>" << inp->getData() << "</objref></value>" << std::endl;
        }
      else
        out << inp->dump() << std::endl;
    }
}

void OutNode::execute()
{
  DEBTRACE("+++++++ OutNode::execute +++++++++++");
  if(_ref != "")
    {
      std::ofstream out(_ref.c_str());
      dump(out);
    }
  else
    dump(std::cout);
  DEBTRACE("+++++++ end OutNode::execute +++++++++++" );
}

void OutNode::accept(Visitor *visitor)
{
  visitor->visitOutNode(this);
}

void OutNode::setData(InputPort* port, const std::string& data)
{
  InputPresetPort *inp = dynamic_cast<InputPresetPort *>(port);
  inp->setData(data);
}

void OutNode::checkBasicConsistency() const throw(YACS::Exception)
{
  DEBTRACE("OutNode::checkBasicConsistency");
  if (! _setOfOutputPort.empty())
    {
      string what = "OutNode ";
      what += getName();
      what += " only accepts InputPorts, no OutputPorts";
      throw Exception(what);
    }
  list<InputPort *>::const_iterator iter;
  for(iter=_setOfInputPort.begin();iter!=_setOfInputPort.end();iter++)
    {
      InputPresetPort *preset = dynamic_cast<InputPresetPort*>(*iter);
      if (!preset)
        {
          string what("Input port: ");
          what += (*iter)->getName();
          what += " is not an InputPresetPort. PresetNode ";
          what += getName();
          what += " only accepts InputPresetPorts";
          throw Exception(what);
        }
      preset->checkBasicConsistency();
    }

}

Node *OutNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new OutNode(*this,father);
}

    }
}

