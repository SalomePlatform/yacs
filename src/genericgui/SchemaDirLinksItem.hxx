#ifndef _SCHEMADIRTLINKSITEM_HXX_
#define _SCHEMADIRTLINKSITEM_HXX_

#include "SchemaItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SchemaDirLinksItem: public SchemaItem
    {
    public:
      SchemaDirLinksItem(SchemaItem *parent, QString label, Subject* subject);
      void addLinkItem(Subject* subject);
      virtual Qt::ItemFlags flags(const QModelIndex &index);
    protected:
    };
  }
}

#endif
