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

#include "ValueDelegate.hxx"
#include "guiObservers.hxx"
#include "SchemaItem.hxx"
#include "DataPort.hxx"
#include "StudyPorts.hxx"
#include "TypeCode.hxx"
#include "Switch.hxx"
#include "ItemEdition.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <QSize>
#include <QSpinBox>
#include <cassert>
#include <climits>

using namespace std;
using namespace YACS::HMI;


// -----------------------------------------------------------------------------

GenericEditor::GenericEditor()
{
  _subject = 0;
  _column = 0;
  _delegate = 0;
  _first = true;
}

GenericEditor::~GenericEditor()
{
  DEBTRACE("GenericEditor::~GenericEditor " << this);
  
}

void GenericEditor::setSubject(Subject* subject)
{
  _subject = subject;
}

void GenericEditor::setColumn(int column)
{
  _column= column;
}

void GenericEditor::setDelegate(const ValueDelegate* delegate)
{
  _delegate = delegate;
}

QString GenericEditor::GetStrValue()
{
  return "";
}

Subject* GenericEditor::getSubject()
{
  return _subject;
}

int GenericEditor::getColumnInSubject()
{
  return _column;
}

void GenericEditor::setData(QVariant val)
{
}

bool GenericEditor::firstSetData()
{
  bool ret = _first;
  _first = false;
  return ret; 
}

// -----------------------------------------------------------------------------

GeneralEditor::GeneralEditor(Subject* subject,
                             const ValueDelegate* delegate,
                             int column,
                             QWidget * parent)
  : QLineEdit(parent), GenericEditor()
{
  DEBTRACE("GeneralEditor::GeneralEditor");
  setDelegate(delegate);
  setSubject(subject);
  setColumn(column);
}

GeneralEditor::~GeneralEditor()
{
}

QString GeneralEditor::GetStrValue()
{
  DEBTRACE("GeneralEditor::GetStrValue " << text().toStdString());
  return text();
}

void GeneralEditor::setData(QVariant val)
{
  DEBTRACE("GeneralEditor::setData " << this);
  DEBTRACE(val.canConvert<QString>());
  DEBTRACE(val.toString().toStdString());
  setText(val.toString());
}

// -----------------------------------------------------------------------------

NameEditor::NameEditor(Subject* subject,
                       const ValueDelegate* delegate,
                       int column,
                       QWidget * parent)
  : QLineEdit(parent), GenericEditor()
{
  DEBTRACE("NameEditor::NameEditor");
  setDelegate(delegate);
  setSubject(subject);
  setColumn(column);
}

NameEditor::~NameEditor()
{
}

QString NameEditor::GetStrValue()
{
  DEBTRACE("Name::GetStrValue " << text().toStdString());
  string filtered = ItemEdition::filterName(text().toStdString());
  return filtered.c_str();
}

void NameEditor::setData(QVariant val)
{
  DEBTRACE("NameEditor::setData " << this);
  DEBTRACE(val.canConvert<QString>());
  DEBTRACE(val.toString().toStdString());
  string filtered = ItemEdition::filterName(val.toString().toStdString());
  setText(filtered.c_str());
}

// -----------------------------------------------------------------------------

IntEditor::IntEditor(Subject* subject,
                     const ValueDelegate* delegate,
                     int column,
                     QWidget * parent)
  : QSpinBox(parent), GenericEditor()
{
  setMinimum(INT_MIN);
  setMaximum(INT_MAX);
  setSubject(subject);
  setDelegate(delegate);
  setColumn(column);
}

IntEditor::~IntEditor()
{
}

QString IntEditor::GetStrValue()
{
  QString str;
  str.setNum(value());
  return str;
}

void IntEditor::setData(QVariant val)
{
  DEBTRACE("IntEditor::setData");
  DEBTRACE(val.canConvert<int>());
  setValue(val.toInt());
}

// -----------------------------------------------------------------------------

CaseSwitchEditor::CaseSwitchEditor(Subject* subject,
                       const ValueDelegate* delegate,
                       int column,
                       QWidget* parent)
  : CaseSwitch(parent), GenericEditor()
{
  sb_case->setMinimum(INT_MIN);
  sb_case->setMaximum(INT_MAX);
  setSubject(subject);
  setDelegate(delegate);
  setColumn(column);
}

CaseSwitchEditor::~CaseSwitchEditor()
{
}

QString CaseSwitchEditor::GetStrValue()
{
  QString str;
  int val = sb_case->value();
  if (_isDefault)
    val = YACS::ENGINE::Switch::ID_FOR_DEFAULT_NODE;
  str.setNum(val);
  DEBTRACE(val);
  return str;
}

