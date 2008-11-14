#ifndef _SCHEMACONTAINERITEM_HXX_
#define _SCHEMACONTAINERITEM_HXX_

#include "SchemaItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SchemaContainerItem: public SchemaItem
    {
    public:
      SchemaContainerItem(SchemaItem *parent, QString label, Subject* subject);
      virtual void update(GuiEvent event, int type, Subject* son);
      void addComponentInstance(Subject* subject);
    protected:
    };
  }
}

#endif
