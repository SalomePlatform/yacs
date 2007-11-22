#ifndef _BROWSECATALOG_H_
#define _BROWSECATALOG_H_

#include "catalog.h"

#include <qlistview.h>
#include <list>
#include <string>
#include <map>

#include <qlistview.h>

namespace YACS
{
  namespace ENGINE
  {
    class Catalog;
/*     class ServiceNode; */
  }

  namespace HMI
  {

    typedef enum
      {
        CATALOGNODE,
        CATALOGDATATYPE,
        CATALOGCOMPOSEDNODE
      } TypeOfCatalogItem;

    class editTree;
    class Subject;

    class BrowseCatalog: public catalog
    {
      Q_OBJECT

    public:
      BrowseCatalog(editTree* editTree,
                    Subject* sub,
                    int cataType,
                    QWidget* parent = 0,
                    const char* name = 0,
                    bool modal = FALSE,
                    WFlags fl = 0 );
      virtual ~BrowseCatalog();
      virtual void resetTree();
    public slots:
      virtual void addSelection();
    protected:
      YACS::ENGINE::Catalog* _sessionCatalog;
      std::map<QListViewItem*,std::pair<std::string,std::string> > _serviceMap;
      std::map<QListViewItem*,std::string> _typeCodeMap;
      editTree* _editTree;
      Subject* _subject;
      int _cataType;
    };

  }
}
#endif
