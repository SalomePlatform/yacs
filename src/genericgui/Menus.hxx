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

#ifndef _MENUS_HXX_
#define _MENUS_HXX_

#include <QObject>
#include <QWidget>
#include <QAction>
#include <QPoint>

namespace YACS
{
  namespace HMI
  {
    class MenusBase: public QObject
    {
      Q_OBJECT
    public:
      MenusBase();
      virtual ~MenusBase();
      virtual void popupMenu(QWidget *caller,
                             const QPoint &globalPos,
                             const QString& m = "MenuBase");
      virtual void addForEachMenu(QMenu *m, QActionGroup* actgroup);
      virtual void addForEachMenuToPutGraph(QMenu *m, QActionGroup* actgroup);
    protected slots:
      void dummyAction();
      void foreachAction(QAction*);
      void putGraphInForeachAction(QAction*);
    protected:
      virtual void buildForEachMenu(QMenu *m, QActionGroup* actgroup);
      virtual void addHeader(QMenu &m, const QString &h);
      QAction *_dummyAct;
    };

    class NodeMenu: public MenusBase
    {
      Q_OBJECT
    public:
      NodeMenu();
      virtual ~NodeMenu();
      virtual void popupMenu(QWidget *caller,
                             const QPoint &globalPos,
                             const QString& m = "Node Menu");
    };

    class ComposedNodeMenu: public MenusBase
    {
      Q_OBJECT
    public:
      ComposedNodeMenu();
      virtual ~ComposedNodeMenu();
      virtual void popupMenu(QWidget *caller,
                             const QPoint &globalPos,
                             const QString& m = "Composed Node Menu");
    };

    class ProcMenu: public MenusBase
    {
      Q_OBJECT
    public:
      ProcMenu();
      virtual ~ProcMenu();
      virtual void popupMenu(QWidget *caller,
                             const QPoint &globalPos,
                             const QString& m = "Proc Menu");
    };

    class HeaderNodeMenu: public MenusBase
    {
      Q_OBJECT
    public:
      HeaderNodeMenu();
      virtual ~HeaderNodeMenu();
      virtual void popupMenu(QWidget *caller,
                             const QPoint &globalPos,
                             const QString& m = "Header Node Menu");
    };

    class LinkMenu: public MenusBase
    {
      Q_OBJECT
    public:
      LinkMenu();
      virtual ~LinkMenu();
      virtual void popupMenu(QWidget *caller,
                             const QPoint &globalPos,
                             const QString& m = "Link Menu");
    };

    class ElementaryNodeMenu: public MenusBase
    {
      Q_OBJECT
    public:
      ElementaryNodeMenu();
      virtual ~ElementaryNodeMenu();
      virtual void popupMenu(QWidget *caller,
                             const QPoint &globalPos,
                             const QString& m = "Elementary Node Menu");
    };

    class InPortMenu: public MenusBase
    {
      Q_OBJECT
    public:
      InPortMenu();
      virtual ~InPortMenu();
      virtual void popupMenu(QWidget *caller,
                             const QPoint &globalPos,
                             const QString& m = "In Port Menu");
    };

    class OutPortMenu: public MenusBase
    {
      Q_OBJECT
    public:
      OutPortMenu();
      virtual ~OutPortMenu();
      virtual void popupMenu(QWidget *caller,
                             const QPoint &globalPos,
                             const QString& m = "Out Port Menu");
    };

    class CtrlInPortMenu: public MenusBase
    {
      Q_OBJECT
    public:
      CtrlInPortMenu();
      virtual ~CtrlInPortMenu();
      virtual void popupMenu(QWidget *caller,
                             const QPoint &globalPos,
                             const QString& m = "Control In Port Menu");
    };

    class CtrlOutPortMenu: public MenusBase
    {
      Q_OBJECT
    public:
      CtrlOutPortMenu();
      virtual ~CtrlOutPortMenu();
      virtual void popupMenu(QWidget *caller,
                             const QPoint &globalPos,
                             const QString& m = "Control Out Port Menu");
    };

    class ReferenceMenu: public MenusBase
    {
      Q_OBJECT
    public:
      ReferenceMenu();
      virtual ~ReferenceMenu();
      virtual void popupMenu(QWidget *caller,
                             const QPoint &globalPos,
                             const QString& m = "Reference Menu");
    };

    class ContainerDirMenu: public MenusBase
    {
      Q_OBJECT
    public:
      ContainerDirMenu();
      virtual ~ContainerDirMenu();
      virtual void popupMenu(QWidget *caller,
                             const QPoint &globalPos,
                             const QString& m = "Containers Menu");
    };

    class ComponentInstanceMenu: public MenusBase
    {
      Q_OBJECT
    public:
      ComponentInstanceMenu();
      virtual ~ComponentInstanceMenu();
      virtual void popupMenu(QWidget *caller,
                             const QPoint &globalPos,
                             const QString& m = "ComponentInstance Menu");
    };

    class ContainerMenu: public MenusBase
    {
      Q_OBJECT
    public:
      ContainerMenu();
      virtual ~ContainerMenu();
      virtual void popupMenu(QWidget *caller,
                             const QPoint &globalPos,
                             const QString& m = "Container Menu");
    };
  }
}

#endif
