
#include "EditionStudyOutNode.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

EditionStudyOutNode::EditionStudyOutNode(Subject* subject,
                                         QWidget* parent,
                                         const char* name)
  : EditionOutNode(subject, parent, name)
{
}

EditionStudyOutNode::~EditionStudyOutNode()
{
}

