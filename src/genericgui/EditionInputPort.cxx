
#include "EditionInputPort.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

EditionInputPort::EditionInputPort(Subject* subject,
                                   QWidget* parent,
                                   const char* name)
  : ItemEdition(subject, parent, name)
{
}

EditionInputPort::~EditionInputPort()
{
}

