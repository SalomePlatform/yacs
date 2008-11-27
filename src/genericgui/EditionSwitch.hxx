
#ifndef _EDITIONSWITCH_HXX_
#define _EDITIONSWITCH_HXX_

#include "EditionBloc.hxx"

#include <QString>

namespace YACS
{
  namespace HMI
  {
    class TableSwitch;

    class EditionSwitch: public EditionBloc
    {
      Q_OBJECT

    public slots:
      virtual void onModifySelect(const QString &text);

    public:
      EditionSwitch(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionSwitch();
      virtual void synchronize();
      virtual void update(GuiEvent event, int type, Subject* son);
      
    protected:
      TableSwitch *_tvSwitch;
    };
  }
}
#endif
