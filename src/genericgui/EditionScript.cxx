//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "EditionScript.hxx"
#include "QtGuiContext.hxx"

#include "InlineNode.hxx"

#if (defined HAS_QSCI4 == 1)
#include <qsciscintilla.h>
#include <qscilexerpython.h>
#endif

#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

#if (defined HAS_QSCI4 == 1)
class myQsciScintilla: public QsciScintilla
{
public:
  myQsciScintilla(QWidget *parent=0)
    : QsciScintilla(parent) {};
  ~myQsciScintilla(){};
  virtual QSize sizeHint() const { return QSize(350, 150); };
protected:
  virtual bool event(QEvent *e);
};

bool myQsciScintilla::event(QEvent *e)
{
  if (e->type() == QEvent::ShortcutOverride)
    {
      QKeyEvent *ke = (QKeyEvent*)e;
      if ( ((ke->modifiers() == Qt::NoModifier) || (ke->modifiers() == Qt::ShiftModifier))
           && (ke->key() != Qt::Key_Escape))
        {
          e->accept();
          return true;
        }
    }
  return QsciScintilla::event(e);
}

#endif


EditionScript::EditionScript(Subject* subject,
                         QWidget* parent,
                         const char* name)
  : EditionElementaryNode(subject, parent, name)
{
  _subInlineNode = 0;
  _sci = 0;

  _subInlineNode = dynamic_cast<SubjectInlineNode*>(_subject);
  assert(_subInlineNode);

  createTablePorts();
  setEditablePorts(true);

  _haveScript = true;
#if (defined HAS_QSCI4 == 1)
  _sci = new myQsciScintilla(this);
#else
  _sci = new QTextEdit(this);
#endif
  _wid->gridLayout->removeItem(_wid->spacerItem);
  _wid->gridLayout1->addWidget( _sci );
#if (defined HAS_QSCI4 == 1)
  _sci->setUtf8(1);
  QsciLexerPython *lex = new QsciLexerPython(_sci);
  _sci->setLexer(lex);
  _sci->setBraceMatching(QsciScintilla::SloppyBraceMatch);
  _sci->setAutoIndent(1);
  _sci->setIndentationWidth(4);
  _sci->setIndentationGuides(1);
  _sci->setIndentationsUseTabs(0);
  _sci->setAutoCompletionThreshold(2);
#endif

  if (YACS::ENGINE::InlineNode* pyNode =
      dynamic_cast<YACS::ENGINE::InlineNode*>(_subInlineNode->getNode()))
    {
      _sci->append(pyNode->getScript().c_str());
    }
  connect(_sci, SIGNAL(textChanged()), this, SLOT(onScriptModified()));

}

EditionScript::~EditionScript()
{
}

void EditionScript::onApply()
{
  bool scriptEdited = false;
  if (_haveScript)
    {
#if (defined HAS_QSCI4 == 1)
      if (_sci->isModified())
        {
          scriptEdited = true;
          bool ret = _subInlineNode->setScript(_sci->text().toStdString());
          if (ret) scriptEdited = false;
        }
#else
      if (_sci->document()->isModified())
        {
          scriptEdited = true;
          bool ret = _subInlineNode->setScript(_sci->document()->toPlainText().toStdString());
          if (ret) scriptEdited = false;
        }
#endif
    }
  _isEdited = _isEdited || scriptEdited;

  EditionElementaryNode::onApply();
}

void EditionScript::onCancel()
{
  if (_haveScript)
    _sci->setText(_subInlineNode->getScript().c_str());
  EditionElementaryNode::onCancel();
}

void EditionScript::onScriptModified()
{
  setEdited(true);
}
