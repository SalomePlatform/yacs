
#ifndef _EDITIONOUTNODE_HXX_
#define _EDITIONOUTNODE_HXX_

#include "EditionElementaryNode.hxx"

namespace YACS
{
  namespace HMI
  {
      // restriction aux input
      // fichier de sortie ?

    class EditionOutNode: public EditionElementaryNode
    {
      Q_OBJECT

    public:
      EditionOutNode(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionOutNode();

    protected:
      virtual bool hasOutputPorts();
    };
  }
}
#endif
