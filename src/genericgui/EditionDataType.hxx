
#ifndef _EDITIONDATATYPE_HXX_
#define _EDITIONDATATYPE_HXX_

#include "ItemEdition.hxx"

namespace YACS
{
  namespace HMI
  {

    class EditionDataType: public ItemEdition
    {
    public:
      EditionDataType(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionDataType();
      // editeur xml de creation de nouveaux types
    };
  }
}
#endif
