
#ifndef _SCHEMAMODEL_HXX_
#define _SCHEMAMODEL_HXX_

#include <QAbstractItemModel>
#include <QItemSelection>
#include <QBrush>
#include "guiObservers.hxx"
#include <list>

namespace YACS
{
  namespace HMI
  {
    typedef enum
      {
        YLabel = 0,
        YType  = 1,
        YValue = 2,
        YState = 2,
      } columnId;

    class SchemaItem;
    class SchemaNodeItem;
    class SchemaComposedNodeItem;
    class SchemaDirTypesItem;
    class SchemaDirLinksItem;
    class SchemaDirContainersItem;
    class SchemaContainerItem;

    class SchemaModel: public QAbstractItemModel, public GuiObserver
    {
      Q_OBJECT

      friend class SchemaItem;
      friend class SchemaNodeItem;
      friend class SchemaComposedNodeItem;
      friend class SchemaDirTypesItem;
      friend class SchemaDirLinksItem;
      friend class SchemaDirContainersItem;
      friend class SchemaContainerItem;
      friend class SchemaComponentItem;

    public:
      SchemaModel(YACS::HMI::Subject *context,
                  QObject * parent = 0);
      virtual ~SchemaModel();

      virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
      virtual QModelIndex parent(const QModelIndex &index) const;
      virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
      virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
      virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
      virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

      virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
      virtual Qt::ItemFlags flags(const QModelIndex &index) const;

      virtual void update(GuiEvent event, int type, Subject* son);
      inline SchemaItem * getRootItem() {return _rootItem; };
      inline bool isEdition() {return _isEdition; };
      void setEdition(bool isEdition = true);

      const QBrush& stdBackBrush();
      const QBrush& editedBackBrush();
      virtual QMimeData* mimeData(const QModelIndexList &indexes) const;
      virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action,
                                int row, int column, const QModelIndex& parent);
      virtual Qt::DropActions supportedDropActions() const;
      virtual QStringList mimeTypes() const;

    public slots:
    void updateSelection(const QItemSelection &selected, const QItemSelection &deselected);

    signals:
    void signalSelection(const QModelIndex &index);

    protected:
      void setNewRoot(YACS::HMI::Subject *root);

      Subject *_context;
      Subject *_root;
      SchemaItem *_rootItem;

      QBrush _stdBackBrush;
      QBrush _editedBackBrush;
      bool _isEdition;
    };

  }
}

#endif
