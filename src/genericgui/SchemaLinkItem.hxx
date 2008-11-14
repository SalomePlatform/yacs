#ifndef _SCHEMALINKITEM_HXX_
#define _SCHEMALINKITEM_HXX_

#include "SchemaItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SchemaLinkItem: public SchemaItem
    {
    public:
      SchemaLinkItem(SchemaItem *parent, QString label, Subject* subject);
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
    protected: 
    };
  }
}

#endif
