
#include "EditionControlLink.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

EditionControlLink::EditionControlLink(Subject* subject,
                                       QWidget* parent,
                                       const char* name)
  : ItemEdition(subject, parent, name)
{
}

EditionControlLink::~EditionControlLink()
{
}

