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

#ifndef __YDFXGUIPORTSSELECTOR_HXX__
#define __YDFXGUIPORTSSELECTOR_HXX__

#include "ydfxwidgetsExport.hxx"

#include "YDFXGUIWrap.hxx"

#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QItemDelegate>

#include <vector>

class QValidator;

class YDFXGUIInputPortValueEditor;

class ColoredString
{
public:
  ColoredString(const QString& str):_str(str) { }
  void setColor(const QColor& col) { _col=col; }
  const QString& str() const { return _str; }
  const QColor& color() const { return _col; }
private:
  QString _str;
  QColor _col;
};

class MyWidgetPainter
{
public:
  virtual ~MyWidgetPainter() { }
  virtual void paintEvent(QPaintEvent *event) = 0;
  virtual QSize sizeHint() const = 0;
  virtual void appendLetter(const QString& letter) = 0;
  virtual void supprLetter() = 0;
  virtual bool isNotSelected() const = 0;
public:
  void changeAngle();
protected:
  MyWidgetPainter(YDFXGUIInputPortValueEditor *wid):_wid(wid),_angle(120) { }
  YDFXGUIInputPortValueEditor *getWidget() const { return _wid; }
protected:
  void paintDataCommonPart(QPaintEvent *event, const QString& text);
  void drawPadlock(QWidget *wid) const;
  void drawDice(QWidget *wid) const;
private:
  YDFXGUIInputPortValueEditor *_wid;
  int _angle;
public:
  static const int SZ_OF_PEN_RECT;
  static const int PADLOCK_X;
  static const int PADLOCK_Y;
  static const int DICE_X;
  static const int DICE_Y;
};

class MyWidgetPainterNoFocus : public MyWidgetPainter
{
public:
  MyWidgetPainterNoFocus(YDFXGUIInputPortValueEditor *wid):MyWidgetPainter(wid) { }
private:
  template<class FontBrushSetGet>
  static void prepareFontAndBrush(FontBrushSetGet *fbsg);
private:
  virtual ~MyWidgetPainterNoFocus() { }
  void paintEvent(QPaintEvent *event);
  QSize sizeHint() const;
  void appendLetter(const QString& letter);
  void supprLetter();
  bool isNotSelected() const { return true; }
};

class MyWidgetPainterFocus : public MyWidgetPainter
{
public:
  MyWidgetPainterFocus(YDFXGUIInputPortValueEditor *wid):MyWidgetPainter(wid) { }
  QString getText() const { return _textEntered; }
private:
  template<class FontBrushSetGet>
  static void prepareFontAndBrush(FontBrushSetGet *fbsg);
private:
  virtual ~MyWidgetPainterFocus() { }
  void paintEvent(QPaintEvent *event);
  QSize sizeHint() const;
  void appendLetter(const QString& letter);
  void supprLetter();
  bool isNotSelected() const { return false; }
private:
  QString _textEntered;
};

class YDFXGUIInputPortValueEditor : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QString text READ text WRITE setText)
public:
  YDFXWIDGETS_EXPORT YDFXGUIInputPortValueEditor(YACSEvalInputPort *inp);
  YDFXWIDGETS_EXPORT ~YDFXGUIInputPortValueEditor();
  YDFXWIDGETS_EXPORT QString text() const;
  YDFXWIDGETS_EXPORT void setText(const QString& text);
  YDFXWIDGETS_EXPORT void paintEvent(QPaintEvent *event);
  YDFXWIDGETS_EXPORT void mousePressEvent(QMouseEvent *event);
  YDFXWIDGETS_EXPORT void mouseReleaseEvent(QMouseEvent *event);
  YDFXWIDGETS_EXPORT void keyPressEvent(QKeyEvent *event);
  YDFXWIDGETS_EXPORT void focusOutEvent(QFocusEvent * event);
  YDFXWIDGETS_EXPORT QSize sizeHint() const;
  YDFXWIDGETS_EXPORT QSize minimumSizeHint() const;
