
#ifndef _EDITIONDATALINK_HXX_
#define _EDITIONDATALINK_HXX_

#include "ItemEdition.hxx"

namespace YACS
{
  namespace HMI
  {

    class EditionDataLink: public ItemEdition
    {
    public:
      EditionDataLink(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionDataLink();
      // informations
    };
  }
}
#endif
