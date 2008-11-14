
#ifndef _CATALOGWIDGET_HXX_
#define _CATALOGWIDGET_HXX_

#include <QTreeWidget>
#include <QSize>
#include <string>
#include <map>

namespace YACS
{
  namespace ENGINE
  {
    class Catalog;
  }

  namespace HMI
  {
    class CatalogWidget: public QTreeWidget
    {
    public:
      CatalogWidget(QWidget *parent,
                    YACS::ENGINE::Catalog* builtinCatalog,
                    YACS::ENGINE::Catalog* sessionCatalog);
      virtual ~CatalogWidget();
      virtual bool addCatalogFromFile(std::string fileName);
      virtual std::map<std::string, YACS::ENGINE::Catalog*> getCataMap();
      virtual YACS::ENGINE::Catalog* getCatalog(std::string cataName);
      virtual YACS::ENGINE::Catalog* getCatalogFromType(std::string typeName);

    protected:
      virtual void addCatalog(YACS::ENGINE::Catalog* catalog,
                              std::string name);
      virtual void startDrag(Qt::DropActions supportedActions);

      YACS::ENGINE::Catalog *_builtinCatalog;
      YACS::ENGINE::Catalog *_sessionCatalog;
      int _idCatalog;
      std::map<std::string, YACS::ENGINE::Catalog*> _cataMap;
      std::map<std::string, YACS::ENGINE::Catalog*> _typeToCataMap;
    };
  }
}

#endif