void CaseSwitchEditor::setData(QVariant val)
{
  DEBTRACE("CaseSwitchEditor::setData");
  if (val == "default")
    {
      setDefaultChecked(true);
    }
  else
    {
      setDefaultChecked(false);
      DEBTRACE(val.canConvert<int>() << " " << val.toInt());
      sb_case->setValue(val.toInt());
    }
}


// -----------------------------------------------------------------------------

ValueDelegate::ValueDelegate(QObject *parent)
  : QItemDelegate(parent)
{
  DEBTRACE("ValueDelegate::ValueDelegate");
  _errorMap.clear();
}

ValueDelegate::~ValueDelegate()
{
  DEBTRACE("ValueDelegate::~ValueDelegate");
}
      
QWidget *ValueDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{
  DEBTRACE("ValueDelegate::createEditor");

  SchemaItem *item = static_cast<SchemaItem*>(index.internalPointer());
  Subject *subject = item->getSubject();
  int column = index.column();
  DEBTRACE(subject->getName() << " " << column);
  
  QWidget *editor = 0;
  SubjectDataPort *sport = 0;
  SubjectNode *snode = 0;

  if (column == YValue)
    {
      sport = dynamic_cast<SubjectDataPort*>(subject);
      if (!sport)
        snode= dynamic_cast<SubjectNode*>(subject);
      
      if (sport)
        {
          YACS::ENGINE::DataPort *port = sport->getPort();
          YACS::ENGINE::InputStudyPort* istport=dynamic_cast<YACS::ENGINE::InputStudyPort*>(port);
          YACS::ENGINE::OutputStudyPort* ostport=dynamic_cast<YACS::ENGINE::OutputStudyPort*>(port);
          YACS::ENGINE::TypeCode *tc = port->edGetType();
          YACS::ENGINE::DynType dt = tc->kind();
          if (!istport && !ostport && dt == YACS::ENGINE::Int)
            editor = new IntEditor(subject, this, column, parent);
        }
      else if (snode)
        {
          SubjectSwitch *sSwitch = dynamic_cast<SubjectSwitch*>(snode->getParent());
          if (sSwitch)
            editor = new CaseSwitchEditor(subject, this, column, parent);
        }
    }

  if (column == YLabel)
    {
      sport = dynamic_cast<SubjectDataPort*>(subject);      
      if (sport)
        editor = new NameEditor(subject, this, column, parent);
    }

  if (!editor) editor = new GeneralEditor(subject, this, column, parent);
  return editor;
}

/*!
 * Initialise the editor with data from model, unless there is a
 * previous error in edition of this item, and it is the first
 * initialisation of the editor: in case of error, a new editor
 * must be initialised with the wrong edited string, for correction.
 */     
void ValueDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
  DEBTRACE("ValueDelegate::setEditorData");
  GenericEditor* gedit = dynamic_cast<GenericEditor*>(editor);
  YASSERT(gedit);
  QString edited = gedit->GetStrValue();
  DEBTRACE(edited.toStdString());
  Subject *sub = gedit->getSubject();
  if (! gedit->firstSetData())    // --- editor already initialized
    return;
  else if (_errorMap.count(sub))  // --- initialize with previous error
    {
      string error = _errorMap[sub];
      gedit->setData(QString(error.c_str()));
      _errorMap.erase(gedit->getSubject());
      DEBTRACE("--------erase-----------");
    }
  else                            // --- initialize with model
    gedit->setData(index.model()->data(index, Qt::DisplayRole));
}

/*!
 * Nothing done here: model->setData is done by UPDATE.
 */
void ValueDelegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
  DEBTRACE("ValueDelegate::setModelData");
  GenericEditor* gedit = dynamic_cast<GenericEditor*>(editor);
  YASSERT(gedit);
  QString value = gedit->GetStrValue();
  DEBTRACE(value.toStdString());
  //model->setData(index, value, Qt::EditRole); // real set done by update
}
      
void ValueDelegate::updateEditorGeometry(QWidget *editor,
                                         const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
  DEBTRACE("ValueDelegate::updateEditorGeometry");
  editor->setGeometry(option.rect);
}

/*!
 *  If edition is in error, keep the wrong string value in a map,
 *  to initialise a next instance of editor with the wrong string,
 *  in order to help correction of the edition.
 */
void ValueDelegate::setResultEditing(QWidget *editor, bool isOk)
{
  DEBTRACE("ValueDelegate::setResultEditing " << isOk);
  GenericEditor* gedit = dynamic_cast<GenericEditor*>(editor);
  YASSERT(gedit);
  Subject *sub = gedit->getSubject();
  string val = gedit->GetStrValue().toStdString();
  DEBTRACE(sub->getName() << " " << val);
  if (!isOk)
    {
      sub->update(EDIT, 1, sub);
      _errorMap[sub] = val;
    }
  else
      sub->update(EDIT, 0, sub);
}
