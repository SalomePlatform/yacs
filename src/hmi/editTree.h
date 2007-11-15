
#ifndef _EDITTREE_H_
#define _EDITTREE_H_

#include "guiObservers.hxx"

#include "wiedittree.h"
#include <qlistview.h>

#include <map>
#include <utility>

namespace YACS
{
  namespace ENGINE
  {
    class Catalog;
  }

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
    
    class editTree: public wiEditTree, public GuiObserver
    {
      Q_OBJECT
        
    public slots:
      void printName();
      void newNode(int key);
      void newNode(YACS::ENGINE::Catalog* catalog,
                   Subject* sub,
                   std::pair<std::string,std::string> compoServ);
      void newInputPort(int key);
      void newOutputPort(int key);
      void select();
      void select(QListViewItem *it);
      void destroy();
      void addLink();
      void cataSession();
      void cataProc();
    public:
      editTree(YACS::HMI::Subject *context,
                QWidget* parent = 0,
                const char* name = 0,
                WFlags fl = 0 );
      virtual ~editTree();
      virtual void update(GuiEvent event, int type, Subject* son);
      void setNewRoot(YACS::HMI::Subject *root);
      void addViewItemInMap(YACS::HMI::ViewItem* item, YACS::HMI::Subject* subject);
      YACS::HMI::ViewItem* getViewItem(YACS::HMI::Subject* subject);
      YACS::HMI::Subject* getSelectedSubject();
    protected:
      void contextMenuEvent ( QContextMenuEvent * );
      void ComposedNodeContextMenu();
      void NodeContextMenu();
      void PortContextMenu();
      void LinkContextMenu();
      void resetTreeNode(QListView *lv);
      QWidget* _parent;
      Subject *_context;
      Subject *_root;
      QListViewItem *_previousSelected;
      SubjectDataPort *_selectedSubjectOutPort;
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
    
    class LinkViewItem: public ViewItem
    {
    public:
      LinkViewItem(ViewItem *parent, QString label, Subject* subject);
      virtual void update(GuiEvent event, int type, Subject* son);
    protected:
      
    };
    
  }
}

#endif
