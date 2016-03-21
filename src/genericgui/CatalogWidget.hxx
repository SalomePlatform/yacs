// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
      virtual void mousePressEvent(QMouseEvent  *event);

    protected:
      virtual void addCatalog(YACS::ENGINE::Catalog* catalog,
                              std::string name);
      virtual void startDrag(Qt::DropActions supportedActions);

      YACS::ENGINE::Catalog *_builtinCatalog;
      YACS::ENGINE::Catalog *_sessionCatalog;
      int _idCatalog;
      std::map<std::string, YACS::ENGINE::Catalog*> _cataMap;
      std::map<std::string, YACS::ENGINE::Catalog*> _typeToCataMap;
      bool _dragModifier;
    };
  }
}

#endif
