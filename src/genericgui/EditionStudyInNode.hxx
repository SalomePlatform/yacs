
#ifndef _EDITIONSTUDYINNODE_HXX_
#define _EDITIONSTUDYINNODE_HXX_

#include "EditionPresetNode.hxx"

namespace YACS
{
  namespace HMI
  {

    class EditionStudyInNode: public EditionPresetNode
    {
    public:
      EditionStudyInNode(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionStudyInNode();
      // selection d'etude ?
    };
  }
}
#endif
