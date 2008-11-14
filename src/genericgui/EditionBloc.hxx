
#ifndef _EDITIONBLOC_HXX_
#define _EDITIONBLOC_HXX_

#include "EditionNode.hxx"

namespace YACS
{
  namespace HMI
  {

    class EditionBloc: public EditionNode
    {
    public:
      EditionBloc(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionBloc();
      // liste de nodes fils
    };
  }
}
#endif
