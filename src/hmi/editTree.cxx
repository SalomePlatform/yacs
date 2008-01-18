
#include "RuntimeSALOME.hxx"
#include "editTree.h"
#include "Proc.hxx"
#include "Node.hxx"
#include "guiContext.hxx"
#include "Catalog.hxx"
#include "browseCatalog.h"
#include "chooseName.h"
#include "ServiceNode.hxx"

#include <iostream>
#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <qpopupmenu.h>
#include <qfiledialog.h>
#include <qlabel.h>
#include <qcursor.h>
#include <qmessagebox.h>


#include <string>
#include <sstream>
#include <iostream>

using namespace std;

using namespace YACS;
using namespace YACS::HMI;


/*!
 *
 */

ViewItem::ViewItem(QListView *parent, QString label, Subject* subject)
  : QListViewItem(parent, label)
{
  _parent = parent;
  _subject = subject;
  _cf = Qt::black;
  _subject->attach(this);
}

/*!
 *
 */

ViewItem::ViewItem(ViewItem *parent, QString label, Subject* subject)
  : QListViewItem(parent, label)
{
  _parent = parent->getParent();
  _subject = subject;
  _cf = Qt::black;
  _subject->attach(this);
}

/*!
 *
 */

ViewItem::~ViewItem()
{
  _subject->detach(this);
}

/*!
 *
 */

void ViewItem::setState(int state)
{
}

/*!
 *
 */

QListView *ViewItem::getParent()
{
  return _parent;
}

/*!
 *
 */

Subject* ViewItem::getSubject()
{
  return _subject;
}

/*!
 *
 */

void ViewItem::select(bool isSelected)
{
//   DEBTRACE("ViewItem::select() "<< this);
  _parent->setSelected(this, isSelected);
  _parent->ensureItemVisible(this);
}

void ViewItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("ViewItem::update");
  ViewItem *item = 0;
  switch (event)
    {
    case RENAME:
      DEBTRACE("ViewItem::update RENAME " << _subject->getName());
      setText(0,_subject->getName());
      break;
    case ADDREF:
      DEBTRACE("ViewItem::update ADDREF " << _subject->getName());
      item = new ReferenceViewItem(this,
                                   son->getName(),
                                   son);
      break;
    default:
      DEBTRACE("ViewItem::update(), event not handled: " << event);
      GuiObserver::update(event, type, son);
    }
}

/*!
 *
 */

void ViewItem::paintCell( QPainter *p, const QColorGroup &cg,
                          int column, int width, int alignment )
{
  QColorGroup _cg( cg );
  QColor c = _cg.text();
  if (column == 1) _cg.setColor( QColorGroup::Text, _cf );
  QListViewItem::paintCell( p, _cg, column, width, alignment );
  if (column == 1) _cg.setColor( QColorGroup::Text, c );
}

// ----------------------------------------------------------------------------

/*!
 *
 */

NodeViewItem::NodeViewItem(ViewItem *parent,
                           QString label,
                           Subject* subject)
  : ViewItem::ViewItem(parent, label, subject)
{
}

void NodeViewItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("NodeViewItem::update");
  ViewItem *item = 0;
  switch (event)
    {
    case YACS::HMI::ADD:
      switch (type)
        {
        case YACS::HMI::INPUTPORT:
        case YACS::HMI::OUTPUTPORT:
        case YACS::HMI::INPUTDATASTREAMPORT:
        case YACS::HMI::OUTPUTDATASTREAMPORT:
          item =  new PortViewItem(this,
                                   son->getName(),
                                   son);
          break;
        default:
          DEBTRACE("NodeViewItem::update(), ADD, type not handled: " << type);
        }
      break;
    default:
      DEBTRACE("NodeViewItem::update(), event not handled: " << event);
      ViewItem::update(event, type, son);
    }
}

// ----------------------------------------------------------------------------

/*!
 *
 */

ComposedNodeViewItem::ComposedNodeViewItem(QListView *parent,
                                           QString label,
                                           Subject* subject)
  : ViewItem::ViewItem(parent, label, subject)
{
}

/*!
 *
 */


ComposedNodeViewItem::ComposedNodeViewItem(ViewItem *parent,
                                           QString label,
                                           Subject* subject)
  : ViewItem::ViewItem(parent, label, subject)
{
}

void ComposedNodeViewItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("ComposedNodeViewItem::update");
  ViewItem *item = 0;
  switch (event)
    {
    case YACS::HMI::ADD:
      switch (type)
        {
        case YACS::HMI::BLOC:
        case YACS::HMI::FORLOOP:
        case YACS::HMI::WHILELOOP:
        case YACS::HMI::SWITCH:
        case YACS::HMI::FOREACHLOOP:
        case YACS::HMI::OPTIMIZERLOOP:
          item =  new ComposedNodeViewItem(this,
                                           son->getName(),
                                           son);
          break;
        case YACS::HMI::PYTHONNODE:
        case YACS::HMI::PYFUNCNODE:
        case YACS::HMI::CORBANODE:
        case YACS::HMI::CPPNODE:
        case YACS::HMI::SALOMENODE:
        case YACS::HMI::SALOMEPYTHONNODE:
        case YACS::HMI::XMLNODE:
        case YACS::HMI::PRESETNODE:
        case YACS::HMI::OUTNODE:
        case YACS::HMI::STUDYINNODE:
        case YACS::HMI::STUDYOUTNODE:
          item =  new NodeViewItem(this,
                                   son->getName(),
                                   son);
          break;
        case YACS::HMI::INPUTPORT:
        case YACS::HMI::OUTPUTPORT:
        case YACS::HMI::INPUTDATASTREAMPORT:
        case YACS::HMI::OUTPUTDATASTREAMPORT:
          item =  new PortViewItem(this,
                                   son->getName(),
                                   son);
          break;
        case YACS::HMI::COMPONENT:
          item =  new ComponentViewItem(this,
                                        son->getName(),
                                        son);
          break;
        case YACS::HMI::CONTAINER:
          item =  new ContainerViewItem(this,
                                        son->getName(),
                                        son);
          break;
        case YACS::HMI::DATATYPE:
          item = new DataTypeViewItem(this,
                                      son->getName(),
                                      son);
          break;
        default:
          DEBTRACE("ComposedNodeViewItem::update() ADD, type not handled:" << type);
        }
      break;
    case YACS::HMI::ADDLINK:
    case YACS::HMI::ADDCONTROLLINK:
      item = new LinkViewItem(this,
                              son->getName(),
                              son);
      break;
    default:
      DEBTRACE("ComposedNodeViewItem::update(), event not handled: "<< event);
      ViewItem::update(event, type, son);
    }
}

// ----------------------------------------------------------------------------

PortViewItem::PortViewItem(ViewItem *parent, QString label, Subject* subject)
  : ViewItem::ViewItem(parent, label, subject)
{
}

void PortViewItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("PortViewItem::update");
  ViewItem::update(event, type, son);
}

// ----------------------------------------------------------------------------

LinkViewItem::LinkViewItem(ViewItem *parent, QString label, Subject* subject)
  : ViewItem::ViewItem(parent, label, subject)
{
}

void LinkViewItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("LinkViewItem::update");
  ViewItem::update(event, type, son);
}

// ----------------------------------------------------------------------------

ComponentViewItem::ComponentViewItem(ViewItem *parent, QString label, Subject* subject)
  : ViewItem::ViewItem(parent, label, subject)
{
}

void ComponentViewItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("ComponentViewItem::update");
  ViewItem::update(event, type, son);
}

// ----------------------------------------------------------------------------

ContainerViewItem::ContainerViewItem(ViewItem *parent, QString label, Subject* subject)
  : ViewItem::ViewItem(parent, label, subject)
{
}

void ContainerViewItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("ContainerViewItem::update");
  ViewItem::update(event, type, son);
}
 
// ----------------------------------------------------------------------------

DataTypeViewItem::DataTypeViewItem(ViewItem *parent, QString label, Subject* subject)
  : ViewItem::ViewItem(parent, label, subject)
{
}

void DataTypeViewItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("DataTypeViewItem::update");
  ViewItem::update(event, type, son);
}
 
// ----------------------------------------------------------------------------

ReferenceViewItem::ReferenceViewItem(ViewItem *parent, QString label, Subject* subject)
  : ViewItem::ViewItem(parent, label, subject)
{
}

void ReferenceViewItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("ReferenceViewItem::update");
  ViewItem::update(event, type, son);
}

// ----------------------------------------------------------------------------


/*!
 *
 */

editTree::editTree(YACS::HMI::Subject *context, 
                   QWidget* parent,
                   const char* name,
                   WFlags fl) : wiEditTree(parent, name, fl)
{
  _context = context;
  _parent = parent;
  _root = 0;
  _previousSelected = 0;
  _selectedSubjectOutPort = 0;
  _selectedSubjectNode = 0;
  _selectedSubjectComponent = 0;
  _selectedSubjectService = 0;
  resetTreeNode(lv);
  _context->attach(this);
  connect( lv, SIGNAL(selectionChanged()),
           this, SLOT(select()) );
//   connect( lv, SIGNAL(clicked(QListViewItem *)),
//            this, SLOT(select()) );
//   connect( lv, SIGNAL(selectionChanged(QListViewItem *)),
//            this, SLOT(select(QListViewItem *)) );
//   connect( lv, SIGNAL(clicked(QListViewItem *)),
//            this, SLOT(select(QListViewItem *)) );
}

