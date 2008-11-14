
#ifndef _EDITIONOUTPUTPORT_HXX_
#define _EDITIONOUTPUTPORT_HXX_

#include "ItemEdition.hxx"

namespace YACS
{
  namespace HMI
  {

    class EditionOutputPort: public ItemEdition
    {
    public:
      EditionOutputPort(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionOutputPort();
      // changement de type ?
      // edition valeur initiale (noeud in)
    };
  }
}
#endif
