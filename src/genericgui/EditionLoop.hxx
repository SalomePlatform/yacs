
#ifndef _EDITIONLOOP_HXX_
#define _EDITIONLOOP_HXX_

#include "EditionNode.hxx"

namespace YACS
{
  namespace HMI
  {

    class EditionLoop: public EditionNode
    {
    public:
      EditionLoop(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionLoop();
      // choix du noeud body
    };
  }
}
#endif
