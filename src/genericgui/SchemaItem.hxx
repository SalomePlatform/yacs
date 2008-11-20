
#ifndef _SCHEMAITEM_HXX_
#define _SCHEMAITEM_HXX_

#include "SchemaModel.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class Node;
  }

  namespace HMI
  {
    class ItemMimeData;

    class SchemaItem: public QObject, public GuiObserver
    {
      Q_OBJECT

    public:
      SchemaItem(SchemaItem *parent, QString label, Subject* subject);
      virtual ~SchemaItem();
      
      virtual void appendChild(SchemaItem *child);
      virtual void removeChild(SchemaItem *child);
      
      virtual SchemaItem *child(int row);
      virtual int childCount() const;
      virtual int columnCount() const;
      virtual QVariant data(int column, int role) const;
      virtual Qt::ItemFlags flags(const QModelIndex &index);
      virtual int row() const;
      virtual SchemaItem *parent();
      virtual Subject* getSubject();
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual void select(bool isSelected);
      virtual void toggleState();
      QModelIndex modelIndex(int column = 0);
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
      virtual ItemMimeData* mimeData(ItemMimeData *mime);
      virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action);
      virtual void reparent(SchemaItem *parent);

    protected:
      virtual QString getMimeFormat();
      virtual void setExecState(int execState);
      QList<SchemaItem*> _childItems;
      QList<QVariant> _itemData;
      QList<QVariant> _itemDeco;
      QList<QVariant> _itemForeground;
      QList<QVariant> _itemBackground;
      QList<QVariant> _itemCheckState;
      QString _label;
      SchemaItem *_parentItem;
      Subject* _subject;
      int _execState;
    };


//     class SchemaSelectionModel: public QItemSelectionModel
//     {
//     public:
//       SchemaSelectionModel(QAbstractItemModel *model);
//       SchemaSelectionModel(QAbstractItemModel *model, QObject *parent);
//       virtual ~SchemaSelectionModel();
//     };

  }
}

#endif