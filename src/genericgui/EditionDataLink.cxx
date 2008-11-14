
#include "EditionDataLink.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

EditionDataLink::EditionDataLink(Subject* subject,
                                 QWidget* parent,
                                 const char* name)
  : ItemEdition(subject, parent, name)
{
}

EditionDataLink::~EditionDataLink()
{
}