/*!
 *
 */

editTree::~editTree()
{
}

void editTree::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("editTree::update");
  switch (event)
    {
    case YACS::HMI::NEWROOT:
      setNewRoot(son);
      break;
    default:
      DEBTRACE("editTree::update(), event not handled: "<< event);
    }
}

void editTree::setNewRoot(YACS::HMI::Subject *root)
{
  _root=root;
  _previousSelected=0;
  resetTreeNode(lv);
}

void editTree::addViewItemInMap(YACS::HMI::ViewItem* item, YACS::HMI::Subject* subject)
{
  _viewItemsMap[subject] = item;
}

YACS::HMI::ViewItem* editTree::getViewItem(YACS::HMI::Subject* subject)
{
  return _viewItemsMap[subject];
}

/*!
 *
 */

void editTree::resetTreeNode(QListView *lv)
{
  lv->clear();
  lv->setColumnWidth(0,400);
//   lv->addColumn( "state", 100);
  lv->setRootIsDecorated( TRUE );
  if (_root)
    {
      string name = _root->getName();
      ViewItem *start = new ComposedNodeViewItem(lv, name, _root);
      addViewItemInMap(start, _root);
    }
}

/*!
 *
 */

void editTree::printName()
{
  QListViewItem *it = lv->selectedItem();
  if (it) DEBTRACE("my name is Bond, James Bond... "<< this)
  else DEBTRACE("nobody "<< this);
}

YACS::HMI::Subject* editTree::getSelectedSubject()
{
  Subject *sub = 0;
  QListViewItem *it = lv->selectedItem();
  if (it)
    {
      ViewItem *item = dynamic_cast<ViewItem*> (it);
      if (item) sub = item->getSubject();
    }
  return sub;
}

void editTree::destroy()
{
  DEBTRACE("editTree::destroy");
  Subject *sub =getSelectedSubject();
  if (sub)
    {
      Subject *parent = sub->getParent();
      if (parent)
        {
          parent->destroy(sub);
        }
    }
}

void editTree::addLink()
{
  DEBTRACE("editTree::addLink");
  Subject *sub =getSelectedSubject();
  if (sub)
    {
      if (dynamic_cast<SubjectOutputPort*>(sub) || dynamic_cast<SubjectOutputDataStreamPort*>(sub))
        _selectedSubjectOutPort = static_cast<SubjectDataPort*>(sub);
      else if (dynamic_cast<SubjectNode*>(sub))
        _selectedSubjectNode = static_cast<SubjectNode*>(sub);
    }
}

void editTree::editPortProperties()
{
  DEBTRACE("editTree::editPortProperties");
  Subject *sub =getSelectedSubject();
  if (sub)
    {
      if (SubjectOutputDataStreamPort* subODS = dynamic_cast<SubjectOutputDataStreamPort*>(sub))
        {
          map<string, string> properties = subODS->getProperties();
          properties["prop1"] = "val1";
          properties["prop2"] = "val2";
          subODS->setProperties(properties);
          properties = subODS->getProperties();
          map<string, string>::iterator it;
          for (it = properties.begin(); it !=properties.end(); ++it)
            DEBTRACE("OutputDataStreamPort property " << (*it).first << " " << (*it).second);
        }
      else if (SubjectInputDataStreamPort* subIDS = dynamic_cast<SubjectInputDataStreamPort*>(sub))
        {
          map<string, string> properties = subIDS->getProperties();
          properties["prop1"] = "val1";
          properties["prop2"] = "val2";
          subIDS->setProperties(properties);
          properties = subIDS->getProperties();
          map<string, string>::iterator it;
          for (it = properties.begin(); it !=properties.end(); ++it)
            DEBTRACE("InputDataStreamPort property " << (*it).first << " " << (*it).second);
         }
    }
}

void editTree::addComponent()
{
  DEBTRACE("editTree::addComponent");
  stringstream name;
  name << "component";
  name << "_" << GuiContext::getCurrent()->getNewId();
  ChooseName chooseName("root", "component", name.str());
  chooseName.exec();
  if (chooseName.isOk())
    {
      string theName = chooseName.getChoosenName();
      bool ret = GuiContext::getCurrent()->getSubjectProc()->addComponent(theName);
      if (!ret) QMessageBox::warning(this, "Component not Created", GuiContext::getCurrent()->_lastErrorMessage );
    }
}

