// Copyright (C) 2006-2024  CEA, EDF
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

#ifndef _FORMCONTAINERBASE_HXX_
#define _FORMCONTAINERBASE_HXX_

#include "ui_FormParamContainer.h"

#include <QString>

namespace YACS
{
  namespace ENGINE
  {
    class Container;
  }
}

class FormAdvParamContainer;

class FormContainerBase : public QWidget, public Ui::fm_paramcontainer
{
  Q_OBJECT

public:
  FormContainerBase(QWidget *parent = 0);
  virtual ~FormContainerBase();
  virtual void FillPanel(YACS::ENGINE::Container *container);
  virtual void onModified();
  virtual bool onApply();
  virtual void onCancel();
  virtual QString getTypeStr() const = 0;
public slots:
  void on_ch_advance_stateChanged(int state);
  void onModifyName(const QString &text);

protected:
  bool _advanced;
  YACS::ENGINE::Container *_container;
  std::map<std::string, std::string> _properties;
  FormAdvParamContainer *_advancedParams;
};

#endif
