
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
                             QWidget* parent,
                             const char* name,
                             bool modal,
                             WFlags fl)
  : catalog(parent, name, modal, fl), _editTree(editTree), _subject(sub)
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
    {
      map<string, YACS::ENGINE::ComponentDefinition*>::const_iterator itComp; 
      for (itComp = _sessionCatalog->_componentMap.begin(); itComp != _sessionCatalog->_componentMap.end(); ++itComp)
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
    }
}

void BrowseCatalog::addSelection()
{
  QListViewItem *it =lv1->selectedItem();
  if (it)                          // an item selected
    {
      _editTree->newNode(_sessionCatalog, _subject, _serviceMap[it]);
    }
}
