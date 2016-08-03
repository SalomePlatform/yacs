// Copyright (C) 2016  CEA/DEN, EDF R&D
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
// Author : Anthony Geay (EDF R&D)

#include "YDFXGUISeqInit.hxx"

#include <QFile>
#include <QFrame>
#include <QPainter>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QApplication>

#include "Python.h"

#include "AutoGIL.hxx"

#include "YDFXGUIWrap.hxx"
#include "YDFXGUIPyThreadSaver.hxx"

#include "YACSEvalPort.hxx"
#include "YACSEvalSeqAny.hxx"

#include <iostream>
#include <limits>

const char YDFXGUIStatus::OK_STR[]="OK !";

class AutoPyRef
{
public:
  AutoPyRef(PyObject *pyobj=0):_pyobj(pyobj) { }
  ~AutoPyRef() { release(); }
  AutoPyRef(const AutoPyRef& other):_pyobj(other._pyobj) { if(_pyobj) Py_XINCREF(_pyobj); }
  AutoPyRef& operator=(const AutoPyRef& other) { if(_pyobj==other._pyobj) return *this; release(); _pyobj=other._pyobj; Py_XINCREF(_pyobj); return *this; }
  operator PyObject *() { return _pyobj; }
  void set(PyObject *pyobj) { if(pyobj==_pyobj) return ; release(); _pyobj=pyobj; }
  PyObject *get() { return _pyobj; }
  bool isNull() const { return _pyobj==0; }
  PyObject *retn() { if(_pyobj) Py_XINCREF(_pyobj); return _pyobj; }
private:
  void release() { if(_pyobj) Py_XDECREF(_pyobj); _pyobj=0; }
private:
  PyObject *_pyobj;
};

////////////////////////////

void YDFXGUIDoubleVectHolder::applyOnInput(YACSEvalInputPort *inp) const
{
  YACSEvalSeqAny *val(new YACSEvalSeqAnyDouble(_vect));
  inp->setSequenceOfValuesToEval(val);
  delete val;
}

////////////////////////////

YDFXGUISeqSetterP::YDFXGUISeqSetterP(QWidget *parent):QPushButton(parent),_isIn(false)
{
  setText("Open...");
  connect(this,SIGNAL(clicked()),this,SLOT(selectAFile()));
}

void YDFXGUISeqSetterP::loadState(const QString& state)
{
  _fileName=state;
  setToolTip(_fileName);
}

QString YDFXGUISeqSetterP::saveState() const
{
  return _fileName;
}

bool YDFXGUISeqSetterP::executeScript(int& sz)
{
  QObject *zeBoss(parent()->parent());
  YDFXGUISeqLine *zeBossc(qobject_cast<YDFXGUISeqLine *>(zeBoss));
  if(!zeBossc)
    return false;
  //
  if(_fileName.isEmpty())
    {
      emit problemDetected(QString("For \"%1\" : no file defined !").arg(zeBossc->getName()));
      return false;
    }
  QFile file(_fileName);
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return false;
  int i=0;
  _vect.clear();
  while(!file.atEnd())
    {
      QByteArray line(file.readLine());
      QString line2(line.data());
      bool isOK(false);
      double v(line2.toDouble(&isOK));
      if(!isOK)
        {
          emit problemDetected(QString("For \"%1\" : At line %2 it is not a float !").arg(zeBossc->getName()).arg(i));
          return false;
        }
      _vect.push_back(v);
      i++;
    }
  sz=_vect.size();
  return true;
}

void YDFXGUISeqSetterP::enterEvent(QEvent *event)
{
  _isIn=true;
  update();
}

void YDFXGUISeqSetterP::leaveEvent(QEvent *event)
{
  _isIn=false;
  update();
}

