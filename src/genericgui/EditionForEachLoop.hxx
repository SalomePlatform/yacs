
#ifndef _EDITIONFOREACHLOOP_HXX_
#define _EDITIONFOREACHLOOP_HXX_

#include "EditionNode.hxx"

namespace YACS
{
  namespace HMI
  {

    class EditionForEachLoop: public EditionNode
    {
    public:
      EditionForEachLoop(Subject* subject,
                         QWidget* parent = 0,
                         const char* name = 0);
      virtual ~EditionForEachLoop();
    };
  }
}
#endif
