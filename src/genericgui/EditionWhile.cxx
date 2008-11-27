
#include "EditionWhile.hxx"
#include "FormLoop.hxx"
#include "guiObservers.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <cassert>
#include <sstream>

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

EditionWhile::EditionWhile(Subject* subject,
                         QWidget* parent,
                         const char* name)
  : EditionNode(subject, parent, name)
{
  _formWhile = new FormLoop(this);
  _wid->gridLayout1->addWidget(_formWhile);
  _formWhile->sb_nsteps->setMinimum(0);
  _formWhile->sb_nsteps->setMaximum(1);
  _formWhile->label->setText("Condition");
  connect(_formWhile->sb_nsteps, SIGNAL(valueChanged(const QString &)),
          this, SLOT(onModifyCondition(const QString &)));
}

EditionWhile::~EditionWhile()
{
}

void EditionWhile::onModifyCondition(const QString &text)
{
  DEBTRACE("EditionWhile::onModifyCondition " << text.toStdString());
  SubjectWhileLoop *swl = dynamic_cast<SubjectWhileLoop*>(_subject);
  assert(swl);
  swl->setCondition(text.toStdString());
}

void EditionWhile::synchronize()
{
  _subject->update(SETVALUE, 0, _subject);
}

void EditionWhile::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("EditionWhile::update " <<event << " " << type);
  EditionNode::update(event, type, son);
  switch (event)
    {
    case SETVALUE:
      SubjectComposedNode * scn = dynamic_cast<SubjectComposedNode*>(_subject);
      string val = scn->getValue();
      if (val == "True") val = "1";
      else if (val == "False") val = "0";
      DEBTRACE(val);
      istringstream ss(val);
      bool i = 0;
      ss >> i;
      DEBTRACE(i);
      _formWhile->sb_nsteps->setValue(i);
      break;
    }
}
