
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
    protected slots:
      void dummyAction();
    protected:
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

  }
}

#endif
