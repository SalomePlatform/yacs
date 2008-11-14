
#ifndef _EDITIONCONTAINER_HXX_
#define _EDITIONCONTAINER_HXX_

#include "ItemEdition.hxx"

class FormContainer;

namespace YACS
{
  namespace HMI
  {

    class EditionContainer: public ItemEdition
    {
      Q_OBJECT

    public:
      EditionContainer(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionContainer();
      virtual void update(GuiEvent event, int type, Subject* son);

    public slots:
      virtual void fillContainerPanel();

    protected:
      FormContainer *_wContainer;
    };
  }
}
#endif
