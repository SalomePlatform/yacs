#ifndef _TREEVIEW_HXX_
#define _TREEVIEW_HXX_

#include <QTreeView>
#include <QContextMenuEvent>
#include <QAction>

namespace YACS
{
  namespace HMI
  {
    class TreeView: public QTreeView
    {
      Q_OBJECT

    public:
      TreeView(QWidget *parent = 0);
      virtual ~TreeView();
      virtual void setModel(QAbstractItemModel *model);

    public slots:
      void viewSelection(const QModelIndex &ind);
      void resizeColumns();

    protected:
      void contextMenuEvent(QContextMenuEvent *event);

      bool _isEdition;

//       virtual void mousePressEvent(QMouseEvent *event);
//       virtual void mouseMoveEvent(QMouseEvent *event);
//       virtual void mouseReleaseEvent(QMouseEvent *event);

//       void dragEnterEvent(QDragEnterEvent *event);
//       void dragLeaveEvent(QDragLeaveEvent *event);
//       void dragMoveEvent(QDragMoveEvent *event);
//       void dropEvent(QDropEvent *event);

    };
  }
}
#endif
