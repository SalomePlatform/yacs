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

#include "FormSchemaView.hxx"
#include "GraphicsView.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

FormSchemaView::FormSchemaView(QWidget *parent)
{
  setupUi(this);

  vboxLayout->removeWidget(gv_schema);
  delete gv_schema;
  gv_schema = new GraphicsView(this);
  gv_schema->setObjectName(QString::fromUtf8("gv_schema"));
  vboxLayout->insertWidget(0, gv_schema);

  _prevRotate=0;
  dial->setWrapping(true);
  dial->setRange(0, 359);
  dial->setValue(0);

  _prevHsPos=500;
  hs_position->setRange(0, 999);
  hs_position->setValue(500);

  _prevVsPos=500;
  vs_position->setRange(0, 999);
  vs_position->setValue(500);

  _prevZoom=0;
  vs_zoom->setRange(-100, 100);
  vs_zoom->setValue(0);

}

FormSchemaView::~FormSchemaView()
{
}

void FormSchemaView::on_dial_valueChanged(int val)
{
  DEBTRACE("FormSchemaView::on_dial_valueChanged " << val);
  int delta = val - _prevRotate;
  _prevRotate = val;
  gv_schema->rotate(delta);
}

void FormSchemaView::on_hs_position_valueChanged(int val)
{
  DEBTRACE("FormSchemaView::on_hs_poistion_valueChanged " << val);
  int delta = val - _prevHsPos;
  _prevHsPos = val;
  gv_schema->translate(val,0);
}

void FormSchemaView::on_vs_position_valueChanged(int val)
{
  DEBTRACE("FormSchemaView::on_vs_poistion_valueChanged " << val);
  int delta = val - _prevVsPos;
  _prevVsPos = val;
  gv_schema->translate(0, val);
}

void FormSchemaView::on_vs_zoom_valueChanged(int val)
{
  DEBTRACE("FormSchemaView::on_vs_zoom_valueChanged " << val);
  int delta = val - _prevZoom;
  double s=1;
  if (delta>=0) s=delta/25.0;
  else s=25/delta;
  _prevZoom = val;
  gv_schema->scale(s, s);
}
