// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
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

#include "FormHPContainer.hxx"
#include "FormAdvParamContainer.hxx"
#include "HomogeneousPoolContainer.hxx"
#include "QtGuiContext.hxx"
#include "guiObservers.hxx"
//#define _DEVDEBUG_
#include "Resource.hxx"
#include "YacsTrace.hxx"

#include <QIntValidator>
#include <QLineEdit>

#ifdef HAS_PYEDITOR
#include <PyEditor_Editor.h>
#endif

#include <sstream>
#include <limits>

using namespace std;

FormHPContainer::FormHPContainer(QWidget *parent):FormContainerBase(parent),_poolSz(new QLineEdit(this)),_initScriptModified(false)
{
  QIntValidator *iv(new QIntValidator(_poolSz)); iv->setRange(1,INT_MAX);
  _poolSz->setValidator(iv);
  label_15->setText("Size of pool :");
  gridLayout_2_2->addWidget(_poolSz);
  FormHPContainer::FillPanel(0); // --- set widgets before signal connexion to avoid false modif detection
  connect(_poolSz, SIGNAL(textChanged(const QString&)),this, SLOT(onModifySzOfPool(const QString&)));
  ch_aoc->setEnabled(false);
  ch_aoc->setCheckState(Qt::Checked);
  //
#ifdef HAS_PYEDITOR
  _initScript=new PyEditor_Editor(_advancedParams->tw_advance);
#else
  _initScript=new QTextEdit(this);
#endif
  connect(_initScript,SIGNAL(textChanged()),this,SLOT(initSciptChanged()));
  QGridLayout *gridLayout(new QGridLayout(_initScript));
  _advancedParams->tw_advance->addTab(_initScript,"Init Script");
}

FormHPContainer::~FormHPContainer()
{
}

void FormHPContainer::FillPanel(YACS::ENGINE::Container *container)
{
  DEBTRACE("FormHPContainer::FillPanel");
  FormContainerBase::FillPanel(container);
  if(!container)
    return ;
  YACS::ENGINE::HomogeneousPoolContainer *hpc(dynamic_cast<YACS::ENGINE::HomogeneousPoolContainer *>(container));
  if(!hpc)
    throw YACS::Exception("FormHPContainer::FillPanel : not a HP Container !");
  _poolSz->setText(QString("%1").arg(hpc->getSizeOfPool()));
  std::string initScript;
  if(_properties.count(YACS::ENGINE::HomogeneousPoolContainer::INITIALIZE_SCRIPT_KEY))
    initScript=_properties[YACS::ENGINE::HomogeneousPoolContainer::INITIALIZE_SCRIPT_KEY];
  std::string initScript2(BuildWithFinalEndLine(initScript));
  _initScript->blockSignals(true);
  _initScript->setText(initScript2.c_str());
  _initScript->blockSignals(false);
  if (!YACS::HMI::QtGuiContext::getQtCurrent()->isEdition())
    {
      //if the schema is in execution do not allow editing
      _poolSz->setEnabled(false);
      _initScript->setEnabled(false);
    }
}

QString FormHPContainer::getTypeStr() const
{
  return QString("Container (HP)");
}

void FormHPContainer::onModifySzOfPool(const QString& newSz)
{
  if (!_container)
    return;
  map<string,string> properties(_container->getProperties());
  uint sz;
  bool isOK;
  sz=newSz.toUInt(&isOK);
  if(!isOK)
    return ;
  _properties[YACS::ENGINE::HomogeneousPoolContainer::SIZE_OF_POOL_KEY] = newSz.toStdString();
  if(properties[YACS::ENGINE::HomogeneousPoolContainer::SIZE_OF_POOL_KEY] != newSz.toStdString())
    onModified();
}

bool FormHPContainer::onApply()
{
  YACS::HMI::SubjectContainerBase *scont(YACS::HMI::QtGuiContext::getQtCurrent()->_mapOfSubjectContainer[_container]);
  YASSERT(scont);
  bool ret(scont->setName(le_name->text().toStdString()));
  std::map<std::string,std::string> properties(_properties);
  if(_initScriptModified)
    {
      std::string text(_initScript->toPlainText().toStdString());
      std::string text2(BuildWithFinalEndLine(text));
      properties[YACS::ENGINE::HomogeneousPoolContainer::INITIALIZE_SCRIPT_KEY]=text2;
    }
  _initScriptModified=false;
  DEBTRACE(ret);
  if(ret)
    ret = scont->setProperties(properties);
  return ret;
}

void FormHPContainer::initSciptChanged()
{
  _initScriptModified=true;
  onModified();
}

std::string FormHPContainer::BuildWithFinalEndLine(const std::string& script)
{
  if(script.empty())
    return std::string("\n");
  std::size_t sz(script.length());
  if(script[sz-1]!='\n')
    {
      std::string ret(script);
      ret+="\n";
      return ret;
    }
  else
    return script;
}
