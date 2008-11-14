
#include "nodeEdition.h"
#include "guiContext.hxx"
#include "Port.hxx"
#include "InlineNode.hxx"
#include "TypeCode.hxx"

#include <qlineedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>

#ifdef HAVE_QEXTSCINTILLA_H
#include <qextscintilla.h>
#include <qextscintillalexerpython.h>
#endif

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <string>
#include <cassert>

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

#ifdef HAVE_QEXTSCINTILLA_H
class myQextScintilla: public QextScintilla
{
public:
  myQextScintilla(QWidget *parent=0, const char *name=0, WFlags f=0)
    : QextScintilla(parent, name, f) {};
  ~myQextScintilla(){};
  virtual QSize sizeHint() const { return QSize(350, 150); };
};
#endif

itemEdition::itemEdition(Subject* subject)
  : GuiObserver(), _subject(subject)
{
  _subject->attach(this);
  _stackId = -1;
  _name = _subject->getName();
  _type = "Unknown";
  _category = "Unknown";
  ProcInvoc* invoc = GuiContext::getCurrent()->getInvoc();
  if (SubjectNode * sub = dynamic_cast<SubjectNode*>(_subject))
    {
      _category = "Node";
      _type = invoc->getTypeName(invoc->getTypeOfNode(sub->getNode()));
    }
  else if (SubjectDataPort * sub = dynamic_cast<SubjectDataPort*>(_subject))
    {
      _category = "Port";
      _type = invoc->getTypeName(invoc->getTypeOfPort(sub->getPort()));
    }
  else if (SubjectLink * sub = dynamic_cast<SubjectLink*>(_subject))
    {
      _category = "Link";
      _type = "Link";
    }
  else if (SubjectControlLink * sub = dynamic_cast<SubjectControlLink*>(_subject))
    {
      _category = "ControlLink";
      _type = "Control Link";
    }
  else if (SubjectDataType * sub = dynamic_cast<SubjectDataType*>(_subject))
    {
      _category = "Data";
      switch (sub->getTypeCode()->kind())
        {
        case YACS::ENGINE::Double:   _type = "double"; break;
        case YACS::ENGINE::Int:      _type = "int"; break;
        case YACS::ENGINE::String:   _type = "string"; break;
        case YACS::ENGINE::Bool:     _type = "bool"; break;
        case YACS::ENGINE::Objref:   _type = "Objref"; break;
        case YACS::ENGINE::Sequence: _type = "Sequence"; break;
        case YACS::ENGINE::Array:    _type = "Array"; break;
        case YACS::ENGINE::Struct:   _type = "Struct"; break;
        default: _type = "None or Unknown";
        }
    }
  else if (SubjectComponent * sub = dynamic_cast<SubjectComponent*>(_subject))
    {
      _category = "Component";
      _type = "Salome Component";
    }
  else if (SubjectContainer * sub = dynamic_cast<SubjectContainer*>(_subject))
    {
      _category = "Container";
      _type = "Salome Container";
    }
}

itemEdition::~itemEdition()
{
}

void itemEdition::select(bool isSelected)
{
  DEBTRACE("NodeEdition::select " << isSelected);
  if (isSelected)
    GuiContext::getCurrent()->getWidgetStack()->raiseWidget(_stackId);
}


void itemEdition::update(GuiEvent event, int type, Subject* son)
{
}

Subject* itemEdition::getSubject()
{
  return _subject;
}


NodeEdition::NodeEdition(Subject* subject,
                         QWidget* parent,
                         const char* name,
                         WFlags fl)
  : wiNodeEdition(parent, name, fl), itemEdition(subject)
{
  DEBTRACE("NodeEdition::NodeEdition " << name);
  string category = _category + " type:";
  laNodeType->setText(category.c_str());
  category = _category + " name:";
  laNodeName->setText(category.c_str());
  liNodeType->setText(_type);
  liNodeName->setText(_name.c_str());
  liNodeType->setText(_type);
  setEdited(false);
  _haveScript = false;
  _sci = 0;
  _subInlineNode = 0;
  _subFuncNode = 0;
  _funcName ="";
  _liFuncName = 0;
  if (_subFuncNode = dynamic_cast<SubjectPyFuncNode*>(_subject))
    if (YACS::ENGINE::InlineFuncNode *pyFuncNode
        = dynamic_cast<YACS::ENGINE::InlineFuncNode*>(_subFuncNode->getNode()))
      {
        _funcName = pyFuncNode->getFname();
        QLabel* laFuncName = new QLabel( this, "laFuncName" );
        layout5->addWidget( laFuncName );
        laFuncName->setText("Function Name:");
        _liFuncName = new QLineEdit( this, "liFuncName" );
        layout5->addWidget( _liFuncName );
        _liFuncName->setText(_funcName);
        connect(_liFuncName, SIGNAL(textChanged(const QString&)), this, SLOT(onFuncNameModified(const QString&)));
      }
    else assert(0);
  if (_subInlineNode = dynamic_cast<SubjectInlineNode*>(_subject))
    {
      _haveScript = true;
#ifdef HAVE_QEXTSCINTILLA_H
      _sci = new myQextScintilla( this, "Python Editor" );
#else
      _sci = new QTextEdit( this, "Python Editor" );
#endif
      wiNodeEditionLayout->removeItem(spacer2);
      layout5->addWidget( _sci );
#ifdef HAVE_QEXTSCINTILLA_H
      _sci->setUtf8(1);
      QextScintillaLexerPython *lex = new QextScintillaLexerPython(_sci);
      _sci->setLexer(lex);
      _sci->setBraceMatching(QextScintilla::SloppyBraceMatch);
      _sci->setAutoIndent(1);
      _sci->setIndentationWidth(4);
      _sci->setIndentationGuides(1);
      _sci->setIndentationsUseTabs(0);
      _sci->setAutoCompletionThreshold(2);
#endif
      if (YACS::ENGINE::InlineNode* pyNode = dynamic_cast<YACS::ENGINE::InlineNode*>(_subInlineNode->getNode()))
        {
          _sci->append(pyNode->getScript());
        }
      connect(_sci, SIGNAL(textChanged()), this, SLOT(onScriptModified()));
    }
  _stackId = GuiContext::getCurrent()->getWidgetStack()->addWidget(this);
  DEBTRACE("_stackId " << _stackId);
  //GuiContext::getCurrent()->getWidgetStack()->raiseWidget(_stackId);
}

