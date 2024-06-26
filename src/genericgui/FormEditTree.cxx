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

#include "FormEditTree.hxx"
#include "TreeView.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

FormEditTree::FormEditTree(QWidget *parent)
{
  setupUi(this);
  gridLayout->removeWidget(tv_schema);
  delete tv_schema;
  tv_schema = new TreeView(this);
  tv_schema->setObjectName(QString::fromUtf8("tv_schema"));
  gridLayout->addWidget(tv_schema, 0, 0, 1, 1);
}

FormEditTree::~FormEditTree()
{
}

