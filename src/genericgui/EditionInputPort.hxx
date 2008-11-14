
#ifndef _EDITIONINPUTPORT_HXX_
#define _EDITIONINPUTPORT_HXX_

#include "ItemEdition.hxx"

namespace YACS
{
  namespace HMI
  {

    class EditionInputPort: public ItemEdition
    {
    public:
      EditionInputPort(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionInputPort();
      // changement de type ?
      // edition valeur initiale
    };
  }
}
#endif
