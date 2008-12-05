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
#ifndef _EDITIONELEMENTARYNODE_HXX_
#define _EDITIONELEMENTARYNODE_HXX_

#include "EditionNode.hxx"

#include <QTabWidget>
#include <QTableView>
#include <QWidget>

namespace YACS
{
  namespace HMI
  {

      // ports input et output, creation, modification, cablage + init
      // panneau depliable sur le port selectionne?
      // panneau depliable sur le type du port selectionne?

    class ValueDelegate;

    class EditionElementaryNode: public EditionNode
    {
      Q_OBJECT

    public slots:
      virtual void onApply();
      virtual void onCancel();
      virtual void onPortIndexChanged(int index);
      virtual void onCommitData(QWidget *editor);

    public:
      EditionElementaryNode(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionElementaryNode();
      virtual void synchronize();
      virtual void setEditablePorts(bool isEditable);

    protected:
      virtual bool hasInputPorts();
      virtual bool hasOutputPorts();
      virtual void createTablePorts();
      SubjectElementaryNode *_subElemNode;
      QTabWidget* _twPorts;
      TablePortsEdition* _tvInPorts;
      TablePortsEdition* _tvOutPorts;
      ValueDelegate* _valueDelegate;
    };
  }
}
#endif
