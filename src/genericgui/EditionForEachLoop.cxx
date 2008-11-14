
#include "EditionForEachLoop.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

EditionForEachLoop::EditionForEachLoop(Subject* subject,
                                       QWidget* parent,
                                       const char* name)
  : EditionNode(subject, parent, name)
{
}

EditionForEachLoop::~EditionForEachLoop()
{
}

