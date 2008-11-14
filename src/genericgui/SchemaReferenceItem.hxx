#ifndef _SCHEMAREFERENCEITEM_HXX_
#define _SCHEMAREFERENCEITEM_HXX_

#include "SchemaItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SchemaReferenceItem: public SchemaItem
    {
    public:
      SchemaReferenceItem(SchemaItem *parent, QString label, Subject* subject);
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
    protected:
    };
  }
}

#endif