void editTree::addContainer()
{
  DEBTRACE("editTree::addContainer");
  std::stringstream name;
  name << "container" << GuiContext::getCurrent()->getNewId();
  ChooseName chooseName("root", "container", name.str());
  chooseName.exec();
  if (chooseName.isOk())
    {
      string theName = chooseName.getChoosenName();
      bool ret = GuiContext::getCurrent()->getSubjectProc()->addContainer(theName);
      if (!ret) QMessageBox::warning(this, "Container not Created", GuiContext::getCurrent()->_lastErrorMessage );
    }
}

void editTree::associateServiceToComponent()
{
  DEBTRACE("editTree::associateServiceToComponent");
  Subject *sub =getSelectedSubject();
  if (sub)
    {
      if (SubjectServiceNode *subserv = dynamic_cast<SubjectServiceNode*>(sub))
        _selectedSubjectService = subserv;
    }
}

void editTree::associateComponentToContainer()
{
  DEBTRACE("editTree::associateComponentToContainer");
  Subject *sub =getSelectedSubject();
  if (sub)
    {
      if (SubjectComponent *subcomp = dynamic_cast<SubjectComponent*>(sub))
        _selectedSubjectComponent = subcomp;
    }
}

void editTree::editContainerProperties()
{
  DEBTRACE("editTree::editContainerProperties");
  Subject *sub =getSelectedSubject();
  if (sub)
    {
      if (SubjectContainer *subcont = dynamic_cast<SubjectContainer*>(sub))
        {
          map<string, string> properties = subcont->getProperties();
          properties["prop1"] = "val1";
          properties["prop2"] = "val2";
          subcont->setProperties(properties);
          properties = subcont->getProperties();
          map<string, string>::iterator it;
          for (it = properties.begin(); it !=properties.end(); ++it)
            DEBTRACE("container property " << (*it).first << " " << (*it).second);
        }
    }
}

void editTree::newNode(int key)
{
  DEBTRACE("editTree::newNode(Catalog* catalog, string type): "<< this);
  YACS::ENGINE::Catalog* catalog = _catalogItemsMap[key].first;
  std::string type = _catalogItemsMap[key].second;
  Subject *sub =getSelectedSubject();
  if (sub)
    {
      YACS::HMI::SubjectComposedNode *subject = dynamic_cast< YACS::HMI::SubjectComposedNode*>(sub);
      if (subject)
        {
          std::stringstream name;
          name << type << GuiContext::getCurrent()->getNewId();
          YACS::HMI::SubjectSwitch *aSwitch = dynamic_cast< YACS::HMI::SubjectSwitch*>(subject);
          if (aSwitch)
            {
              map<int, SubjectNode*> bodyMap = aSwitch->getBodyMap();
              int swCase = 0;
              if (bodyMap.empty()) swCase = 1;
              else
                {
                  map<int, SubjectNode*>::reverse_iterator rit = bodyMap.rbegin();
                  swCase = (*rit).first + 1;
                }
              aSwitch->addNode(catalog, "", type, name.str(), swCase);
            }
          else
            subject->addNode(catalog, "", type, name.str());
        }
    }
  else DEBTRACE("editTree::newNode(Catalog* catalog, string type): no ComposedNode selected  "<< this);
}

void editTree::newNode(YACS::ENGINE::Catalog* catalog,
                       Subject* sub,
                       std::pair<std::string,std::string> compoServ)
{
  DEBTRACE("editTree::newNode(catalog, subject, std::pair<std::string,std::string> compoServ)");
  std::string compo = compoServ.first;
  std::string service = compoServ.second;
  std::stringstream name;
  name << compo << "_" << service << GuiContext::getCurrent()->getNewId();
  YACS::HMI::SubjectComposedNode *subject = dynamic_cast< YACS::HMI::SubjectComposedNode*>(sub);
  if (subject)
    {
      YACS::HMI::SubjectSwitch *aSwitch = dynamic_cast< YACS::HMI::SubjectSwitch*>(subject);
      if (aSwitch)
        {
          map<int, SubjectNode*> bodyMap = aSwitch->getBodyMap();
          int swCase = 0;
          if (bodyMap.empty()) swCase = 1;
          else
            {
              map<int, SubjectNode*>::reverse_iterator rit = bodyMap.rbegin();
              swCase = (*rit).first + 1;
            }
          aSwitch->addNode(catalog, compo, service, name.str(), swCase);
        }
      else
        subject->addNode(catalog, compo, service, name.str());
    }
}

void editTree::newDataType(YACS::ENGINE::Catalog* catalog,
                           Subject* sub,
                           std::string typeName)
{
  DEBTRACE("editTree::newDataType " << sub->getName() << " " << typeName);
  YACS::HMI::SubjectProc *subproc = GuiContext::getCurrent()->getSubjectProc();
  subproc->addDataType(catalog, typeName);
}