public:
  YDFXWIDGETS_EXPORT bool isOK() const;
  YDFXWIDGETS_EXPORT bool isRandom() const;
  YDFXWIDGETS_EXPORT YACSEvalInputPort *getPort() const { return _inp; }
  YDFXWIDGETS_EXPORT static bool IsOK(YACSEvalInputPort *inp);
public slots:
  YDFXWIDGETS_EXPORT void applyModificationOfLook();
  YDFXWIDGETS_EXPORT bool toggleRandom();
signals:
  void changeLook();
  void statusChanged();
private:
  YACSEvalInputPort *_inp;
  MyWidgetPainter *_zePainter;
  QValidator *_valid;
};

class HoverLabel : public QLabel
{
public:
  YDFXWIDGETS_EXPORT HoverLabel(QWidget *parent);
  YDFXWIDGETS_EXPORT void mousePressEvent(QMouseEvent *event);
  YDFXWIDGETS_EXPORT void mouseReleaseEvent(QMouseEvent *event);
  YDFXWIDGETS_EXPORT void enterEvent(QEvent *event);
  YDFXWIDGETS_EXPORT void leaveEvent(QEvent *event);
  YDFXWIDGETS_EXPORT void paintEvent(QPaintEvent *event);
  YDFXWIDGETS_EXPORT QSize sizeHint() const;
private:
  virtual QSize sizeHintNotHovered() const = 0;
  virtual void paintIfNotOn(QPaintEvent *event) = 0;
  virtual void pressOccured() = 0;
  virtual std::vector<ColoredString> textForEmulatedPushButton() const = 0;
  static void AssignFontOnHover(QFont& ft);
private:
  bool _isin;
  bool _isPressed;
  static const int PT_SZ_ON_HOVER;
};

class InputLabel : public HoverLabel
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT InputLabel(YACSEvalInputPort *inp, QWidget *parent);
private:
  QSize sizeHintNotHovered() const;
  void paintIfNotOn(QPaintEvent *event);
  void pressOccured();
  std::vector<ColoredString> textForEmulatedPushButton() const;
public:
  YDFXWIDGETS_EXPORT static void AssignTextAndTooltip(QLabel *wid, YACSEvalInputPort *port);
signals:
  YDFXWIDGETS_EXPORT void randomnessStatusChanged();
private:
  YACSEvalInputPort *_inp;
};

class InputLabelNonToggle : public QLabel
{
public:
  YDFXWIDGETS_EXPORT InputLabelNonToggle(YDFXGUIInputPortValueEditor *wid, QWidget *parent);
private:
  YDFXGUIInputPortValueEditor *_wid;
};

class OutputLabel : public HoverLabel
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT OutputLabel(YACSEvalOutputPort *outp, QWidget *parent);
  YDFXWIDGETS_EXPORT bool isQOfInt() const;
private:
  QSize sizeHintNotHovered() const;
  void paintIfNotOn(QPaintEvent *event);
  void pressOccured();
  std::vector<ColoredString> textForEmulatedPushButton() const;
  QSize minimumSizeHint() const;
  static void ComputePointsToCircle(const QString& txt, const QRect& refRect, const QFont& font, QPointF pts[7]);
signals:
  void clicked();
private:
  static const int PEN_SZ_CIRCLED;
private:
  YACSEvalOutputPort *_outp;
};

class YDFXGUIOKCls
{
public:
  YDFXWIDGETS_EXPORT YDFXGUIOKCls():_isOK(false) { }
  YDFXWIDGETS_EXPORT bool wasOK() const { return _isOK; }
  YDFXWIDGETS_EXPORT void setWasOKStatus(bool newStatus) { _isOK=newStatus; }
  YDFXWIDGETS_EXPORT virtual bool isOK() const = 0;
protected:
  void initOK();
private:
  bool _isOK;
};

class YDFXGUIGatherPorts : public QWidget, public YDFXGUIOKCls
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUIGatherPorts(QWidget *parent):QWidget(parent) { }
public slots:
  YDFXWIDGETS_EXPORT void somebodyChangedStatus();
signals:
  void theGlobalStatusChanged(bool newStatus);
};

