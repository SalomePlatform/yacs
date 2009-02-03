//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
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
        CATALOGINPUTPORT,
        CATALOGOUTPUTPORT,
        CATALOGIDSPORT,
        CATALOGODSPORT,
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
      YACS::ENGINE::Catalog* _currentCatalog;
      std::map<QListViewItem*,std::pair<std::string,std::string> > _serviceMap;
      std::map<QListViewItem*,std::string> _typeCodeMap;
      editTree* _editTree;
      Subject* _subject;
      int _cataType;
    };

  }
}
#endif
