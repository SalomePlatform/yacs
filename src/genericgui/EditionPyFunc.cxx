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

  _wid->gridLayout1->removeWidget( _sci );

  QGridLayout *glt = new QGridLayout();;
  _funcName = pyFuncNode->getFname();
  QLabel* laFuncName = new QLabel("laFuncName", this );
  glt->addWidget(laFuncName, 0, 0, 1, 1);
  laFuncName->setText("Function Name:");
  _liFuncName = new QLineEdit( "liFuncName", this );
  glt->addWidget(_liFuncName, 0, 1, 1, 1);
  _liFuncName->setText(_funcName.c_str());
  int rows = _wid->gridLayout1->rowCount();
  _wid->gridLayout1->addLayout( glt , rows, 0, 1, 1);

  _wid->gridLayout1->addWidget( _sci );

  connect(_liFuncName, SIGNAL(textChanged(const QString&)),
          this, SLOT(onFuncNameModified(const QString&)));
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
