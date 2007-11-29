
#include "browseCatalog.h"
#include "guiContext.hxx"
#include "Catalog.hxx"
#include "ComponentDefinition.hxx"
#include "ServiceNode.hxx"
#include "editTree.h"
#include "guiObservers.hxx"

#include <qlistview.h>

#include <iostream>
#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;



BrowseCatalog::BrowseCatalog(editTree* editTree,
                             Subject* sub,
                             int cataType,
                             QWidget* parent,
                             const char* name,
                             bool modal,
                             WFlags fl)
  : catalog(parent, name, modal, fl), _editTree(editTree), _subject(sub), _cataType(cataType)
{
  resetTree();
}

BrowseCatalog::~BrowseCatalog()
{
}

void BrowseCatalog::resetTree()
{
  lv1->clear();
  lv1->setRootIsDecorated( TRUE );
  _currentCatalog = YACS::HMI::GuiContext::getCurrent()->getCurrentCatalog();
  if (_currentCatalog)
    switch (_cataType)
      {
      case CATALOGNODE:
        {
          map<string, YACS::ENGINE::ComponentDefinition*>::const_iterator itComp; 
          for (itComp = _currentCatalog->_componentMap.begin();
               itComp != _currentCatalog->_componentMap.end();
               ++itComp)
            {
              string compoName = (*itComp).first;
              YACS::ENGINE::ComponentDefinition* compo = (*itComp).second;
              DEBTRACE("Component: " << compoName);
              QListViewItem *item = new QListViewItem(lv1, compoName.c_str());
              map<string, YACS::ENGINE::ServiceNode*>::iterator itServ;
              for (itServ = compo->_serviceMap.begin(); itServ != compo->_serviceMap.end(); ++itServ)
                {
                  string serviceName = (*itServ).first;
                  YACS::ENGINE::ServiceNode* service = (*itServ).second;
                  DEBTRACE("   Service: " << serviceName);
                  QListViewItem *sitem = new QListViewItem(item, serviceName.c_str());
                  _serviceMap[sitem] = pair<string,string>(compoName,serviceName);
                }
            }
          break;
        }
      case CATALOGINPUTPORT:
      case CATALOGOUTPUTPORT:
      case CATALOGIDSPORT:
      case CATALOGODSPORT:
      case CATALOGDATATYPE:
        {
          map<string, YACS::ENGINE::TypeCode*>::const_iterator itType;
          for(itType = _currentCatalog->_typeMap.begin();
              itType != _currentCatalog->_typeMap.end();
              ++itType)
            {
              string typeName = (*itType).first;
              YACS::ENGINE::TypeCode* typcod = (*itType).second;
              DEBTRACE("Type Code " << typeName);
              QListViewItem *item = new QListViewItem(lv1, typeName.c_str());
              _typeCodeMap[item] = typeName;
            }
          break;
        }
      default:
        DEBTRACE("BrowseCatalog::resetTree: type not handled " << _cataType);
      }
}

void BrowseCatalog::addSelection()
{
  QListViewItem *it =lv1->selectedItem();
  if (it)                          // an item selected
    switch (_cataType)
      {
      case CATALOGNODE:
        {
          _editTree->newNode(_currentCatalog, _subject, _serviceMap[it]);
          break;
        }
      case CATALOGDATATYPE:
        {
          _editTree->newDataType(_currentCatalog, _subject, _typeCodeMap[it]);
          break;
        }
      case CATALOGINPUTPORT:
        {
          _editTree->newDFInputPort(_currentCatalog, _subject, _typeCodeMap[it]);
          break;
        }
      case CATALOGOUTPUTPORT:
        {
          _editTree->newDFOutputPort(_currentCatalog, _subject, _typeCodeMap[it]);
          break;
        }
      case CATALOGIDSPORT:
        {
          _editTree->newDSInputPort(_currentCatalog, _subject, _typeCodeMap[it]);
          break;
        }
      case CATALOGODSPORT:
        {
          _editTree->newDSOutputPort(_currentCatalog, _subject, _typeCodeMap[it]);
          break;
        }
      default:
        DEBTRACE("BrowseCatalog::resetTree: type not handled " << _cataType);
      }
}
