
#include "chooseName.h"
#include "guiObservers.hxx"
#include <qlabel.h>
#include <qlineedit.h>

using namespace std;

using namespace YACS;
using namespace YACS::HMI;
#define _DEVDEBUG_

#include "YacsTrace.hxx"

#include <string>


ChooseName::ChooseName( std::string parentName,
                        std::string typeName,
                        std::string defaultName,
                        QWidget* parent,
                        const char* name,
                        bool modal,
                        WFlags fl )
  : diChooseName(parent, name, modal, fl)
{
//   _parentName = parentName;
//   _typeName = typeName;
//   _defaultName = defaultName;
  tlParentPath->setText(parentName.c_str());
  tlTypeValue->setText(typeName.c_str());
  leName->setText(defaultName.c_str());
  _choosenName = defaultName;
  _isOk = false;
}

ChooseName::~ChooseName()
{
}

void ChooseName::accept()
{
  _choosenName = leName->text().latin1();
  _isOk = true;
  diChooseName::accept();
}

void ChooseName::reject()
{
  diChooseName::reject();
}
