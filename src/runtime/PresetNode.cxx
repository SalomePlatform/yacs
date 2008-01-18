
#include "PresetNode.hxx"
#include "PresetPorts.hxx"
#include "Visitor.hxx"

#include <iostream>
#include <set>
#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

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
      assert(outp);
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

void PresetNode::setData(OutputPort* port, std::string& data)
{
  OutputPresetPort *outp = dynamic_cast<OutputPresetPort *>(port);
  outp->setData(data);
}

void PresetNode::checkBasicConsistency() const throw(Exception)
{
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
      string data = preset->getData();
      if (data.empty())
        {
          string what("OutputPresetPort: ");
          what += (*iter)->getName();
          what += "is not initialised";
          throw Exception(what);
        }
    }
}

Node *PresetNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new PresetNode(*this,father);
}
