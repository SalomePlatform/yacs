
#include "EditionLoop.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

EditionLoop::EditionLoop(Subject* subject,
                         QWidget* parent,
                         const char* name)
  : EditionNode(subject, parent, name)
{
}

EditionLoop::~EditionLoop()
{
}

