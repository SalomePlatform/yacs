// Copyright (C) 2006-2014  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "EditionScript.hxx"
#include "QtGuiContext.hxx"
#include "Resource.hxx"
#include "Container.hxx"
#include "InlineNode.hxx"
#include "FormContainer.hxx"
#include "Message.hxx"

#if HAS_QSCI4>0
#include <qsciscintilla.h>
#include <qscilexerpython.h>
#endif

#include <QSplitter>
#include <QTemporaryFile>
#include <QTextStream>
#include <QProcess>

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
      e->accept();
      return true;
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
  _portslayout = new QVBoxLayout;
  widg->setLayout(_portslayout);
  _portslayout->setMargin(1);
  splitter->addWidget(widg);

  QWidget* window=new QWidget;
  _glayout=new QVBoxLayout;
  window->setLayout(_glayout);
  _glayout->setMargin(1);
  splitter->addWidget(window);

  fr_container = new QFrame();
  QHBoxLayout* hboxLayout2 = new QHBoxLayout(fr_container);
  hboxLayout2->setMargin(0);
  QLabel* laContainer = new QLabel("Container:");
  hboxLayout2->addWidget(laContainer);
  cb_container = new ComboBox();
  hboxLayout2->addWidget(cb_container);
  _portslayout->addWidget(fr_container);

  formcontainer = new FormContainer(this);
  formcontainer->on_tb_container_toggled(false);
  _portslayout->addWidget(formcontainer);
  //end of insertion of execution mode

  createTablePorts(_portslayout);
  setEditablePorts(true);

  _haveScript = true;
#if HAS_QSCI4>0
  _sci = new myQsciScintilla(this);
#else
  _sci = new QTextEdit(this);
#endif
  _wid->gridLayout->removeItem(_wid->spacerItem);

  _editor = new QPushButton("External Editor", this);
  connect(_editor, SIGNAL(clicked()), this, SLOT(onEdit()));
  if(!Resource::pythonExternalEditor.isEmpty())
    {
      _glayout->addWidget( _editor );
    }
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
  _sci->setFolding(QsciScintilla::PlainFoldStyle);
#endif

  if (!QtGuiContext::getQtCurrent()->isEdition())
    _sci->setReadOnly(true);

  if (YACS::ENGINE::InlineNode* pyNode =
      dynamic_cast<YACS::ENGINE::InlineNode*>(_subInlineNode->getNode()))
    {
      _sci->append(pyNode->getScript().c_str());
    }
  connect(_sci, SIGNAL(textChanged()), this, SLOT(onScriptModified()));

  connect(cb_container, SIGNAL(mousePressed()), this, SLOT(fillContainerPanel()));
  connect(cb_container, SIGNAL(activated(int)), this, SLOT(changeContainer(int)));

  update(UPDATE,0,0);
  changeContainer(0);
}

EditionScript::~EditionScript()
{
}

void EditionScript::synchronize()
{
  DEBTRACE("EditionScript::synchronize " << this->_isEdited);
  EditionElementaryNode::synchronize();
  YACS::ENGINE::InlineNode* pyNode = dynamic_cast<YACS::ENGINE::InlineNode*>(_subInlineNode->getNode());
  YASSERT(pyNode);
  disconnect(_sci, SIGNAL(textChanged()), this, SLOT(onScriptModified()));
  if (!_isEdited)
  {
    _sci->clear();
    _sci->append(pyNode->getScript().c_str());
  }
  connect(_sci, SIGNAL(textChanged()), this, SLOT(onScriptModified()));
}

void EditionScript::onApply()
{
  DEBTRACE("EditionScript::onApply");
  bool scriptEdited = false;
#if HAS_QSCI4>0
  _sci->lexer()->setFont(Resource::pythonfont);
#endif

  if(Resource::pythonExternalEditor.isEmpty())
    {
      _editor->hide();
      _glayout->removeWidget(_editor);
    }
  else
    {
      _editor->show();
      if(_glayout->itemAt(0)->widget() != _editor)
        _glayout->insertWidget ( 0, _editor );
    }

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

  bool containerEdited = true;
  if (formcontainer->onApply()) {
    fillContainerPanel();
    containerEdited = false;
  } else {
    Message mess(GuiContext::getCurrent()->_lastErrorMessage);
    return;
  }

  _isEdited = _isEdited || scriptEdited || containerEdited;

  EditionElementaryNode::onApply();
}

