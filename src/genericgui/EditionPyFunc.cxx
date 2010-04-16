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
#include "EditionPyFunc.hxx"
#include "InlineNode.hxx"
#include "QtGuiContext.hxx"
#include "Container.hxx"

#include <QToolButton>

#if HAS_QSCI4>0
#include <qsciscintilla.h>
#include <qscilexerpython.h>
#endif

#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;
using namespace YACS::ENGINE;


EditionPyFunc::EditionPyFunc(Subject* subject,
                             QWidget* parent,
                             const char* name)
  : EditionScript(subject, parent, name)
{
  _subFuncNode = 0;
  _funcName ="";
  _liFuncName = 0;

  _subFuncNode = dynamic_cast<SubjectPyFuncNode*>(_subject);
  YASSERT(_subFuncNode);

  YACS::ENGINE::InlineFuncNode *pyFuncNode
    = dynamic_cast<YACS::ENGINE::InlineFuncNode*>(_subFuncNode->getNode());
  YASSERT(pyFuncNode);

  //remove table widget from ports layout
  _portslayout->removeWidget( _twPorts );
  //add an options section in ports layout
  QHBoxLayout* hboxLayout = new QHBoxLayout();
  hboxLayout->setMargin(0);
  QToolButton* tb_options = new QToolButton();
  tb_options->setCheckable(true);
  QIcon icon;
  icon.addFile("icons:icon_down.png");
  icon.addFile("icons:icon_up.png", QSize(), QIcon::Normal, QIcon::On);
  tb_options->setIcon(icon);
  hboxLayout->addWidget(tb_options);
  QLabel* label = new QLabel("Execution Mode");
  hboxLayout->addWidget(label);
  _portslayout->addLayout(hboxLayout);

  fr_options = new QFrame();
  QHBoxLayout* hboxLayout1 = new QHBoxLayout(fr_options);
  hboxLayout1->setMargin(0);
  radiolocal= new QRadioButton("Local");
  radioremote= new QRadioButton("Remote");
  radiolocal->setChecked(true);
  hboxLayout1->addWidget(radiolocal);
  hboxLayout1->addWidget(radioremote);
  hboxLayout->addWidget(fr_options);

  fr_container = new QFrame();
  QHBoxLayout* hboxLayout2 = new QHBoxLayout(fr_container);
  hboxLayout2->setMargin(0);
  QLabel* laContainer = new QLabel("Container:");
  hboxLayout2->addWidget(laContainer);
  cb_container = new ComboBox();
  hboxLayout2->addWidget(cb_container);
  _portslayout->addWidget(fr_container);

  //reinsert table widget into ports layout
  _portslayout->addWidget( _twPorts );

  _glayout->removeWidget( _sci );

  QGridLayout *glt = new QGridLayout();
  _funcName = pyFuncNode->getFname();
  QLabel* laFuncName = new QLabel("laFuncName", this );
  glt->addWidget(laFuncName, 0, 0, 1, 1);
  laFuncName->setText("Function Name:");
  _liFuncName = new QLineEdit( "liFuncName", this );
  glt->addWidget(_liFuncName, 0, 1, 1, 1);
  _liFuncName->setText(_funcName.c_str());
  _glayout->addLayout( glt , 1);

  _glayout->addWidget( _sci );

  connect(_liFuncName, SIGNAL(textChanged(const QString&)),
          this, SLOT(onFuncNameModified(const QString&)));
  connect(tb_options, SIGNAL(toggled(bool)), this, SLOT(on_tb_options_toggled(bool)));
  connect(radioremote, SIGNAL(toggled(bool)), this, SLOT(on_remote_toggled(bool)));
  connect(cb_container, SIGNAL(mousePressed()), this, SLOT(fillContainerPanel()));
  connect(cb_container, SIGNAL(activated(int)), this, SLOT(changeContainer(int)));

  update(UPDATE,0,0);
  on_tb_options_toggled(false);
}

EditionPyFunc::~EditionPyFunc()
{
}

