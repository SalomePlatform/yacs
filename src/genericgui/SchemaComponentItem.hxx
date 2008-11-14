#ifndef _SCHEMACOMPONENTITEM_HXX_
#define _SCHEMACOMPONENTITEM_HXX_

#include "SchemaItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SchemaComponentItem: public SchemaItem
    {
    public:
      SchemaComponentItem(SchemaItem *parent, QString label, Subject* subject);
      virtual void update(GuiEvent event, int type, Subject* son);
    protected:
    };
  }
}

#endif
