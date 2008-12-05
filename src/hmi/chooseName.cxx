//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "chooseName.h"
#include "guiObservers.hxx"
#include <qlabel.h>
#include <qlineedit.h>

using namespace std;

using namespace YACS;
using namespace YACS::HMI;
//#define _DEVDEBUG_

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
