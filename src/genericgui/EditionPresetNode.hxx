
#ifndef _EDITIONPRESETNODE_HXX_
#define _EDITIONPRESETNODE_HXX_

#include "EditionElementaryNode.hxx"

namespace YACS
{
  namespace HMI
  {
      // restriction aux output
      // fichier d'entree ?

    class EditionPresetNode: public EditionElementaryNode
    {
      Q_OBJECT

    public:
      EditionPresetNode(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionPresetNode();

    protected:
      virtual bool hasInputPorts();
    };
  }
}
#endif
