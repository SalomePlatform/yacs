
#ifndef _EDITIONSALOMENODE_HXX_
#define _EDITIONSALOMENODE_HXX_

#include "EditionElementaryNode.hxx"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

class FormComponent;
class FormContainer;

namespace YACS
{
  namespace ENGINE
  {
    class ServiceNode;
  }

  namespace HMI
  {
      // ports en cablage plus initialisation seulement
      // choix de l'instance de composant
      // panneau depliable instance de composant
      // panneau depliable container

    class EditionSalomeNode: public EditionElementaryNode
    {
      Q_OBJECT

    public:
      EditionSalomeNode(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionSalomeNode();
      virtual void update(GuiEvent event, int type, Subject* son);

    public slots:
      virtual void fillComponentPanel();
      virtual void fillContainerPanel();
      virtual void changeInstance(int index);
      virtual void changeContainer(int index);
      virtual void changeHost(int index);

    protected:
      FormComponent *_wComponent;
      FormContainer *_wContainer;
      QHBoxLayout *_hbl_method;
      QLabel *_la_method;
      QLineEdit *_le_method;
      YACS::ENGINE::ServiceNode* _servNode;
    };
  }
}
#endif
