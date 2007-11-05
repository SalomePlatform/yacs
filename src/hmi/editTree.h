
#ifndef _EDITTREE_H_
#define _EDITTREE_H_

#include "guiObservers.hxx"

#include "wiedittree.h"
#include <qlistview.h>

#include <map>
#include <utility>

namespace YACS
{
  namespace HMI
  {

    class ViewItem: public QListViewItem, public GuiObserver
    {
    public:
      ViewItem(QListView *parent, QString label, Subject* subject);
      ViewItem(ViewItem *parent, QString label, Subject* subject);
      virtual ~ViewItem();
      void setState(int state);
      virtual void paintCell( QPainter *p, const QColorGroup &cg,
                              int column, int width, int alignment );
      virtual QListView *getParent();
      virtual void select(bool isSelected);
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual Subject* getSubject();
    protected:
      int _state;
      QColor _cf;
      Subject* _subject;
      QListView *_parent;
    };
    
    class editTree: public wiEditTree
    {
      Q_OBJECT
        
    public slots:
      void printName();
      void newNode(int key);
      void newInputPort(int key);
      void newOutputPort(int key);
      void select();
      void select(QListViewItem *it);;
      void destroy();
    public:
      editTree(YACS::HMI::Subject *root,
                QWidget* parent = 0,
                const char* name = 0,
                WFlags fl = 0 );
      virtual ~editTree();
      void addViewItemInMap(YACS::HMI::ViewItem* item, YACS::HMI::Subject* subject);
      YACS::HMI::ViewItem* getViewItem(YACS::HMI::Subject* subject);
    protected:
      void contextMenuEvent ( QContextMenuEvent * );
      void ComposedNodeContextMenu();
      void NodeContextMenu();
      void PortContextMenu();
      void resetTreeNode(QListView *lv);
      Subject *_root;
      QListViewItem *_previousSelected;
      std::map<YACS::HMI::Subject*,YACS::HMI::ViewItem*> _viewItemsMap;
      std::map<int, std::pair<YACS::ENGINE::Catalog*, std::string> > _catalogItemsMap;
      int _keymap;
    };
    
    class NodeViewItem: public ViewItem
    {
    public:
      NodeViewItem(ViewItem *parent, QString label, Subject* subject);
      virtual void update(GuiEvent event, int type, Subject* son);
    protected:
      
    };
    
    
    class ComposedNodeViewItem: public ViewItem
    {
    public:
      ComposedNodeViewItem(QListView *parent, QString label, Subject* subject);
      ComposedNodeViewItem(ViewItem *parent, QString label, Subject* subject);
      virtual void update(GuiEvent event, int type, Subject* son);
    protected:
    };
    
    class PortViewItem: public ViewItem
    {
    public:
      PortViewItem(ViewItem *parent, QString label, Subject* subject);
      virtual void update(GuiEvent event, int type, Subject* son);
    protected:
      
    };
    
  }
}

#endif