void editTree::newInputPort(int key)
{
  DEBTRACE("editTree::newInputPort(Catalog* catalog, string type): "<< this);
  YACS::ENGINE::Catalog* catalog = _catalogItemsMap[key].first;
  std::string type = _catalogItemsMap[key].second;
  Subject *sub =getSelectedSubject();
  newDFInputPort(catalog, sub, type);
}

void editTree::newOutputPort(int key)
{
  DEBTRACE("editTree::newOutputPort(Catalog* catalog, string type): "<< this);
  YACS::ENGINE::Catalog* catalog = _catalogItemsMap[key].first;
  std::string type = _catalogItemsMap[key].second;
  Subject *sub =getSelectedSubject();
  newDFOutputPort(catalog, sub, type);
}

void editTree::newDFInputPort(YACS::ENGINE::Catalog* catalog,
                              Subject* sub,
                              std::string typeName)
{
  if (sub)
    {
      YACS::HMI::SubjectElementaryNode *subject = dynamic_cast< YACS::HMI::SubjectElementaryNode*>(sub);
      if (subject)
        {
          std::stringstream name;
          name << "InDF_" << typeName << GuiContext::getCurrent()->getNewId();
          ChooseName chooseName(subject->getName(), typeName, name.str());
          chooseName.exec();
          if (chooseName.isOk())
            {
              string theName = chooseName.getChoosenName();
              bool ret = subject->addInputPort(catalog, typeName, theName);
              if (!ret) QMessageBox::warning(this, "Input Port not Created", GuiContext::getCurrent()->_lastErrorMessage );
            }
        }
    }
}

void editTree::newDFOutputPort(YACS::ENGINE::Catalog* catalog,
                               Subject* sub,
                               std::string typeName)
{
  if (sub)
    {
      YACS::HMI::SubjectElementaryNode *subject = dynamic_cast< YACS::HMI::SubjectElementaryNode*>(sub);
      if (subject)
        {
          std::stringstream name;
          name << "OutDF_" << typeName << GuiContext::getCurrent()->getNewId();
          ChooseName chooseName(subject->getName(), typeName, name.str());
          chooseName.exec();
          if (chooseName.isOk())
            {
              string theName = chooseName.getChoosenName();
              bool ret = subject->addOutputPort(catalog, typeName, theName);
              if (!ret) QMessageBox::warning(this, "Output Port not Created", GuiContext::getCurrent()->_lastErrorMessage );
            }
        }
    }
}

void editTree::newDSInputPort(YACS::ENGINE::Catalog* catalog,
                              Subject* sub,
                              std::string typeName)
{
  if (sub)
    {
      YACS::HMI::SubjectElementaryNode *subject = dynamic_cast< YACS::HMI::SubjectElementaryNode*>(sub);
      if (subject)
        {
          std::stringstream name;
          name << "InDS_" << typeName << GuiContext::getCurrent()->getNewId();
          ChooseName chooseName(subject->getName(), typeName, name.str());
          chooseName.exec();
          if (chooseName.isOk())
            {
              string theName = chooseName.getChoosenName();
              bool ret = subject->addIDSPort(catalog, typeName, theName);
              if (!ret) QMessageBox::warning(this, "Input Port not Created", GuiContext::getCurrent()->_lastErrorMessage );
            }
        }
    }
}

void editTree::newDSOutputPort(YACS::ENGINE::Catalog* catalog,
                               Subject* sub,
                               std::string typeName)
{
  if (sub)
    {
      YACS::HMI::SubjectElementaryNode *subject = dynamic_cast< YACS::HMI::SubjectElementaryNode*>(sub);
      if (subject)
        {
          std::stringstream name;
          name << "OutDS_" << typeName << GuiContext::getCurrent()->getNewId();
          ChooseName chooseName(subject->getName(), typeName, name.str());
          chooseName.exec();
          if (chooseName.isOk())
            {
              string theName = chooseName.getChoosenName();
              bool ret = subject->addODSPort(catalog, typeName, theName);
              if (!ret) QMessageBox::warning(this, "Output Port not Created", GuiContext::getCurrent()->_lastErrorMessage );
            }
        }
    }
}


void editTree::newDFInputPort()
{
  DEBTRACE("editTree::newDFInputPort");
  Subject *sub =getSelectedSubject();
  if (sub)
    {
      YACS::HMI::SubjectElementaryNode *subject = dynamic_cast< YACS::HMI::SubjectElementaryNode*>(sub);
      if (subject)
        catalog(CATALOGINPUTPORT);
    }

}

