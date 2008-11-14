
#ifndef _SCHEMAPROCITEM_HXX_
#define _SCHEMAPROCITEM_HXX_

#include "SchemaComposedNodeItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SchemaProcItem: public SchemaComposedNodeItem
    {
    public:
      SchemaProcItem(SchemaItem *parent, QString label, Subject* subject);
      virtual ~SchemaProcItem();
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
    protected:
      virtual void setExecState(int execState);
    };
  }
}

#endif
