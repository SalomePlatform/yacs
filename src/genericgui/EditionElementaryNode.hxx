
#ifndef _EDITIONELEMENTARYNODE_HXX_
#define _EDITIONELEMENTARYNODE_HXX_

#include "EditionNode.hxx"

#include <QTabWidget>
#include <QTableView>
#include <QWidget>

namespace YACS
{
  namespace HMI
  {

      // ports input et output, creation, modification, cablage + init
      // panneau depliable sur le port selectionne?
      // panneau depliable sur le type du port selectionne?

    class ValueDelegate;

    class EditionElementaryNode: public EditionNode
    {
      Q_OBJECT

    public slots:
      virtual void onApply();
      virtual void onCancel();
      virtual void onPortIndexChanged(int index);
      virtual void onCommitData(QWidget *editor);

    public:
      EditionElementaryNode(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionElementaryNode();
      virtual void synchronize();
      virtual void setEditablePorts(bool isEditable);

    protected:
      virtual bool hasInputPorts();
      virtual bool hasOutputPorts();
      virtual void createTablePorts();
      SubjectElementaryNode *_subElemNode;
      QTabWidget* _twPorts;
      TablePortsEdition* _tvInPorts;
      TablePortsEdition* _tvOutPorts;
      ValueDelegate* _valueDelegate;
    };
  }
}
#endif
