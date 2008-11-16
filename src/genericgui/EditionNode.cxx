
#include "EditionNode.hxx"
#include "QtGuiContext.hxx"
#include "Proc.hxx"

#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

EditionNode::EditionNode(Subject* subject,
                         QWidget* parent,
                         const char* name)
  : ItemEdition(subject, parent, name)
{
  DEBTRACE("EditionNode::EditionNode");
  _subjectNode = dynamic_cast<SubjectNode*>(_subject);
  assert(_subjectNode);

  _le_fullName = 0;
  if (!dynamic_cast<SubjectProc*>(_subject))
    {
      _le_fullName = new QLineEdit(this);
      _le_fullName->setToolTip("Full name of the node in schema");
      _le_fullName->setReadOnly(true);
      _wid->gridLayout1->addWidget(_le_fullName);

      YACS::ENGINE::Proc* proc = QtGuiContext::getQtCurrent()->getProc();
      _le_fullName->setText((proc->getChildName(_subjectNode->getNode())).c_str());
    }
}

EditionNode::~EditionNode()
{
}

void EditionNode::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("EditionNode::update ");
  ItemEdition::update(event, type, son);
  YACS::ENGINE::Proc* proc = QtGuiContext::getQtCurrent()->getProc();
  switch (event)
    {
    case RENAME:
      if (_le_fullName)
        _le_fullName->setText((proc->getChildName(_subjectNode->getNode())).c_str());
      break;
    default:
      ;
    }
}
