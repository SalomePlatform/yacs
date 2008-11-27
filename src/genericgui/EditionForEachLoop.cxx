
#include "EditionForEachLoop.hxx"
#include "FormEachLoop.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <cassert>
#include <sstream>

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

EditionForEachLoop::EditionForEachLoop(Subject* subject,
                                       QWidget* parent,
                                       const char* name)
  : EditionNode(subject, parent, name)
{
  _formEachLoop = new FormEachLoop(this);
  _wid->gridLayout1->addWidget(_formEachLoop);
  _formEachLoop->sb_nbranch->setMinimum(1);
  _formEachLoop->sb_nbranch->setMaximum(INT_MAX);
  connect(_formEachLoop->sb_nbranch, SIGNAL(valueChanged(const QString &)),
          this, SLOT(onModifyNbBranches(const QString &)));
}

EditionForEachLoop::~EditionForEachLoop()
{
}

void EditionForEachLoop::onModifyNbBranches(const QString &text)
{
  SubjectForEachLoop *sfe = dynamic_cast<SubjectForEachLoop*>(_subject);
  assert(sfe);
  sfe->setNbBranches(text.toStdString());
}

void EditionForEachLoop::synchronize()
{
  _subject->update(SETVALUE, 0, _subject);
}

void EditionForEachLoop::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("EditionForEachLoop::update " <<event << " " << type);
  EditionNode::update(event, type, son);
  switch (event)
    {
    case SETVALUE:
      SubjectComposedNode * scn = dynamic_cast<SubjectComposedNode*>(_subject);
      string val = scn->getValue();
      istringstream ss(val);
      DEBTRACE( val);
      int i = 0;
      ss >> i;
      DEBTRACE(i);
      _formEachLoop->sb_nbranch->setValue(i);
      break;
    }
}
