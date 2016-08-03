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

#ifndef __YDFXGUISEQINIT_HXX__
#define __YDFXGUISEQINIT_HXX__

#include "ydfxwidgetsExport.hxx"

#include <QWidget>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>

#include <vector>

class QTextEdit;
class QComboBox;
class QPushButton;

class YACSEvalYFXWrap;
class YACSEvalInputPort;

class YDFXGUIDoubleVectHolder
{
public:
  YDFXWIDGETS_EXPORT YDFXGUIDoubleVectHolder() { }
  YDFXWIDGETS_EXPORT virtual bool executeScript(int& sz) = 0;
  YDFXWIDGETS_EXPORT void applyOnInput(YACSEvalInputPort *inp) const;
protected:
  std::vector<double> _vect;
};

class YDFXGUISeqSetterP : public QPushButton, public YDFXGUIDoubleVectHolder
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUISeqSetterP(QWidget *parent=0);
  YDFXWIDGETS_EXPORT void loadState(const QString& state);
  YDFXWIDGETS_EXPORT QString saveState() const;
  YDFXWIDGETS_EXPORT bool executeScript(int& sz);
  YDFXWIDGETS_EXPORT void enterEvent(QEvent *event);
  YDFXWIDGETS_EXPORT void leaveEvent(QEvent *event);
  YDFXWIDGETS_EXPORT void paintEvent(QPaintEvent *event);
public slots:
  YDFXWIDGETS_EXPORT void selectAFile();
signals:
  void problemDetected(const QString& msg);
private:
  bool _isIn;
  QString _fileName;
};

class YDFXGUISeqSetterT : public QTextEdit, public YDFXGUIDoubleVectHolder
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUISeqSetterT(QWidget *parent=0):QTextEdit(parent) { }
  YDFXWIDGETS_EXPORT void loadState(const QString& state);
  YDFXWIDGETS_EXPORT QString saveState() const;
  YDFXWIDGETS_EXPORT bool executeScript(int& sz);
signals:
  void problemDetected(const QString& msg);
};

class YDFXGUICombo : public QComboBox
{
public:
  YDFXWIDGETS_EXPORT YDFXGUICombo(QWidget *parent);
  YDFXWIDGETS_EXPORT QString getName();
  YDFXWIDGETS_EXPORT void enterEvent(QEvent *event);
  YDFXWIDGETS_EXPORT void leaveEvent(QEvent *event);
  YDFXWIDGETS_EXPORT void paintEvent(QPaintEvent *event);
private:
  bool _isIn;
};

class YDFXGUISeqSetter : public QWidget
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUISeqSetter(QWidget *parent, const QString& name);
  YDFXWIDGETS_EXPORT int loadState(const QString& state);
  YDFXWIDGETS_EXPORT QString saveState() const;
  YDFXWIDGETS_EXPORT QSize sizeHint() const;
  YDFXWIDGETS_EXPORT QSize minimumSizeHint() const;
  YDFXWIDGETS_EXPORT bool checkOK(int& sz);
  YDFXWIDGETS_EXPORT void applyOnInput(YACSEvalInputPort *inp);
public:
  YDFXWIDGETS_EXPORT void typeOfAssignmentChanged(int newType);
signals:
  void problemDetected(const QString& msg);
private:
  YDFXGUISeqSetterT *_textEdit;
  YDFXGUISeqSetterP *_push;
  int _curType;
};

class YDFXGUIStatus;

class YDFXGUISeqLine : public QWidget
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUISeqLine(QWidget *parent, YACSEvalInputPort *inp);
  YDFXWIDGETS_EXPORT void loadState(const QMap<QString,QString>& state);
  YDFXWIDGETS_EXPORT void saveState(QMap<QString,QString>& state) const;
  YDFXWIDGETS_EXPORT QString getName() const;
  YDFXWIDGETS_EXPORT int getPositionOfCombo() const;
  YDFXWIDGETS_EXPORT bool checkOK(int& sz);
  YDFXWIDGETS_EXPORT void connectToStatus(YDFXGUIStatus *status);
  YDFXWIDGETS_EXPORT YDFXGUISeqSetter *setter() { return _setter; }
  YDFXWIDGETS_EXPORT void applyOnInput();
public slots:
  void typeOfAssignmentChanged(int newType);
private:
  YDFXGUICombo *_combo;
  YDFXGUISeqSetter *_setter;
  YACSEvalInputPort *_inp;
};

class YDFXGUIStatus : public QWidget
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUIStatus(QWidget *parent);
  YDFXWIDGETS_EXPORT void paintEvent(QPaintEvent *event);
  YDFXWIDGETS_EXPORT QSize sizeHint() const;
  YDFXWIDGETS_EXPORT QSize minimumSizeHint() const;
public slots:
  YDFXWIDGETS_EXPORT void declareOK(bool);
  YDFXWIDGETS_EXPORT void displayInfo(const QString& txt);
public:
  static const char OK_STR[];
private:
  QString _text;
};

class YDFXGUISeqInitEff : public QWidget
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUISeqInitEff(QWidget *parent, YACSEvalYFXWrap *efx);
  YDFXWIDGETS_EXPORT void loadState(const QMap<QString,QString>& state);
  YDFXWIDGETS_EXPORT QMap<QString,QString> saveState() const;
  YDFXWIDGETS_EXPORT void connectToStatus(YDFXGUIStatus *status);
public slots:
  YDFXWIDGETS_EXPORT void assignButtonClicked();
  YDFXWIDGETS_EXPORT void applyOnEFX();
signals:
  void configurationIsOK(bool);
private:
  bool checkConsistency(int& sz);
private:
  std::vector<YDFXGUISeqLine *> _lines;
};

class YDFXGUISeqInit : public QWidget
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUISeqInit(QWidget *parent, YACSEvalYFXWrap *efx);
  YDFXWIDGETS_EXPORT void loadState(const QMap<QString,QString>& state);
  YDFXWIDGETS_EXPORT QMap<QString,QString> saveState() const;
signals:
  void assignButtonClicked();
private:
  YDFXGUISeqInitEff *_zeWidget;
};

#endif
