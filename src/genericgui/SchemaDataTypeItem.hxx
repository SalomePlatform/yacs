#ifndef _SCHEMADATATTYPEITEM_HXX_
#define _SCHEMADATATTYPEITEM_HXX_

#include "SchemaItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SchemaDataTypeItem: public SchemaItem
    {
    public:
      SchemaDataTypeItem(SchemaItem *parent, QString label, Subject* subject);
      virtual void update(GuiEvent event, int type, Subject* son);
    protected:
    };
  }
}

#endif
