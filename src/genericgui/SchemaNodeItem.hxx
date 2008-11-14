#ifndef _SCHEMANODEITEM_HXX_
#define _SCHEMANODEITEM_HXX_

#include "SchemaItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SchemaNodeItem: public SchemaItem
    {
    public:
      SchemaNodeItem(SchemaItem *parent, QString label, Subject* subject);
      virtual ~SchemaNodeItem();
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
      virtual Qt::ItemFlags flags(const QModelIndex &index);
      virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action);
      virtual void toggleState();
    protected:
      virtual QString getMimeFormat();
    };
  }
}

#endif
