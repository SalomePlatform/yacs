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

#ifndef _FORMEDITITEM_HXX_
#define _FORMEDITITEM_HXX_

#include <QScrollArea>

#include "ui_FormEditItem.h"

class WidEditItem: public QWidget, public Ui::fm_editItem
{
  Q_OBJECT

public:
  WidEditItem(QWidget *parent = 0);
  virtual ~WidEditItem();

public slots:

protected:

private:
};

class FormEditItem: public QScrollArea
{
  Q_OBJECT

public:
  FormEditItem(QWidget *parent = 0);
  virtual ~FormEditItem();

public slots:

protected:
  WidEditItem* _wid;

private:
};

#endif
