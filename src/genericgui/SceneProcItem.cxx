
#include "SceneProcItem.hxx"
#include "Menus.hxx"


//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

SceneProcItem::SceneProcItem(QGraphicsScene *scene, SceneItem *parent,
                             QString label, Subject *subject)
  : SceneComposedNodeItem(scene, parent, label, subject)
{
  DEBTRACE("SceneProcItem::SceneProcItem " <<label.toStdString());
//   _height = 2000;
//   _width = 2000;
//   setGeometryOptimization(false);
}

SceneProcItem::~SceneProcItem()
{
}

void SceneProcItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  ProcMenu m;
  m.popupMenu(caller, globalPos);
}
