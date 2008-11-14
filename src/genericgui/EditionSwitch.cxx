
#include "EditionSwitch.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

EditionSwitch::EditionSwitch(Subject* subject,
                             QWidget* parent,
                             const char* name)
  : EditionBloc(subject, parent, name)
{
}

EditionSwitch::~EditionSwitch()
{
}