void EditionScript::onCancel()
{
  if (_haveScript)
    _sci->setText(_subInlineNode->getScript().c_str());
  formcontainer->onCancel();
  EditionElementaryNode::onCancel();
}

void EditionScript::onScriptModified()
{
  DEBTRACE("EditionScript::onScriptModified");
  _isEdited = true;
  setEdited(true);
}

void EditionScript::onEdit()
{
  DEBTRACE("EditionScript::onEdit");
  QTemporaryFile outFile;
  if (!outFile.open())
    return;
  QString fileName = outFile.fileName();
  QTextStream out(&outFile);
#if HAS_QSCI4>0
  out << _sci->text();
#else
  out << _sci->toPlainText();
#endif
  outFile.close();

  QStringList options=Resource::pythonExternalEditor.split(" ");
  QString prog=options.takeAt(0);
  QProcess::execute(prog, QStringList() << options << fileName);

  QFile inFile(fileName);
  if (!inFile.open(QIODevice::ReadOnly))
    return;
  QTextStream in(&inFile);
#if HAS_QSCI4>0
  _sci->setText(in.readAll());
#else
  _sci->setPlainText(in.readAll());
#endif
  onApply();
}

void EditionScript::fillContainerPanel()
{
  DEBTRACE("EditionScript::fillContainerPanel ");
  YACS::ENGINE::Proc* proc = GuiContext::getCurrent()->getProc();

  cb_container->clear();
  std::map<string,YACS::ENGINE::Container*>::const_iterator it = proc->containerMap.begin();
  for(; it != proc->containerMap.end(); ++it)
    cb_container->addItem( QString((*it).first.c_str()));

  YACS::ENGINE::InlineNode *pyNode = dynamic_cast<YACS::ENGINE::InlineNode*>(_subInlineNode->getNode());

  YACS::ENGINE::Container * cont = pyNode->getContainer();
  if (cont)
    {
      int index = cb_container->findText(cont->getName().c_str());
      cb_container->setCurrentIndex(index);
      formcontainer->FillPanel(cont);
    }
}

void EditionScript::changeContainer(int index)
{
  DEBTRACE("EditionScript::changeContainer ");
  string contName = cb_container->itemText(index).toStdString();
  DEBTRACE(contName);
  YACS::ENGINE::InlineNode *pyNode = dynamic_cast<YACS::ENGINE::InlineNode*>(_subInlineNode->getNode());
  YACS::ENGINE::Container *oldContainer = pyNode->getContainer();

  YACS::ENGINE::Container *newContainer = 0;
  YACS::ENGINE::Proc* proc = GuiContext::getCurrent()->getProc();
  if (proc->containerMap.count(contName))
    newContainer = proc->containerMap[contName];
  if (!newContainer)
    {
      DEBTRACE("-------------> not found : " << contName);
      return;
    }
  YASSERT(GuiContext::getCurrent()->_mapOfSubjectContainer.count(newContainer));
  SubjectContainer *scnt = GuiContext::getCurrent()->_mapOfSubjectContainer[newContainer];

  _subInlineNode->setContainer(scnt);
  
  // show the selected container parameters
  formcontainer->FillPanel(newContainer);
}

void EditionScript::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("EditionScript::update " << eventName(event) <<" "<<type<<" "<<son);
  EditionElementaryNode::update(event, type, son);
  if(event == ASSOCIATE)
    {
      fillContainerPanel();
      SubjectContainer *scont = dynamic_cast<SubjectContainer*>(son);
      YASSERT(scont);
      formcontainer->FillPanel(scont->getContainer());
    }
  else if(event == UPDATE)
    {
      fillContainerPanel();
    }
}

