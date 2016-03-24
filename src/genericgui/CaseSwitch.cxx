// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "CaseSwitch.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

CaseSwitch::CaseSwitch(QWidget *parent)
{
  DEBTRACE("CaseSwitch::CaseSwitch");
  setParent(parent);
  setupUi(this);
  _isDefault = false;
}

CaseSwitch::~CaseSwitch()
{
  DEBTRACE("CaseSwitch::~CaseSwitch");
}

void CaseSwitch::on_cb_default_stateChanged(int state)
{
  DEBTRACE("CaseSwitch::on_cb_default_stateChanged " << state)
  if (state == Qt::Unchecked)
    setDefaultChecked(false);
  else
    setDefaultChecked(true);
}

bool CaseSwitch::isDefaultChecked()
{
  return _isDefault;
}

void CaseSwitch::setDefaultChecked(bool isDefault)
{
  DEBTRACE("CaseSwitch::setDefaultChecked " << isDefault)
  _isDefault = isDefault;
  sb_case->setVisible(!_isDefault);
  cb_default->setChecked(isDefault);
}
