#ifndef _SCHEMADIRTCONTAINERSITEM_HXX_
#define _SCHEMADIRTCONTAINERSITEM_HXX_

#include "SchemaItem.hxx"
#include <map>
#include <string>

namespace YACS
{
  namespace HMI
  {
    class SchemaDirContainersItem: public SchemaItem
    {
    public:
      SchemaDirContainersItem(SchemaItem *parent, QString label, Subject* subject);
      void addContainerItem(Subject* subject);
      void addComponentItem(Subject* subject);
    protected:
      std::map<std::string, SchemaContainerItem*> _schemaContItemMap;
      std::multimap<std::string, Subject*> _waitingCompItemMap;
    };
  }
}

#endif
