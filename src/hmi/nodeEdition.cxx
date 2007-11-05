
#include "nodeEdition.h"
#include "guiContext.hxx"

#include <qlineedit.h>
#include <qpushbutton.h>

#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <string>

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

NodeEdition::NodeEdition(Subject* subject,
                         QWidget* parent,
                         const char* name,
                         WFlags fl)
  : wiNodeEdition(parent, name, fl)
{
  DEBTRACE("NodeEdition::NodeEdition " << name);
  _subject = subject;
  _name = _subject->getName();
  _type = "unknown";
  if (dynamic_cast<SubjectBloc*>(_subject)) _type = "BLOC";
  else if (dynamic_cast<SubjectPythonNode*>(_subject)) _type = "PYTHON NODE";
  _subject->attach(this);
  liNodeName->setText(_name.c_str());
  liNodeType->setText(_type);
  liNodeType->setEnabled(false);
  setEdited(false);
  _stackId = GuiContext::getCurrent()->getWidgetStack()->addWidget(this);
  DEBTRACE("_stackId " << _stackId);
  //GuiContext::getCurrent()->getWidgetStack()->raiseWidget(_stackId);
}

NodeEdition::~NodeEdition()
{
  _subject->detach(this);
}

void NodeEdition::setName(std::string name)
{
  _name = name;
  setEdited(false);
}

void NodeEdition::onApply()
{
  string name = liNodeName->text().latin1();
  bool ret = _subject->setName(name);
  if (ret)
    {
      setEdited(false);
      _name = name;
    }
}

void NodeEdition::onCancel()
{
  liNodeName->setText(_name.c_str());
  setEdited(false);
}

void NodeEdition::onModify(const QString &text)
{
  if (_name != text.latin1()) setEdited(true);
}

void NodeEdition::setEdited(bool isEdited)
{
  btApply->setEnabled(isEdited);
  btCancel->setEnabled(isEdited);
}

void NodeEdition::select(bool isSelected)
{
//   DEBTRACE("NodeEdition::select " << isSelected);
  if (isSelected)
    GuiContext::getCurrent()->getWidgetStack()->raiseWidget(_stackId);
}

Subject* NodeEdition::getSubject()
{
  return _subject;
}

void NodeEdition::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("NodeEdition::update ");
  NodeEdition *item = 0;
  switch (event)
    {
    case ADD:
      switch (type)
        {
        case YACS::HMI::BLOC:
        case YACS::HMI::FORLOOP:
        case YACS::HMI::WHILELOOP:
        case YACS::HMI::SWITCH:
        case YACS::HMI::FOREACHLOOP:
        case YACS::HMI::OPTIMIZERLOOP:
          item =  new NodeEdition(son,
                                  GuiContext::getCurrent()->getWidgetStack(),
                                  son->getName().c_str());
          break;
        case YACS::HMI::PYTHONNODE:
        case YACS::HMI::PYFUNCNODE:
        case YACS::HMI::CORBANODE:
        case YACS::HMI::CPPNODE:
        case YACS::HMI::SALOMENODE:
        case YACS::HMI::SALOMEPYTHONNODE:
        case YACS::HMI::XMLNODE:
          item =  new NodeEdition(son,
                                  GuiContext::getCurrent()->getWidgetStack(),
                                  son->getName().c_str());
          break;
        default:
          DEBTRACE("NodeEdition::update() ADD, type not handled:" << type);
        }
      break;
    default:
      DEBTRACE("NodeEdition::update(), event not handled: " << event);
    }
}
