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

#ifndef _TABLEPORTSEDITION_HXX_
#define _TABLEPORTSEDITION_HXX_

#include "ui_TablePorts.h"

#include <QDialog>
#include <QTreeWidget>
#include <QComboBox>

namespace YACS
{
  namespace HMI
  {
    class SubjectElementaryNode;

    class YComboBox: public QComboBox
    {
      Q_OBJECT

    public:
      YComboBox(QWidget *parent = 0);
      virtual ~YComboBox();
      virtual void hidePopup();
      virtual void showPopup();

    protected:
      void keyPressEvent(QKeyEvent *e);
#ifndef QT_NO_WHEELEVENT
      void wheelEvent(QWheelEvent *e);
#endif

    signals:
      void popupHide();
      void popupShow();
    };

    class TablePortsEdition: public QWidget, public Ui::fm_TablePorts
    {
      Q_OBJECT

    public:
      TablePortsEdition(bool inPorts, QWidget *parent = 0);
      virtual ~TablePortsEdition();
      void setNode(SubjectElementaryNode* node);
      virtual void setEditablePorts(bool isEditable);
      virtual void adjustColumns();

    public slots:
      void on_pb_up_clicked();
      void on_pb_down_clicked();
      void on_pb_insert_clicked();
      void on_pb_remove_clicked();
      void oncb_insert_activated(const QString& text);
      void oncb_insert_popupHide();
      void oncb_insert_popupShow();
            
    protected:
      void upOrDown(int isUp);
      bool _inPorts;
      SubjectElementaryNode* _node;
      int _nbUp;
    private:
    };

  }
}


#endif
