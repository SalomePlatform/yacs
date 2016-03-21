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

#include "RuntimeSALOME.hxx"
#include "CatalogWidget.hxx"

#include "Catalog.hxx"
#include "TypeCode.hxx"
#include "ComponentDefinition.hxx"
#include "ItemMimeData.hxx"
#include "QtGuiContext.hxx"

#include <QApplication>
#include <QMimeData>
#include <QDrag>
#include <QPainter>
#include <QBitmap>
#include <QString>
#include <QFileInfo>
#include <QMouseEvent>

#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

CatalogWidget::CatalogWidget(QWidget *parent,
                             YACS::ENGINE::Catalog* builtinCatalog,
                             YACS::ENGINE::Catalog* sessionCatalog)
  : QTreeWidget(parent)
{
  DEBTRACE("CatalogWidget::CatalogWidget");
  _builtinCatalog = builtinCatalog;
  _sessionCatalog = sessionCatalog;

  _idCatalog = 0;
  _cataMap.clear();
  _typeToCataMap.clear();
  _dragModifier=false;

  setColumnCount(1);
  setHeaderHidden( true );

  addCatalog(_builtinCatalog, "Built In");
  addCatalog(_sessionCatalog, "Current Session");

  setDragDropMode(QAbstractItemView::DragOnly);
  setDragEnabled(true);
  setDropIndicatorShown(true);

  setSelectionMode(QAbstractItemView::ExtendedSelection);
}

bool CatalogWidget::addCatalogFromFile(std::string fileName)
{
  DEBTRACE("CatalogWidget::addCatalogFromFile " << fileName);
  QFileInfo afi(fileName.c_str());
  if(!afi.exists())
    return false;
  Catalog *cataProc = YACS::ENGINE::getSALOMERuntime()->loadCatalog("proc", fileName);
  string aFile = afi.fileName().toStdString();
  addCatalog(cataProc, aFile);
}

std::map<std::string, YACS::ENGINE::Catalog*> CatalogWidget::getCataMap()
{
  return _cataMap;
}

YACS::ENGINE::Catalog* CatalogWidget::getCatalog(std::string cataName)
{
  YACS::ENGINE::Catalog* catalog = 0;
  if (_cataMap.count(cataName))
    catalog = _cataMap[cataName];
  return catalog;
}

YACS::ENGINE::Catalog* CatalogWidget::getCatalogFromType(std::string typeName)
{
  DEBTRACE("CatalogWidget::getCatalogFromType " << typeName);
  YACS::ENGINE::Catalog* catalog = 0;
  if (_typeToCataMap.count(typeName))
    catalog = _typeToCataMap[typeName];
  return catalog;
}

void CatalogWidget::addCatalog(YACS::ENGINE::Catalog* catalog,
                              std::string name)
{
  if (!catalog) return;

  QTreeWidgetItem *itemCata = 0;
  QTreeWidgetItem *category = 0;

  itemCata = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(name.c_str())));
  insertTopLevelItem(_idCatalog, itemCata);
  _idCatalog++;

  if (! catalog->_typeMap.empty())
    {
      category = new QTreeWidgetItem(itemCata, QStringList(QString("Types")));
      map<string, TypeCode*>::const_iterator it = catalog->_typeMap.begin();
      for (; it != catalog->_typeMap.end(); ++it)
        {
          DEBTRACE("Type : " <<(*it).first
                   << " " << (*it).second->getKindRepr()
                   << " " << (*it).second->name()
                   << " " << (*it).second->shortName()
                   << " " << (*it).second->id() );
          string typeName = it->first;
          QTreeWidgetItem *item = new QTreeWidgetItem(category, QStringList(QString(typeName.c_str())));
          if (! _typeToCataMap.count(typeName))
            _typeToCataMap[typeName] = catalog;
          else if ( ! ((*it).second)->isEquivalent(_typeToCataMap[typeName]->_typeMap[typeName]) )
            {
              DEBTRACE(" ========================================================================================================");
              DEBTRACE(" type " << typeName << " not compatible with one of same name already present in another catalog, FORCE NEW!");
              DEBTRACE(" ========================================================================================================");
             _typeToCataMap[typeName] = catalog;
             item->setForeground(0,Qt::blue);
           }
        }
    }

  if (! catalog->_componentMap.empty())
    {
      category = new QTreeWidgetItem(itemCata, QStringList(QString("Components")));
      map<string, ComponentDefinition*>::const_iterator it = catalog->_componentMap.begin();
      for (; it != catalog->_componentMap.end(); ++it)
        {
          QTreeWidgetItem *item = new QTreeWidgetItem(category, QStringList(QString((it->first).c_str())));
          map<string, ServiceNode *>::const_iterator its =  (it->second)->_serviceMap.begin();
          for (; its != (it->second)->_serviceMap.end(); ++its)
            QTreeWidgetItem *sitem = new QTreeWidgetItem(item, QStringList(QString((its->first).c_str())));
        }
    }

  if (! catalog->_nodeMap.empty())
    {
      category = new QTreeWidgetItem(itemCata, QStringList(QString("Elementary Nodes")));
      map<string, Node*>::const_iterator it = catalog->_nodeMap.begin();
      for (; it != catalog->_nodeMap.end(); ++it)
        QTreeWidgetItem *item = new QTreeWidgetItem(category, QStringList(QString((it->first).c_str())));
    }

  if (! catalog->_composednodeMap.empty())
    {
      category = new QTreeWidgetItem(itemCata, QStringList(QString("Composed Nodes")));
      map<string, ComposedNode*>::const_iterator it = catalog->_composednodeMap.begin();
      for (; it != catalog->_composednodeMap.end(); ++it)
        QTreeWidgetItem *item = new QTreeWidgetItem(category, QStringList(QString((it->first).c_str())));
    }
  _cataMap[name] = catalog;
}

