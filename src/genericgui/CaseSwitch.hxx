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

#ifndef _CASESWITCH_HXX_
#define _CASESWITCH_HXX_

#include "ui_CaseSwitch.h"

namespace YACS
{
  namespace HMI
  {
    class CaseSwitch: public QWidget, public Ui::CaseSwitch
    {
      Q_OBJECT

    public slots:
      void on_cb_default_stateChanged(int state);

    public:
      CaseSwitch(QWidget *parent = 0);
      ~CaseSwitch();
      bool isDefaultChecked();
      void setDefaultChecked(bool isDefault);

    protected:
      bool _isDefault;

    };
  }
}

#endif
