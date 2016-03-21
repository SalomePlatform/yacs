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

#ifndef _FORMADVPARAMCONTAINER_HXX_
#define _FORMADVPARAMCONTAINER_HXX_

#include "ui_FormAdvParamContainer.h"

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

class FormAdvParamContainer : public QWidget, public Ui::fm_advparamcontainer
{
  Q_OBJECT

public:
  FormAdvParamContainer(std::map<std::string, std::string>& prop, QWidget *parent = 0);
  virtual ~FormAdvParamContainer();

  void FillPanel(const std::string& resource, YACS::ENGINE::Container *container);
  virtual void onModified();
  void updateResource(const std::string &resource);
  void setModeText(const std::string& mode);

public slots:
//  void onModifyName(const QString &text);
  void onModifyResource(const QString &text);
  void onModifyPolicy(const QString &text);
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
  void onModifyProcPar(const QString &text);
  void onModifyResourceName(const QString &text);
  void onModifyHostName(const QString &text);
  void onModifyProcRes(const QString &text);
  void onModifyCompoList(const QString &text);
  void onModifyResourceList(const QString &text);


protected:
  bool _advanced;
  YACS::ENGINE::Container *_container;
  std::map<std::string, std::string>& _properties;

private:
};

#endif