void YDFXGUISeqSetterP::paintEvent(QPaintEvent *event)
{
  if(_isIn || _fileName.isEmpty())
    {
      QPushButton::paintEvent(event);
      return ;
    }
  QFileInfo fi(_fileName);
  QString txt(fi.fileName());
  QRect refRect(rect());
  QFont ft(font());
  QFontMetrics fm(ft);
  QSize refRect2(fm.boundingRect(txt).size());
  //
  QPainter painter(this);
  painter.drawText(QPoint((refRect.width()-refRect2.width())/2,
                          refRect.height()/2+refRect2.height()/2-fm.descent()),txt);
}

void YDFXGUISeqSetterP::selectAFile()
{
  QFileDialog fd(this,QString("Select a file containing list of floats."));
  fd.setFileMode(QFileDialog::ExistingFile);
  if(fd.exec())
    {
      QStringList files(fd.selectedFiles());
      if(files.size()>=1)
        {
          _fileName=files[0];
        }
      setToolTip(_fileName);
      update();
    }
}

////////////////////////////

void YDFXGUISeqSetterT::loadState(const QString& state)
{
  setText(state);
}

QString YDFXGUISeqSetterT::saveState() const
{
  return toPlainText();
}

bool YDFXGUISeqSetterT::executeScript(int& sz)
{
  QObject *zeBoss(parent()->parent());
  YDFXGUISeqLine *zeBossc(qobject_cast<YDFXGUISeqLine *>(zeBoss));
  if(!zeBossc)
    return false;
  //
  std::string name(zeBossc->getName().toStdString());
  //
  static const char TMP_FILENAME[]="/tmp/TMP";
  std::string txt(toPlainText().toStdString());
  YDFXGUIPyThreadSaver::SaveContext(QApplication::instance()->thread());
  {
    YACS::ENGINE::AutoGIL gal;
    AutoPyRef code(Py_CompileString(txt.c_str(),TMP_FILENAME, Py_file_input));
    if(code.get() == NULL)
      {
        emit problemDetected(QString("For \"%1\" : python code is invalid !").arg(zeBossc->getName()));
        return false;
      }
    AutoPyRef context(PyDict_New());
    PyDict_SetItemString( context, "__builtins__", PyEval_GetBuiltins() );
    AutoPyRef res(PyEval_EvalCode((PyCodeObject *)code.get(), context, context));
    PyObject *item(PyDict_GetItemString(context,name.c_str()));
    //
    if(!item)
      {
        emit problemDetected(QString("For \"%1\" : Py var %1 is not defined !").arg(zeBossc->getName()));
        return false;
      }
    if(!PyList_Check(item))
      {
        emit problemDetected(QString("For \"%1\" : Py var %1 must be a list !").arg(zeBossc->getName()));
        return false;
      }
    sz=PyList_Size(item);
    _vect.clear() ; _vect.resize(sz);
    for(int i=0;i<sz;i++)
      {
        PyObject *val(PyList_GetItem(item,i));
        if(!PyFloat_Check(val))
          {
            emit problemDetected(QString("For \"%1\" : At pos %2 of python list, it is not a float !").arg(zeBossc->getName()).arg(i));
            return false;
          }
        _vect[i]=PyFloat_AS_DOUBLE(val);
      }
  }
  return true;
}

////////////////////////////

YDFXGUICombo::YDFXGUICombo(QWidget *parent):QComboBox(parent),_isIn(false)
{
  setFocusPolicy(Qt::TabFocus);
}

QString YDFXGUICombo::getName()
{
  QString ret;
  YDFXGUISeqLine *parentc(qobject_cast<YDFXGUISeqLine *>(parent()));
  if(parentc)
    ret=parentc->getName();
  return ret;
}

void YDFXGUICombo::enterEvent(QEvent *event)
{
  _isIn=true;
  update();
}

void YDFXGUICombo::leaveEvent(QEvent *event)
{
  _isIn=false;
  update();
}

