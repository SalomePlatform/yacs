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

#ifndef _FORMCONTAINERDECORATOR_HXX_
#define _FORMCONTAINERDECORATOR_HXX_

#include <QWidget>
#include <QIcon>

#include "ui_FormContainer.h"

class FormContainerBase;

#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class Container;
  }
}

class FormContainerDecorator : public QWidget, public Ui::fm_container
{
  Q_OBJECT
public:
  FormContainerDecorator(YACS::ENGINE::Container *cont, QWidget *parent = 0);
  ~FormContainerDecorator();
  void FillPanel(YACS::ENGINE::Container *container);
  QWidget *getWidget();
  bool onApply();
  void onCancel();
  void show();
  void hide();
  void synchronizeCheckContainer();
  std::string getHostName(int index) const;
  void setName(const std::string& name);
public:
  static bool CHECKED;
public slots:
  void on_tb_container_toggled(bool checked);
  void onResMousePressed();
  void onResActivated(int);
  void onContToggled(bool);
signals:
  void typeOfContainerIsKnown(const QString& typeOfCont);
  void resourceMousePressed();
  void resourceActivated(int);
  void containerToggled(bool);//connect(_wContainer->tb_container, SIGNAL(toggled(bool)), this, SLOT(fillContainerPanel())); // --- to update display of current selection
private:
  bool checkOK() const;
  void checkAndRepareTypeIfNecessary(YACS::ENGINE::Container *container);
  void connectForTyp();
private:
  FormContainerBase *_typ;
  QIcon _icon;
};

#endif
