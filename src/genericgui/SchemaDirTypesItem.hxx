#ifndef _SCHEMADIRTTYPESITEM_HXX_
#define _SCHEMADIRTTYPESITEM_HXX_

#include "SchemaItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SchemaDirTypesItem: public SchemaItem
    {
    public:
      SchemaDirTypesItem(SchemaItem *parent, QString label, Subject* subject);
      void addTypeItem(Subject* subject);
      virtual Qt::ItemFlags flags(const QModelIndex &index);
      virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action);
    protected:
    };
  }
}

#endif