class YDFXGUIInputPortsSelector : public YDFXGUIGatherPorts
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUIInputPortsSelector(YACSEvalYFXWrap *efx, QWidget *parent=0);
  YDFXWIDGETS_EXPORT ~YDFXGUIInputPortsSelector();
  YDFXWIDGETS_EXPORT const std::vector< YACSEvalInputPort * >& getInputs() const { return _inps; }
  YDFXWIDGETS_EXPORT bool isOK() const;
  YDFXWIDGETS_EXPORT bool areSeqWellDefined() const;
  YDFXWIDGETS_EXPORT static void DrawWarningSign(QPainter& painter, int width0, int height0);
private:
  void fillWidget();
  void showEvent(QShowEvent *e);
  void timerEvent(QTimerEvent *e);
  void paintEvent(QPaintEvent *e);
private:
  std::vector< YACSEvalInputPort * > _inps;
  int _timerId;
};

class YDFXGUIOutputPortsSelector : public YDFXGUIGatherPorts
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUIOutputPortsSelector(YACSEvalYFXWrap *efx, QWidget *parent=0);
  YDFXWIDGETS_EXPORT const std::vector< YACSEvalOutputPort * >& getOutputs() const { return _outps; }
  YDFXWIDGETS_EXPORT bool isOK() const;
private:
  void fillWidget();
  void paintEvent(QPaintEvent *e);
private:
  std::vector< YACSEvalOutputPort * > _outps;
};

class YDFXGUIAbstractPorts : public QWidget
{
protected:
  YDFXGUIAbstractPorts(YACSEvalYFXWrap *efx, QWidget *parent):QWidget(parent) {  }
  QScrollArea *setupWidgets(const QString& title, QWidget *zeWidget);
  virtual YDFXGUIGatherPorts *getPortsManager() = 0;
  virtual const YDFXGUIGatherPorts *getPortsManager() const = 0;
};

class YDFXGUIInputPorts : public YDFXGUIAbstractPorts
{
public:
  YDFXWIDGETS_EXPORT YDFXGUIInputPorts(YACSEvalYFXWrap *efx, QWidget *parent);
  YDFXWIDGETS_EXPORT bool isOK() const { return _inputsSelector->isOK(); }
  YDFXWIDGETS_EXPORT bool areSeqWellDefined() const { return _inputsSelector->areSeqWellDefined(); }
  YDFXWIDGETS_EXPORT YDFXGUIInputPortsSelector *getInputsSelector() const { return _inputsSelector; }
  YDFXWIDGETS_EXPORT YDFXGUIGatherPorts *getPortsManager() { return _inputsSelector; }
  YDFXWIDGETS_EXPORT const YDFXGUIGatherPorts *getPortsManager() const { return _inputsSelector; }
private:
  YDFXGUIInputPortsSelector *_inputsSelector;
};

class YDFXGUIOutputPorts : public YDFXGUIAbstractPorts
{
public:
  YDFXWIDGETS_EXPORT YDFXGUIOutputPorts(YACSEvalYFXWrap *efx, QWidget *parent);
  YDFXWIDGETS_EXPORT bool isOK() const { return _outputsSelector->isOK(); }
  YDFXWIDGETS_EXPORT YDFXGUIGatherPorts *getPortsManager() { return _outputsSelector; }
  YDFXWIDGETS_EXPORT const YDFXGUIGatherPorts *getPortsManager() const { return _outputsSelector; }
private:
  YDFXGUIOutputPortsSelector *_outputsSelector;
};

class YDFXGUIAllPorts : public QWidget
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUIAllPorts(YACSEvalYFXWrap *efx, QWidget *parent);
  YDFXWIDGETS_EXPORT bool isOK() const;
private:
  YDFXGUIInputPorts *_in;
  YDFXGUIOutputPorts *_out;
public slots:
  YDFXWIDGETS_EXPORT void somethingChangedInPorts(bool status);
signals:
  void sequencesCanBeDefinedSignal(bool status);
  void canBeExecutedSignal(bool status);
};

#endif
