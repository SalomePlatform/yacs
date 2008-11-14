
#include "SchemaComponentItem.hxx"
#include "QtGuiContext.hxx"
#include "SchemaModel.hxx"
#include "SchemaReferenceItem.hxx"
#include "guiObservers.hxx"

#include <QIcon>
#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;


SchemaComponentItem::SchemaComponentItem(SchemaItem *parent, QString label, Subject* subject)
  : SchemaItem::SchemaItem(parent, label, subject)
{
  _itemDeco.replace(YLabel, QIcon("icons:component.png"));
}

void SchemaComponentItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SchemaComponentItem::update");
  //SchemaItem::update(event, type, son);
  SchemaModel *model = 0;
  SchemaReferenceItem *item =0;
  switch (event)
    {
    case YACS::HMI::ADDCHILDREF:
      {
        DEBTRACE("ADDCHILDREF ");
        model = QtGuiContext::getQtCurrent()->getSchemaModel();
        SubjectReference *ref = dynamic_cast<SubjectReference*>(son);
        assert(ref);
        SubjectServiceNode *service = dynamic_cast<SubjectServiceNode*>(ref->getReference());
        assert(service);
        YACS::ENGINE::Proc* proc = GuiContext::getCurrent()->getProc();
        string serviceName = proc->getChildName(service->getNode());
        DEBTRACE("ADDCHILDREF " << ref->getReference()->getName());

        int nbsons = childCount();
        model->beginInsertRows(modelIndex(), nbsons, nbsons);
        item = new SchemaReferenceItem(this,
                                       serviceName.c_str(),
                                       son);
        model->endInsertRows();        
      }
      break;

    case YACS::HMI::REMOVECHILDREF:
      {
        DEBTRACE("REMOVECHILDREF ");
      }
      break;

    case YACS::HMI::CUT:
      {
        DEBTRACE("CUT on " << getSubject()->getName());
        SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
        SubjectReference *ref = dynamic_cast<SubjectReference*>(son);
        assert(ref);
        DEBTRACE("CUT " << ref->getReference()->getName());
        SchemaItem *toMove = QtGuiContext::getQtCurrent()->_mapOfSchemaItem[ref];

        int position = toMove->row();
        model->beginRemoveRows(modelIndex(), position, position);
        removeChild(toMove);
        model->endRemoveRows();
      }
      break;

    case YACS::HMI::PASTE:
      {
        DEBTRACE("PASTE on " << getSubject()->getName());
        SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
        SubjectReference *ref = dynamic_cast<SubjectReference*>(son);
        assert(ref);
        DEBTRACE("PASTE " << ref->getReference()->getName());
        SchemaItem *toPaste = QtGuiContext::getQtCurrent()->_mapOfSchemaItem[ref];

        int nbsons = childCount();
        model->beginInsertRows(modelIndex(), nbsons, nbsons);
        toPaste->reparent(this);
        model->endInsertRows();
      }
      break;


    default:
      ;
    }
}
