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
#ifndef _FORMCONTAINER_HXX_
#define _FORMCONTAINER_HXX_

#include "ui_FormContainer.h"

#include <QIcon>
#include <map>
#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class Container;
  }
}

class FormContainer: public QWidget, public Ui::fm_container
{
  Q_OBJECT

public:
  FormContainer(QWidget *parent = 0);
  virtual ~FormContainer();

  void FillPanel(YACS::ENGINE::Container *container);
  virtual void onModified();
  virtual bool onApply();

public:
  static bool _checked;

public slots:
  void on_tb_container_toggled(bool checked);
  void on_ch_advance_stateChanged(int state);
  void onModifyName(const QString &text);
  void onModifyHost(const QString &text);
  void onModifyPolicy(const QString &text);
  void onModifyType(const QString &text);
  void onModifyWorkDir(const QString &text);
  void onModifyContName(const QString &text);
  void onModifyOS(const QString &text);
  void onModifyParLib(const QString &text);
  void onModifyIsMPI(bool isMpi);
  void onModifyMem(const QString &text);
  void onModifyClock(const QString &text);
  void onModifyNodes(const QString &text);
  void onModifyProcs(const QString &text);
  void onModifyCompos(const QString &text);


protected:
  bool _advanced;
  YACS::ENGINE::Container *_container;
  std::map<std::string, std::string> _properties;

private:
};

#endif
