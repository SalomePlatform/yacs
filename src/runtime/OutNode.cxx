
#include "OutNode.hxx"
#include "PresetPorts.hxx"

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
}

void OutNode::setData(InputPort* port, std::string& data)
{
  InputPresetPort *inp = dynamic_cast<InputPresetPort *>(port);
  inp->setData(data);
}

void OutNode::checkBasicConsistency() const throw(Exception)
{
  if (! _setOfOutputPort.empty())
    {
      string what = "OutNode ";
      what += getName();
      what += " only accepts InputPorts, no OutputPorts";
      throw Exception(what);
    }
}

Node *OutNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new OutNode(*this,father);
}

    }
}

