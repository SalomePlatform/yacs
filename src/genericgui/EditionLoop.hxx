
#ifndef _EDITIONLOOP_HXX_
#define _EDITIONLOOP_HXX_

#include "EditionNode.hxx"

namespace YACS
{
  namespace HMI
  {
    class FormLoop;

    class EditionLoop: public EditionNode
    {
      Q_OBJECT

    public slots:
      virtual void onModifyNbSteps(const QString &text);

    public:
      EditionLoop(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionLoop();
      virtual void synchronize();
      virtual void update(GuiEvent event, int type, Subject* son);

    protected:
      FormLoop *_formLoop;
    };
  }
}
#endif
