
#include "EditionOutNode.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

EditionOutNode::EditionOutNode(Subject* subject,
                               QWidget* parent,
                               const char* name)
  : EditionElementaryNode(subject, parent, name)
{
  createTablePorts();
  setEditablePorts(true);
}

EditionOutNode::~EditionOutNode()
{
}

bool EditionOutNode::hasOutputPorts()
{
  return false;
}