void YDFXGUICombo::paintEvent(QPaintEvent *event)
{
  if(_isIn)
    {
      QComboBox::paintEvent(event);
      return ;
    }
  QRect refRect(rect());
  QFont ft(font());
  ft.setBold(true);
  QFontMetrics fm(ft);
  QSize refRect2(fm.boundingRect(getName()).size());
  QPainter painter(this);
  painter.setFont(ft);
  QPen pen(painter.pen());
  pen.setColor(Qt::red);
  painter.setPen(pen);
  painter.drawText(QPoint((refRect.width()-refRect2.width())/2,refRect.height()/2+refRect2.height()/2-fm.descent()),getName());
}

////////////////////////////

YDFXGUISeqSetter::YDFXGUISeqSetter(QWidget *parent, const QString& name):QWidget(parent),_textEdit(0),_push(0),_curType(0)
{
  QVBoxLayout *verticalLayout(new QVBoxLayout(this));
  _textEdit=new YDFXGUISeqSetterT(this); verticalLayout->addWidget(_textEdit);
  _push=new YDFXGUISeqSetterP(this); verticalLayout->addWidget(_push);
  _textEdit->setText(QString("import math\n%1=[math.sqrt(float(elt)+0.) for elt in xrange(4)]").arg(name));
  _textEdit->hide();
  _push->hide();
}

int YDFXGUISeqSetter::loadState(const QString& state)
{
  if(state.isEmpty() || state.size()<3)
    return 0;
  QString sw(state.mid(0,3));
  if(sw=="@0@")
    { 
      _push->loadState(state.mid(3));
      return 0;
    }
  if(sw=="@1@")
    {
      
      _textEdit->loadState(state.mid(3));
      return 1;
    }
  return 0;
}

QString YDFXGUISeqSetter::saveState() const
{
  YDFXGUISeqLine *parentc(qobject_cast<YDFXGUISeqLine *>(parent()));
  if(!parentc)
    return QString();
  int pos(parentc->getPositionOfCombo());
  if(pos==0)
    {
      QString ret("@0@");
      ret+=_push->saveState();
      return ret;
    }
  if(pos==1)
    {
      QString ret("@1@");
      ret+=_textEdit->saveState();
      return ret;
    }
  return QString();
}

QSize YDFXGUISeqSetter::sizeHint() const
{
  QSize sz,sz2(QWidget::sizeHint());
  if(_curType==0)
    sz=_push->sizeHint();
  else
    sz=_textEdit->sizeHint();
  sz.rwidth()+=sz2.rwidth();
  sz.rheight()+=sz2.rheight();
  return sz;
}

QSize YDFXGUISeqSetter::minimumSizeHint() const
{
  QSize sz,sz2(QWidget::minimumSizeHint());
  if(_curType==0)
    sz=_push->minimumSizeHint();
  else
    sz=_textEdit->minimumSizeHint();
  sz.rwidth()+=sz2.rwidth();
  sz.rheight()+=sz2.rheight();
  return sz;
}

bool YDFXGUISeqSetter::checkOK(int& sz)
{
  if(_curType==0)
    return _push->executeScript(sz);
  else
    return _textEdit->executeScript(sz);
}

void YDFXGUISeqSetter::applyOnInput(YACSEvalInputPort *inp)
{
  if(_curType==0)
    _push->applyOnInput(inp);
  else
    _textEdit->applyOnInput(inp);
}

void YDFXGUISeqSetter::typeOfAssignmentChanged(int newType)
{
  _curType=newType;
  if(newType==0)
    {
      _textEdit->hide();
      _push->show();
      disconnect(_textEdit,SIGNAL(problemDetected(const QString&)),this,SIGNAL(problemDetected(const QString&)));
      connect(_push,SIGNAL(problemDetected(const QString&)),this,SIGNAL(problemDetected(const QString&)));
    }
  else
    {
      _textEdit->show();
      _push->hide();
      disconnect(_push,SIGNAL(problemDetected(const QString&)),this,SIGNAL(problemDetected(const QString&)));
      connect(_textEdit,SIGNAL(problemDetected(const QString&)),this,SIGNAL(problemDetected(const QString&)));
    }
}

////////////////////////////