void editTree::newDFOutputPort()
{
  DEBTRACE("editTree::newDFOutputPort");
  Subject *sub =getSelectedSubject();
  if (sub)
    {
      YACS::HMI::SubjectElementaryNode *subject = dynamic_cast< YACS::HMI::SubjectElementaryNode*>(sub);
      if (subject)
        catalog(CATALOGOUTPUTPORT);
    }
}
void editTree::newDSInputPort()
{
  DEBTRACE("editTree::newDSInputPort");
  Subject *sub =getSelectedSubject();
  if (sub)
    {
      YACS::HMI::SubjectElementaryNode *subject = dynamic_cast< YACS::HMI::SubjectElementaryNode*>(sub);
      if (subject)
        catalog(CATALOGIDSPORT);
    }

}

void editTree::newDSOutputPort()
{
  DEBTRACE("editTree::newDSOutputPort");
  Subject *sub =getSelectedSubject();
  if (sub)
    {
      YACS::HMI::SubjectElementaryNode *subject = dynamic_cast< YACS::HMI::SubjectElementaryNode*>(sub);
      if (subject)
        catalog(CATALOGODSPORT);
    }
}

void editTree::catalog(int cataType)
{
  Subject *sub =getSelectedSubject();
  if (sub)
    {
      BrowseCatalog *brCat = new BrowseCatalog(this, sub, cataType);
      brCat->exec();
    }
}

void editTree::setCatalog(int isproc)
{
  Subject *sub =getSelectedSubject();
  if (sub)
    {
      if (isproc)
        {
          QString fn = QFileDialog::getOpenFileName( QString::null,
                                                     tr( "XML-Files (*.xml);;All Files (*)" ),
                                                     this,
                                                     "load YACS scheme file dialog as catalog",
                                                     "Choose a filename to load"  );
          if ( !fn.isEmpty() )
            {
              YACS::ENGINE::Catalog* aCatalog = YACS::ENGINE::getSALOMERuntime()->loadCatalog( "proc", fn.latin1());
              GuiContext::getCurrent()->setProcCatalog(aCatalog);
              GuiContext::getCurrent()->setCurrentCatalog(aCatalog);
            }
        }
      else
        {
          GuiContext::getCurrent()->setCurrentCatalog(GuiContext::getCurrent()->getSessionCatalog());
        }
    }
}


/*!
 *
 */

void editTree::select()
{
//   DEBTRACE("editTree::select() "<< this);
  QListViewItem *it =lv->selectedItem();
  ViewItem *item = 0;
  if (it)                          // an item selected
    {
      if (it != _previousSelected) // a new item is selected
        {
          _previousSelected = it;
          item = dynamic_cast<ViewItem*> (it);
        }
    }
  else if (_previousSelected)      // nothing selected, deselect previous
    {
      it = _previousSelected;
      _previousSelected = 0;
      item = dynamic_cast<ViewItem*> (it);
    }
  if (item)
    {
        item->getSubject()->select(it->isSelected());
        if (_selectedSubjectOutPort)
          {
            Subject *sub = item->getSubject();
            if (dynamic_cast<SubjectInputPort*>(sub) || dynamic_cast<SubjectInputDataStreamPort*>(sub))
              {
                SubjectDataPort *sdp = dynamic_cast<SubjectDataPort*>(sub);
                assert(sdp);
                SubjectDataPort::tryCreateLink(_selectedSubjectOutPort, sdp);
                _selectedSubjectOutPort = 0;
              }
          }
        if (_selectedSubjectNode)
          {
            Subject *sub = item->getSubject();
            if (SubjectNode* subNode = dynamic_cast<SubjectNode*>(sub))
              {
                SubjectNode::tryCreateLink(_selectedSubjectNode, subNode);
                _selectedSubjectNode = 0;
              }
          }
        if (_selectedSubjectService)
          {
            Subject *sub = item->getSubject();
            if (SubjectComponent *subcomp = dynamic_cast<SubjectComponent*>(sub))
              {
                _selectedSubjectService->associateToComponent(subcomp);
                _selectedSubjectService = 0;
              }
          }
        if (_selectedSubjectComponent)
          {
            Subject *sub = item->getSubject();
            if (SubjectContainer *subcont = dynamic_cast<SubjectContainer*>(sub))
              {
                _selectedSubjectComponent->associateToContainer(subcont);
                _selectedSubjectComponent = 0;
              }
          }
    }
}

/*!
 *
 */

void editTree::select(QListViewItem *it)
{
  DEBTRACE("editTree::select(QListViewItem *item) "<< this);
  ViewItem *item = dynamic_cast<ViewItem*> (it);
  if (item) item->getSubject()->select(it->isSelected());
}

/*!
 *
 */

