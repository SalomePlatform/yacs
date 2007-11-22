
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
  _sessionCatalog = YACS::HMI::GuiContext::getCurrent()->getSessionCatalog();
  if (_sessionCatalog)
    switch (_cataType)
      {
      case CATALOGNODE:
        {
          map<string, YACS::ENGINE::ComponentDefinition*>::const_iterator itComp; 
          for (itComp = _sessionCatalog->_componentMap.begin();
               itComp != _sessionCatalog->_componentMap.end();
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
      case CATALOGDATATYPE:
        {
          map<string, YACS::ENGINE::TypeCode*>::const_iterator itType;
          for(itType = _sessionCatalog->_typeMap.begin();
              itType != _sessionCatalog->_typeMap.end();
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
          _editTree->newNode(_sessionCatalog, _subject, _serviceMap[it]);
          break;
        }
      case CATALOGDATATYPE:
        {
          _editTree->newDataType(_sessionCatalog, _subject, _typeCodeMap[it]);
          break;
        }
      default:
        DEBTRACE("BrowseCatalog::resetTree: type not handled " << _cataType);
      }
}
