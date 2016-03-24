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

#ifndef _PROPERTYEDITOR_HXX_
#define _PROPERTYEDITOR_HXX_

#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QToolBar>
#include <QAction>

namespace YACS
{
  namespace HMI
  {
    class Subject;

    class PropertyEditor: public QWidget
    {
      Q_OBJECT

      public:
        PropertyEditor(Subject* subject,QWidget *parent = 0);
        virtual ~PropertyEditor();
        void updateMenu();
        void update();
        void setProperties();

      public slots:
        virtual void onAddProperty(QAction* action);
        virtual void onRemoveProperty();
        virtual void onItemChanged(QTableWidgetItem * item);
        virtual void on_tb_options_toggled(bool checked);

      protected:
       Subject* _subject;
       QTableWidget* _table;
       QToolBar* _bar;
       QAction* _removeAction;
       QAction* _addAction;
       QStringList _knownProperties;
       QStringList _propertyNames;
       bool _editing;
    };
  }
}

#endif