void editTree::contextMenuEvent( QContextMenuEvent * )
{
  QListViewItem *it =lv->selectedItem();
  ViewItem *item = 0;
  if (it)
    {
      if (item = dynamic_cast<ComposedNodeViewItem*> (it))
        ComposedNodeContextMenu();
      else if (item = dynamic_cast<NodeViewItem*> (it))
        NodeContextMenu();
      else if (item = dynamic_cast<PortViewItem*> (it))
        PortContextMenu();
      else if (item = dynamic_cast<LinkViewItem*> (it))
        LinkContextMenu();
      else if (item = dynamic_cast<ComponentViewItem*> (it))
        ComponentContextMenu();
      else if (item = dynamic_cast<ContainerViewItem*> (it))
        ContainerContextMenu();
    }
}

/*!
 *
 */

void editTree::ComposedNodeContextMenu()
{
  DEBTRACE("editTree::ComposedNodeContextMenu");
  QPopupMenu*	contextMenu = new QPopupMenu();
  Q_CHECK_PTR( contextMenu );
  QLabel *caption = new QLabel( "<font color=darkblue><u><b>"
                                "ComposedNode Context Menu</b></u></font>",
                                contextMenu );
  caption->setAlignment( Qt::AlignCenter );
  int id;
  contextMenu->insertItem( caption );
  contextMenu->insertItem( "Message", this, SLOT(printName()) );
  contextMenu->insertItem( "Delete", this, SLOT(destroy()) );
  contextMenu->insertItem( "add control link to other node", this, SLOT(addLink()) );
  _keymap = 0;
  _catalogItemsMap.clear();
  YACS::ENGINE::Catalog *builtinCatalog = YACS::ENGINE::getSALOMERuntime()->getBuiltinCatalog();
  {
    map<string,YACS::ENGINE::Node*>::iterator it;
    for (it=builtinCatalog->_nodeMap.begin(); it!=builtinCatalog->_nodeMap.end(); ++it)
      {
        string nodeType = "new " + (*it).first;
        //DEBTRACE(nodeType);
        id =contextMenu->insertItem( nodeType.c_str(), this,
                                         SLOT(newNode(int)) );
        std::pair<YACS::ENGINE::Catalog*, std::string> p(builtinCatalog, (*it).first);
        _catalogItemsMap[_keymap] = p;
        contextMenu->setItemParameter(id, _keymap++);
      }
  }
  {
    map<string,YACS::ENGINE::ComposedNode*>::iterator it;
    for (it=builtinCatalog->_composednodeMap.begin(); it!=builtinCatalog->_composednodeMap.end(); ++it)
      {
        string nodeType = "new " + (*it).first;
        //DEBTRACE(nodeType);
        id =contextMenu->insertItem( nodeType.c_str(), this,
                                         SLOT(newNode(int)) );
        std::pair<YACS::ENGINE::Catalog*, std::string> p(builtinCatalog, (*it).first);
        _catalogItemsMap[_keymap] = p;
        contextMenu->setItemParameter(id, _keymap++);
      }
  }
  id = contextMenu->insertItem( "set current catalog to session", this, SLOT(setCatalog(int)) );
  contextMenu->setItemParameter(id, 0);
  id = contextMenu->insertItem( "set current catalog to proc", this, SLOT(setCatalog(int)) );
  contextMenu->setItemParameter(id, 1);
  id = contextMenu->insertItem( "new node from current Catalog", this, SLOT(catalog(int)) );
  contextMenu->setItemParameter(id, CATALOGNODE);
  contextMenu->exec( QCursor::pos() );
  delete contextMenu;
}

/*!
 *
 */

