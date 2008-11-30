#ifndef _SCHEMACOMPOSEDNODEITEM_HXX_
#define _SCHEMACOMPOSEDNODEITEM_HXX_

#include "SchemaItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SchemaDirTypesItem;
    class SchemaDirContainersItem;
    class SchemaDirLinksItem;

    class SchemaComposedNodeItem: public SchemaItem
    {
    public:
      SchemaComposedNodeItem(SchemaItem *parent, QString label, Subject* subject);
      virtual ~SchemaComposedNodeItem();
      virtual void update(GuiEvent event, int type, Subject* son);
      std::list<YACS::ENGINE::Node*> getDirectDescendants() const;
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
      virtual Qt::ItemFlags flags(const QModelIndex &index);
      virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action);
      virtual void setCaseValue();
    protected:
      virtual QString getMimeFormat();
      SchemaDirTypesItem *_dirTypesItem;
      SchemaDirContainersItem *_dirContainersItem;
      SchemaDirLinksItem *_dirLinksItem;
    };
  }
}

#endif
