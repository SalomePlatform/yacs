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
#include "Resource.hxx"

#include "InlineNode.hxx"

#if HAS_QSCI4>0
#include <qsciscintilla.h>
#include <qscilexerpython.h>
#endif

#include <QSplitter>

#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

#if HAS_QSCI4>0
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
  YASSERT(_subInlineNode);

  QSplitter *splitter = new QSplitter(this);
  splitter->setOrientation(Qt::Vertical);
  _wid->gridLayout1->addWidget(splitter);

  QWidget* widg=new QWidget;
  QVBoxLayout *layout = new QVBoxLayout;
  widg->setLayout(layout);
  layout->setMargin(1);
  splitter->addWidget(widg);

  QWidget* window=new QWidget;
  _glayout=new QVBoxLayout;
  window->setLayout(_glayout);
  _glayout->setMargin(1);
  splitter->addWidget(window);

  createTablePorts(layout);
  setEditablePorts(true);

  _haveScript = true;
#if HAS_QSCI4>0
  _sci = new myQsciScintilla(this);
#else
  _sci = new QTextEdit(this);
#endif
  _wid->gridLayout->removeItem(_wid->spacerItem);
  _glayout->addWidget( _sci );
#if HAS_QSCI4>0
  _sci->setUtf8(1);
  QsciLexerPython *lex = new QsciLexerPython(_sci);
  lex->setFont(Resource::pythonfont);
  _sci->setLexer(lex);
  _sci->setBraceMatching(QsciScintilla::SloppyBraceMatch);
  _sci->setAutoIndent(1);
  _sci->setIndentationWidth(4);
  _sci->setIndentationGuides(1);
  _sci->setIndentationsUseTabs(0);
  _sci->setAutoCompletionThreshold(2);
  //_sci->setMarginLineNumbers(1,true);
  _sci->setMarginWidth(1,0);
#endif

  if (!QtGuiContext::getQtCurrent()->isEdition())
    _sci->setReadOnly(true);

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
#if HAS_QSCI4>0
  _sci->lexer()->setFont(Resource::pythonfont);
#endif
  if (_haveScript)
    {
#if HAS_QSCI4>0
      if (_sci->isModified())
        {
          scriptEdited = true;
	  std::string text=_sci->text().toStdString();
	  if(text[text.length()-1] != '\n')
	    text=text+'\n';
          bool ret = _subInlineNode->setScript(text);
          if (ret) scriptEdited = false;
        }
#else
      if (_sci->document()->isModified())
        {
          scriptEdited = true;
	  std::string text=_sci->document()->toPlainText().toStdString();
	  if(text[text.length()-1] != '\n')
	    text=text+'\n';
          bool ret = _subInlineNode->setScript(text);
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
