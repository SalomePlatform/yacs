
#ifndef _SCENEPORTITEM_HXX_
#define _SCENEPORTITEM_HXX_

#include "SceneObserverItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SceneTextItem;

    class ScenePortItem
    {
    public:
      ScenePortItem(QString label);
      virtual ~ScenePortItem();
      virtual void setText(QString label)=0;
      static int getPortWidth();
      static int getPortHeight();
    protected:
      static const int _portWidth;
      static const int _portHeight;
      SceneTextItem* _text;
    };
  }
}

#endif