YDFXGUISeqLine::YDFXGUISeqLine(QWidget *parent, YACSEvalInputPort *inp):_combo(0),_setter(0),_inp(inp)
{
  QHBoxLayout *horizontalLayout(new QHBoxLayout(this));
  _combo=new YDFXGUICombo(this);
  _combo->insertItem(0,QString("%1 from file").arg(getName()));
  _combo->insertItem(1,QString("%1 from PyScript").arg(getName()));
  horizontalLayout->addWidget(_combo);
  _setter=new YDFXGUISeqSetter(this,getName());
  connect(_combo,SIGNAL(currentIndexChanged(int)),this,SLOT(typeOfAssignmentChanged(int)));
  horizontalLayout->addWidget(_setter);
  _combo->setCurrentIndex(0);
  emit _combo->currentIndexChanged(0);//to be sure to sync widgets
}

void YDFXGUISeqLine::loadState(const QMap<QString,QString>& state)
{
  QString name(getName());
  QMap<QString,QString>::const_iterator it(state.find(name));
  if(it==state.end())
    return ;
  int pos(_setter->loadState(it.value()));
  _combo->setCurrentIndex(pos);
}

void YDFXGUISeqLine::saveState(QMap<QString,QString>& state) const
{
  state[getName()]=_setter->saveState();
}

QString YDFXGUISeqLine::getName() const
{
  return QString(_inp->getName().c_str());
}

int YDFXGUISeqLine::getPositionOfCombo() const
{
  return _combo->currentIndex();
}

bool YDFXGUISeqLine::checkOK(int& sz)
{
  return _setter->checkOK(sz);
}

void YDFXGUISeqLine::connectToStatus(YDFXGUIStatus *status)
{
  connect(_setter,SIGNAL(problemDetected(const QString&)),status,SLOT(displayInfo(const QString&)));
}

void YDFXGUISeqLine::applyOnInput()
{
  _setter->applyOnInput(_inp);
}

void YDFXGUISeqLine::typeOfAssignmentChanged(int newType)
{
  _setter->typeOfAssignmentChanged(newType);
  _setter->updateGeometry();
  _setter->update();
}

////////////////////////////

YDFXGUIStatus::YDFXGUIStatus(QWidget *parent):QWidget(parent)
{
}

void YDFXGUIStatus::paintEvent(QPaintEvent *event)
{
  QRect refRect(rect());
  QFont ft(font());
  QFontMetrics fm(ft);
  QSize refRect2(fm.boundingRect(_text).size());
  //
  QPainter painter(this);
  QPen pen(painter.pen());
  pen.setColor(_text==OK_STR?Qt::green:Qt::red);
  painter.setPen(pen);
  painter.drawText(QPoint((refRect.width()-refRect2.width())/2,
                          refRect.height()/2+refRect2.height()/2-fm.descent()),_text);
}

QSize YDFXGUIStatus::sizeHint() const
{
  QFont ft(font());
  QFontMetrics fm(ft);
  QSize ret(fm.boundingRect(_text).size());
  return ret;
}

QSize YDFXGUIStatus::minimumSizeHint() const
{
  return sizeHint();
}

void YDFXGUIStatus::declareOK(bool isOK)
{
  if(!isOK)
    return ;
  _text=OK_STR;
  updateGeometry();
  update();
}

void YDFXGUIStatus::displayInfo(const QString& txt)
{
  _text=txt;
  updateGeometry();
  update();
}

////////////////////////////

YDFXGUISeqInitEff::YDFXGUISeqInitEff(QWidget *parent, YACSEvalYFXWrap *efx):QWidget(parent)
{
  QVBoxLayout *verticalLayout(new QVBoxLayout(this));
  std::vector< YACSEvalInputPort * > inputs(efx->getFreeInputPorts());
  foreach(YACSEvalInputPort *input,inputs)
    {
      if(!input->isRandomVar())
        continue;
      YDFXGUISeqLine *line(new YDFXGUISeqLine(this,input));
      verticalLayout->addWidget(line);
      _lines.push_back(line);
    }
}

