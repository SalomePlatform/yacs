
#include "EditionStudyInNode.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

EditionStudyInNode::EditionStudyInNode(Subject* subject,
                                       QWidget* parent,
                                       const char* name)
  : EditionPresetNode(subject, parent, name)
{
}

EditionStudyInNode::~EditionStudyInNode()
{
}

