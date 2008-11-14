
#ifndef _EDITIONCONTROLLINK_HXX_
#define _EDITIONCONTROLLINK_HXX_

#include "ItemEdition.hxx"

namespace YACS
{
  namespace HMI
  {

    class EditionControlLink: public ItemEdition
    {
    public:
      EditionControlLink(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionControlLink();
      // informations
    };
  }
}
#endif