void EditionPyFunc::onApply()
{
  bool funcNameEdited = false;
  string funcName = _liFuncName->text().toStdString();
  if (funcName.empty())
    {
      _liFuncName->setText(_funcName.c_str());
      funcName = _funcName;
    }
  if (funcName != _funcName)
    {
      funcNameEdited = true;
      bool ret = _subFuncNode->setFunctionName(funcName);
      if (ret)
        {
          funcNameEdited = false;
          _funcName = funcName;
        }
    }
  _isEdited = _isEdited || funcNameEdited;
  EditionScript::onApply();
}

void EditionPyFunc::onCancel()
{
  _liFuncName->setText(_funcName.c_str());
  EditionScript::onCancel();
}

void EditionPyFunc::onFuncNameModified(const QString &text)
{
  if (_funcName != text.toStdString()) setEdited(true);
}

void EditionPyFunc::on_tb_options_toggled(bool checked)
{
  DEBTRACE("EditionPyFunc::on_tb_options_toggled " << checked);
  _checked = checked;
  if(_checked)
    {
      fr_options->show();
      if(_remote)fr_container->show();
    }
  else
    {
      fr_options->hide();
      fr_container->hide();
    }
}

void EditionPyFunc::on_remote_toggled(bool checked)
{
  DEBTRACE("EditionPyFunc::on_remote_toggled " << checked);
  _remote=checked;
  YACS::ENGINE::InlineFuncNode *pyFuncNode = dynamic_cast<YACS::ENGINE::InlineFuncNode*>(_subFuncNode->getNode());
  std::string mode = pyFuncNode->getExecutionMode();

  if(checked)
    {
      //remote radio button is checked
      if(mode != "remote")
        _subFuncNode->setExecutionMode("remote");
      fr_container->show();
      fillContainerPanel();
    }
  else
    {
      //remote radio button is unchecked
      if(mode != "local")
        _subFuncNode->setExecutionMode("local");
      fr_container->hide();
    }
}

void EditionPyFunc::fillContainerPanel()
{
  DEBTRACE("EditionPyFunc::fillContainerPanel ");
  Proc* proc = GuiContext::getCurrent()->getProc();

  cb_container->clear();
  std::map<string,Container*>::const_iterator it = proc->containerMap.begin();
  for(; it != proc->containerMap.end(); ++it)
    cb_container->addItem( QString((*it).first.c_str()));

  YACS::ENGINE::InlineFuncNode *pyFuncNode = dynamic_cast<YACS::ENGINE::InlineFuncNode*>(_subFuncNode->getNode());

  Container * cont = pyFuncNode->getContainer();
  if (cont)
    {
      int index = cb_container->findText(cont->getName().c_str());
      cb_container->setCurrentIndex(index);
    }
}

void EditionPyFunc::changeContainer(int index)
{
  DEBTRACE("EditionPyFunc::changeContainer ");
  string contName = cb_container->itemText(index).toStdString();
  DEBTRACE(contName);
  YACS::ENGINE::InlineFuncNode *pyFuncNode = dynamic_cast<YACS::ENGINE::InlineFuncNode*>(_subFuncNode->getNode());
  Container *oldContainer = pyFuncNode->getContainer();

  Container *newContainer = 0;
  Proc* proc = GuiContext::getCurrent()->getProc();
  if (proc->containerMap.count(contName))
    newContainer = proc->containerMap[contName];
  if (!newContainer)
    {
      DEBTRACE("-------------> not found : " << contName);
      return;
    }
  YASSERT(GuiContext::getCurrent()->_mapOfSubjectContainer.count(newContainer));
  SubjectContainer *scnt = GuiContext::getCurrent()->_mapOfSubjectContainer[newContainer];

  _subFuncNode->setContainer(scnt);
}

void EditionPyFunc::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("EditionPyFunc::update " << eventName(event) <<" "<<type<<" "<<son);
  EditionElementaryNode::update(event, type, son);
  if(event == ASSOCIATE)
    {
      fillContainerPanel();
    }
  else if(event == UPDATE)
    {
      YACS::ENGINE::InlineFuncNode *pyFuncNode = dynamic_cast<YACS::ENGINE::InlineFuncNode*>(_subFuncNode->getNode());
      std::string mode = pyFuncNode->getExecutionMode();
      if(mode == "remote")
        {
          _remote=true;
          radioremote->setChecked(true);
        }
      else if(mode == "local")
        {
          _remote=false;
          radiolocal->setChecked(true);
        }

      fillContainerPanel();
    }
}