void editTree::NodeContextMenu()
{
  QPopupMenu*	contextMenu = new QPopupMenu();
  Q_CHECK_PTR( contextMenu );
  QLabel *caption = new QLabel( "<font color=darkblue><u><b>"
                                "Elementary Node Context Menu</b></u></font>",
                                contextMenu );
  caption->setAlignment( Qt::AlignCenter );
  contextMenu->insertItem( caption );
  contextMenu->insertItem( "Message", this, SLOT(printName()) );
  contextMenu->insertItem( "Delete", this, SLOT(destroy()) );
  contextMenu->insertItem( "add Component", this, SLOT(addComponent()) );
  contextMenu->insertItem( "associate to Component", this, SLOT(associateServiceToComponent()) );
  contextMenu->insertItem( "add control link to other node", this, SLOT(addLink()) );
  _keymap = 0;
  _catalogItemsMap.clear();
  YACS::ENGINE::Catalog *builtinCatalog = YACS::ENGINE::getSALOMERuntime()->getBuiltinCatalog();
  {
    int id;
    map<string,YACS::ENGINE::TypeCode*>::iterator it;
    for (it=builtinCatalog->_typeMap.begin(); it!=builtinCatalog->_typeMap.end(); ++it)
      {
        std::pair<YACS::ENGINE::Catalog*, std::string> p(builtinCatalog, (*it).first);
        {
          string typeCode = "new input port " + (*it).first;
          DEBTRACE(typeCode);
          id =contextMenu->insertItem( typeCode.c_str(), this,
                                       SLOT(newInputPort(int)) );
          _catalogItemsMap[_keymap] = p;
          contextMenu->setItemParameter(id, _keymap++);
        }
        {
          string typeCode = "new output port " + (*it).first;
          DEBTRACE(typeCode);
          id =contextMenu->insertItem( typeCode.c_str(), this,
                                       SLOT(newOutputPort(int)) );
          _catalogItemsMap[_keymap] = p;
          contextMenu->setItemParameter(id, _keymap++);
        }
      }
    id = contextMenu->insertItem( "set current catalog to session", this, SLOT(setCatalog(int)) );
    contextMenu->setItemParameter(id, 0);
    id = contextMenu->insertItem( "set current catalog to proc", this, SLOT(setCatalog(int)) );
    contextMenu->setItemParameter(id, 1);
    id = contextMenu->insertItem( "new InputPort from catalog", this, SLOT(newDFInputPort()));
    id = contextMenu->insertItem( "new OutputPort from catalog", this, SLOT(newDFOutputPort()));
    id = contextMenu->insertItem( "new DataStream InputPort from catalog", this, SLOT(newDSInputPort()));
    id = contextMenu->insertItem( "new DataStream OutputPort from catalog", this, SLOT(newDSOutputPort()));
    id = contextMenu->insertItem( "add Data Type from current Catalog", this, SLOT(catalog(int)) );
    contextMenu->setItemParameter(id, CATALOGDATATYPE);
  }
  contextMenu->exec( QCursor::pos() );
  delete contextMenu;
}

void editTree::PortContextMenu()
{
  QPopupMenu*	contextMenu = new QPopupMenu();
  Q_CHECK_PTR( contextMenu );
  QLabel *caption = new QLabel( "<font color=darkblue><u><b>"
                                "Port Context Menu</b></u></font>",
                                contextMenu );
  caption->setAlignment( Qt::AlignCenter );
  contextMenu->insertItem( caption );
  contextMenu->insertItem( "Message", this, SLOT(printName()) );
  contextMenu->insertItem( "Delete", this, SLOT(destroy()) );
  contextMenu->insertItem( "Add link", this, SLOT(addLink()) );
  contextMenu->insertItem( "edit properties", this, SLOT(editPortProperties()) );
  contextMenu->exec( QCursor::pos() );
  delete contextMenu;
}

void editTree::LinkContextMenu()
{
  QPopupMenu*	contextMenu = new QPopupMenu();
  Q_CHECK_PTR( contextMenu );
  QLabel *caption = new QLabel( "<font color=darkblue><u><b>"
                                "Link Context Menu</b></u></font>",
                                contextMenu );
  caption->setAlignment( Qt::AlignCenter );
  contextMenu->insertItem( caption );
  contextMenu->insertItem( "Message", this, SLOT(printName()) );
  contextMenu->insertItem( "Delete", this, SLOT(destroy()) );
  contextMenu->exec( QCursor::pos() );
  delete contextMenu;
}

void  editTree::ComponentContextMenu()
{
  QPopupMenu*	contextMenu = new QPopupMenu();
  Q_CHECK_PTR( contextMenu );
  QLabel *caption = new QLabel( "<font color=darkblue><u><b>"
                                "Component Context Menu</b></u></font>",
                                contextMenu );
  caption->setAlignment( Qt::AlignCenter );
  contextMenu->insertItem( caption );
  contextMenu->insertItem( "Message", this, SLOT(printName()) );
  contextMenu->insertItem( "add Container", this, SLOT(addContainer()) );
  contextMenu->insertItem( "associate to Container", this, SLOT(associateComponentToContainer()) );
  contextMenu->exec( QCursor::pos() );
  delete contextMenu;
}

void  editTree::ContainerContextMenu()
{
  QPopupMenu*	contextMenu = new QPopupMenu();
  Q_CHECK_PTR( contextMenu );
  QLabel *caption = new QLabel( "<font color=darkblue><u><b>"
                                "Container Context Menu</b></u></font>",
                                contextMenu );
  caption->setAlignment( Qt::AlignCenter );
  contextMenu->insertItem( caption );
  contextMenu->insertItem( "Message", this, SLOT(printName()) );
  contextMenu->insertItem( "edit container properties", this, SLOT(editContainerProperties()) );
  contextMenu->exec( QCursor::pos() );
  delete contextMenu;
}