NodeEdition::~NodeEdition()
{
}

void NodeEdition::setName(std::string name)
{
  _name = name;
  setEdited(false);
}

void NodeEdition::onApply()
{
  string name = liNodeName->text().latin1();
  bool nameEdited = false;
  if (name != _name)
    {
      nameEdited = true;
      bool ret = _subject->setName(name);
      if (ret)
        {
          nameEdited = false;
          _name = name;
        }
    }
  bool funcNameEdited = false;
  if (_subFuncNode)
    {
      funcNameEdited = true;
      string funcName = _liFuncName->text().latin1();
      if (funcName != _funcName)
        {
          bool ret = _subFuncNode->setFunctionName(funcName);
          if (ret)
            {
              funcNameEdited = false;
              _funcName = funcName;
            }
        }
    }
  bool scriptEdited = false;
  if (_haveScript)
    {
      if (_sci->isModified())
        {
          scriptEdited = true;
          bool ret = _subInlineNode->setScript(_sci->text().latin1());
          if (ret)
            {
              scriptEdited = false;
            }
        }
    }
  bool edited = nameEdited || funcNameEdited || scriptEdited;
  setEdited(edited);
}

void NodeEdition::onCancel()
{
  liNodeName->setText(_name.c_str());
  if (_haveScript)
    _sci->setText(_subInlineNode->getScript());
  if (_subFuncNode)
    _liFuncName->setText(_funcName);
  setEdited(false);
}

void NodeEdition::onModify(const QString &text)
{
  if (_name != text.latin1()) setEdited(true);
}

void NodeEdition::onFuncNameModified(const QString &text)
{
  if (_funcName != text.latin1()) setEdited(true);
}

void NodeEdition::onScriptModified()
{
  setEdited(true);
}

void NodeEdition::setEdited(bool isEdited)
{
  btApply->setEnabled(isEdited);
  btCancel->setEnabled(isEdited);
}

void NodeEdition::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("NodeEdition::update ");
  NodeEdition *item = 0;
  switch (event)
    {
    case NEWROOT:
      item =  new NodeEdition(son,
                              GuiContext::getCurrent()->getWidgetStack(),
                              son->getName().c_str());
      break;
    case ADD:
      switch (type)
        {
        case YACS::HMI::SALOMEPROC:
        case YACS::HMI::BLOC:
        case YACS::HMI::FOREACHLOOP:
        case YACS::HMI::OPTIMIZERLOOP:
        case YACS::HMI::FORLOOP:
        case YACS::HMI::WHILELOOP:
        case YACS::HMI::SWITCH:
        case YACS::HMI::PYTHONNODE:
        case YACS::HMI::PYFUNCNODE:
        case YACS::HMI::CORBANODE:
        case YACS::HMI::SALOMENODE:
        case YACS::HMI::CPPNODE:
        case YACS::HMI::SALOMEPYTHONNODE:
        case YACS::HMI::XMLNODE:
        case YACS::HMI::PRESETNODE:
        case YACS::HMI::OUTNODE:
        case YACS::HMI::STUDYINNODE:
        case YACS::HMI::STUDYOUTNODE:
        case YACS::HMI::SPLITTERNODE:
        case YACS::HMI::DFTODSFORLOOPNODE:
        case YACS::HMI::DSTODFFORLOOPNODE:
        case YACS::HMI::INPUTPORT:
        case YACS::HMI::OUTPUTPORT:
        case YACS::HMI::INPUTDATASTREAMPORT:
        case YACS::HMI::OUTPUTDATASTREAMPORT:
        case YACS::HMI::CONTAINER:
        case YACS::HMI::COMPONENT:
        case YACS::HMI::REFERENCE:
        case YACS::HMI::DATATYPE:
          item =  new NodeEdition(son,
                                  GuiContext::getCurrent()->getWidgetStack(),
                                  son->getName().c_str());
          break;
        default:
          DEBTRACE("NodeEdition::update() ADD, type not handled:" << type);
        }
      break;
    case ADDLINK:
      switch (type)
        {
        case YACS::HMI::DATALINK:
          item =  new NodeEdition(son,
                                  GuiContext::getCurrent()->getWidgetStack(),
                                  son->getName().c_str());
          break;
        default:
          DEBTRACE("NodeEdition::update() ADDLINK, type not handled:" << type);
        }
      break;
    case ADDCONTROLLINK:
      switch (type)
        {
        case YACS::HMI::CONTROLLINK:
          item =  new NodeEdition(son,
                                  GuiContext::getCurrent()->getWidgetStack(),
                                  son->getName().c_str());
          break;
        default:
          DEBTRACE("NodeEdition::update() ADDCONTROLLINK, type not handled:" << type);
        }
      break;
    default:
      DEBTRACE("NodeEdition::update(), event not handled: " << event);
    }
}