void YDFXGUISeqInitEff::loadState(const QMap<QString,QString>& state)
{
  foreach(YDFXGUISeqLine *line,_lines)
    line->loadState(state);
}

QMap<QString,QString> YDFXGUISeqInitEff::saveState() const
{
  QMap<QString,QString> ret;
  foreach(YDFXGUISeqLine *line,_lines)
    line->saveState(ret);
  return ret;
}

void YDFXGUISeqInitEff::connectToStatus(YDFXGUIStatus *status)
{
  foreach(YDFXGUISeqLine *line,_lines)
    {
      line->connectToStatus(status);
    }
}

void YDFXGUISeqInitEff::assignButtonClicked()
{
  int sz;
  bool verdict(checkConsistency(sz));
  emit configurationIsOK(verdict);
}

void YDFXGUISeqInitEff::applyOnEFX()
{
  foreach(YDFXGUISeqLine *line,_lines)
    line->applyOnInput();
}

bool YDFXGUISeqInitEff::checkConsistency(int& sz)
{
  int refSz(std::numeric_limits<int>::max());
  foreach(YDFXGUISeqLine *line,_lines)
    {
      int locSz;
      if(!line->checkOK(locSz))
        return false;
      if(refSz==std::numeric_limits<int>::max())
        refSz=locSz;
      if(locSz!=refSz)
        {
          emit line->setter()->problemDetected(QString("Var %1 does not have the same number of elts than others !").arg(line->getName()));
          return false;
        }
    }
  sz=refSz;
  return true;
}

////////////////////////////

YDFXGUISeqInit::YDFXGUISeqInit(QWidget *parent, YACSEvalYFXWrap *efx):QWidget(parent),_zeWidget(0)
{
  QVBoxLayout *verticalLayout(new QVBoxLayout(this));
  QFrame *frame(new QFrame(this));
  frame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  verticalLayout->addWidget(frame);
  QHBoxLayout *horizontalLayout2(new QHBoxLayout(frame));
  QScrollArea *sa(new QScrollArea(frame));
  horizontalLayout2->addWidget(sa);
  _zeWidget=new YDFXGUISeqInitEff(sa,efx);
  sa->setWidgetResizable(true);
  sa->setWidget(_zeWidget);
  //
  QHBoxLayout *horizontalLayout(new QHBoxLayout);
  QSpacerItem *si(new QSpacerItem(40,20,QSizePolicy::Expanding,QSizePolicy::Minimum));
  YDFXGUIStatus *statusInfo(new YDFXGUIStatus(this));
  _zeWidget->connectToStatus(statusInfo);
  connect(_zeWidget,SIGNAL(configurationIsOK(bool)),statusInfo,SLOT(declareOK(bool)));
  QPushButton *button(new QPushButton(this));
  button->setText("Check");
  QPushButton *button2(new QPushButton(this));
  button2->setText("Assign !");
  connect(_zeWidget,SIGNAL(configurationIsOK(bool)),button2,SLOT(setEnabled(bool)));
  connect(button2,SIGNAL(clicked(bool)),_zeWidget,SLOT(applyOnEFX()));
  connect(button2,SIGNAL(clicked(bool)),this,SIGNAL(assignButtonClicked()));
  horizontalLayout->addWidget(statusInfo);
  horizontalLayout->addItem(si);
  horizontalLayout->addWidget(button);
  horizontalLayout->addWidget(button2);
  connect(button,SIGNAL(pressed()),_zeWidget,SLOT(assignButtonClicked()));
  button2->setEnabled(false);
  //
  verticalLayout->addLayout(horizontalLayout);
}

void YDFXGUISeqInit::loadState(const QMap<QString,QString>& state)
{
  _zeWidget->loadState(state);
}

QMap<QString,QString> YDFXGUISeqInit::saveState() const
{
  return _zeWidget->saveState();
}