CatalogWidget::~CatalogWidget()
{
}

void CatalogWidget::startDrag(Qt::DropActions supportedActions)
{
  DEBTRACE("startDrag " << supportedActions);
  if (! QtGuiContext::getQtCurrent()) return;
  if (! QtGuiContext::getQtCurrent()->isEdition()) return;

  QDrag *drag = 0;
  ItemMimeData *mime = 0;
  QString theMimeInfo;

  QList<QTreeWidgetItem*> selectList = selectedItems();
  for (int i=0; i<selectList.size(); i++)
    {
      QTreeWidgetItem *parent = selectList[i]->parent();
      if (!parent) continue;
      QTreeWidgetItem *grandPa = parent->parent();
      if (!grandPa) continue;
      QTreeWidgetItem *grandGrandPa = grandPa->parent();
      string cataName ="";
      if (grandGrandPa)
        cataName = grandGrandPa->text(0).toStdString();
      else
        cataName = grandPa->text(0).toStdString();
      DEBTRACE("cataName=" << cataName);
      YASSERT(_cataMap.count(cataName));
      YACS::ENGINE::Catalog *catalog = _cataMap[cataName];

      QString mimeInfo;
      string compo = "";
      string definition = "";
      QPixmap pixmap;
      if (! parent->text(0).compare("Types"))
        {
          mimeInfo = "Type";
          definition = selectList[i]->text(0).toStdString();
          pixmap.load("icons:data_link.png");
        }
      else if (parent->text(0).contains("Nodes"))
        {
          mimeInfo = "Node";
          definition = selectList[i]->text(0).toStdString();
          pixmap.load("icons:add_node.png");
        }
      else if (! grandPa->text(0).compare("Components"))
        {
          mimeInfo = "Service";
          definition = selectList[i]->text(0).toStdString();
          compo = parent->text(0).toStdString();
          pixmap.load("icons:new_salome_service_node.png");
        }
      else
        {
          mimeInfo = "Component";
          compo = selectList[i]->text(0).toStdString();
          pixmap.load("icons:component.png");
        }
      QString mimeType = "yacs/cata" + mimeInfo;
      
      if (!drag) // --- intialize mime data with the first selected item
        {
          DEBTRACE("mimeInfo=" << mimeInfo.toStdString() << " definition=" << definition << " compo=" << compo);
          drag = new QDrag(this);
          mime = new ItemMimeData;
          drag->setMimeData(mime);
          mime->setData(mimeType, mimeInfo.toLatin1());
          drag->setPixmap(pixmap);

          theMimeInfo = mimeInfo;

          mime->setCatalog(catalog);
          mime->setCataName(cataName);
          mime->setCompo(compo);
          mime->setType(definition);
        }
      else       // --- push only selected item of the same mimeType than the first
        {
          if (theMimeInfo == mimeInfo)
            {
              DEBTRACE("mimeInfo=" << mimeInfo.toStdString() << " definition=" << definition << " compo=" << compo);
              mime->setCatalog(catalog);
              mime->setCataName(cataName);
              mime->setCompo(compo);
              mime->setType(definition);
            }          
        }
    }

  if (drag)
    {
      if(_dragModifier)
        mime->setControl(true);
      else
        mime->setControl(false);
      
      drag->exec(supportedActions);
    }
}

void CatalogWidget::mousePressEvent(QMouseEvent  *event)
{
  DEBTRACE("CatalogWidget::mousePressEvent ");
  _dragModifier= false;
  if(event->button() == Qt::MidButton)
    _dragModifier= true;
  QTreeWidget::mousePressEvent(event);
}

