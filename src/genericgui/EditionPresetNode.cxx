
#include "EditionPresetNode.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

EditionPresetNode::EditionPresetNode(Subject* subject,
                                     QWidget* parent,
                                     const char* name)
  : EditionElementaryNode(subject, parent, name)
{
  createTablePorts();
  setEditablePorts(true);
}

EditionPresetNode::~EditionPresetNode()
{
}

bool EditionPresetNode::hasInputPorts()
{
  return false;
}
