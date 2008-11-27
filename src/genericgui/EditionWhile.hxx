
#ifndef _EDITIONWHILE_HXX_
#define _EDITIONWHILE_HXX_

#include "EditionNode.hxx"

namespace YACS
{
  namespace HMI
  {
    class FormLoop;

    class EditionWhile: public EditionNode
    {
      Q_OBJECT

    public slots:
      virtual void onModifyCondition(const QString &text);

    public:
      EditionWhile(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionWhile();
      virtual void synchronize();
      virtual void update(GuiEvent event, int type, Subject* son);

    protected:
      FormLoop *_formWhile;
    };
  }
}
#endif
