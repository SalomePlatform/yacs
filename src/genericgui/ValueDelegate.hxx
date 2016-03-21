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

#ifndef _VALUEDELEGATE_HXX
#define _VALUEDELEGATE_HXX

#include "CaseSwitch.hxx"

#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QLineEdit>
#include <QSpinBox>
#include <QVariant>
#include <set>

namespace YACS
{
  namespace HMI
  {
    class Subject;
    class ValueDelegate;

    class GenericEditor
    {
    public:
      GenericEditor();
      virtual ~GenericEditor();
      virtual void setSubject(Subject* subject);
      virtual void setColumn(int column);
      virtual void setDelegate(const ValueDelegate* delegate);
      virtual QString GetStrValue();
      virtual Subject* getSubject();
      virtual int getColumnInSubject();
      virtual void setData(QVariant val);
      virtual bool firstSetData();
    protected:
      Subject* _subject;
      int _column;
      const ValueDelegate* _delegate;
      bool _first;
    };

    class GeneralEditor: public QLineEdit, public GenericEditor
    {
      Q_OBJECT
      
    public:
      GeneralEditor(Subject* subject,
                    const ValueDelegate* delegate,
                    int column = 0,
                    QWidget* parent = 0);
      virtual ~GeneralEditor();
      virtual QString GetStrValue();
      virtual void setData(QVariant val);
    };

    class NameEditor: public QLineEdit, public GenericEditor
    {
      Q_OBJECT
      
    public:
      NameEditor(Subject* subject,
                 const ValueDelegate* delegate,
                 int column = 0,
                 QWidget* parent = 0);
      virtual ~NameEditor();
      virtual QString GetStrValue();
      virtual void setData(QVariant val);
    };

    class IntEditor: public QSpinBox, public GenericEditor
    {
      Q_OBJECT
      
    public:
      IntEditor(Subject* subject,
                const ValueDelegate* delegate,
                int column = 0,
                QWidget* parent = 0);
      virtual ~IntEditor();
      virtual QString GetStrValue();
      virtual void setData(QVariant val);
    };

    class CaseSwitchEditor: public CaseSwitch, public GenericEditor
    {
      Q_OBJECT

    public:
      CaseSwitchEditor(Subject* subject,
                       const ValueDelegate* delegate,
                       int column = 0,
                       QWidget* parent = 0);
      virtual ~CaseSwitchEditor();
      virtual QString GetStrValue();
      virtual void setData(QVariant val);
    };

    class ValueDelegate : public QItemDelegate
    {
      Q_OBJECT
      
    public:
      ValueDelegate(QObject *parent = 0);
      virtual ~ValueDelegate();
      
      virtual QWidget *createEditor(QWidget *parent,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const;
      
      virtual void setEditorData(QWidget *editor,
                                 const QModelIndex &index) const;
      virtual void setModelData(QWidget *editor,
                                QAbstractItemModel *model,
                                const QModelIndex &index) const;
      
      virtual void updateEditorGeometry(QWidget *editor,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const;
      virtual void setResultEditing(QWidget *editor, bool isOk);

    protected:
      mutable std::map<Subject*, std::string> _errorMap;

    };
  }
}
#endif
