
#ifndef _EDITIONSTUDYOUTNODE_HXX_
#define _EDITIONSTUDYOUTNODE_HXX_

#include "EditionOutNode.hxx"

namespace YACS
{
  namespace HMI
  {

    class EditionStudyOutNode: public EditionOutNode
    {
    public:
      EditionStudyOutNode(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionStudyOutNode();
      // selection d'etude ?
    };
  }
}
#endif
