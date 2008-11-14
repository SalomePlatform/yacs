#ifndef _SCHEMAOUTPORTITEM_HXX_
#define _SCHEMAOUTPORTITEM_HXX_

#include "SchemaItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SchemaOutPortItem: public SchemaItem
    {
    public:
      SchemaOutPortItem(SchemaItem *parent, QString label, Subject* subject);
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual Qt::ItemFlags flags(const QModelIndex &index);
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);

    protected:
      virtual QString getMimeFormat();
    };
  }
}

#endif
