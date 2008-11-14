
#ifndef _EDITIONNODE_HXX_
#define _EDITIONNODE_HXX_

#include "ItemEdition.hxx"
#include <QLineEdit>

namespace YACS
{

  namespace HMI
  {
    class SubjectNode;

    class EditionNode: public ItemEdition
    {
      Q_OBJECT

    public slots:

    public:
      EditionNode(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionNode();
      virtual void update(GuiEvent event, int type, Subject* son);

    protected:
      QLineEdit *_le_fullName;
      SubjectNode *_subjectNode;
    };
  }
}
#endif
