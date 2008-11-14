
/*!
 *  only one declaration in src/genericgui,
 *  implementations in salomewrap and standalonegui
 */
    
#include "WrapGraphicsView.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;


WrapGraphicsView::WrapGraphicsView(QWidget *parent)
  : QGraphicsView(parent)
{
  DEBTRACE("WrapGraphicsView::WrapGraphicsView");
}

WrapGraphicsView::~WrapGraphicsView()
{
  DEBTRACE("WrapGraphicsView::~WrapGraphicsView");
}

void WrapGraphicsView::onViewFitAll()
{
  DEBTRACE("WrapGraphicsView::onViewFitAll");
}

void WrapGraphicsView::onViewFitArea()
{
  DEBTRACE("WrapGraphicsView::onViewFitArea");
}

void WrapGraphicsView::onViewZoom()
{
  DEBTRACE("WrapGraphicsView::onViewZoom");
}

void WrapGraphicsView::onViewPan()
{
  DEBTRACE("WrapGraphicsView::onViewPan");
}

void WrapGraphicsView::onViewGlobalPan()
{
  DEBTRACE("WrapGraphicsView::onViewGlobalPan");
}

void WrapGraphicsView::onViewReset()
{
  DEBTRACE("WrapGraphicsView::onViewReset");
}

