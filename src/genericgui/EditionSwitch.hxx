
#ifndef _EDITIONSWITCH_HXX_
#define _EDITIONSWITCH_HXX_

#include "EditionBloc.hxx"

namespace YACS
{
  namespace HMI
  {
    class TableSwitch;

    class EditionSwitch: public EditionBloc
    {
    public:
      EditionSwitch(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionSwitch();
      virtual void synchronize();
      
    protected:
      TableSwitch *_tvSwitch;
    };
  }
}
#endif
