
#ifndef _EDITIONCOMPONENT_HXX_
#define _EDITIONCOMPONENT_HXX_

#include "ItemEdition.hxx"

namespace YACS
{
  namespace HMI
  {

    class EditionComponent: public ItemEdition
    {
    public:
      EditionComponent(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionComponent();
    };
  }
}
#endif
