#ifndef _SCHEMAINPORTITEM_HXX_
#define _SCHEMAINPORTITEM_HXX_

#include "SchemaItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SchemaInPortItem: public SchemaItem
    {
    public:
      SchemaInPortItem(SchemaItem *parent, QString label, Subject* subject);
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual Qt::ItemFlags flags(const QModelIndex &index);
      virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action);
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);

    protected:
    };
  }
}

#endif
