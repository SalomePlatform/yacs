
#ifndef _EDITIONFOREACHLOOP_HXX_
#define _EDITIONFOREACHLOOP_HXX_

#include "EditionNode.hxx"

namespace YACS
{
  namespace HMI
  {
    class FormEachLoop;

    class EditionForEachLoop: public EditionNode
    {
      Q_OBJECT

    public slots:
      virtual void onModifyNbBranches(const QString &text);

    public:
      EditionForEachLoop(Subject* subject,
                         QWidget* parent = 0,
                         const char* name = 0);
      virtual ~EditionForEachLoop();
      virtual void synchronize();
      virtual void update(GuiEvent event, int type, Subject* son);

    protected:
      FormEachLoop *_formEachLoop;
    };
  }
}
#endif
