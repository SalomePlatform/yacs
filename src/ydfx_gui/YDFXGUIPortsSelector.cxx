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

#include "YDFXGUIPortsSelector.hxx"
#include "YDFXGUIPortsValidator.hxx"

#include "YACSEvalPort.hxx"
#include "YACSEvalSeqAny.hxx"

#include <QLabel>
#include <QPainter>
#include <QLineEdit>
#include <QClipboard>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPaintEvent>
#include <QApplication>
#include <QSplitter>

#include <string>
#include <iostream>
#include <limits>

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#include <math.h>
#else
#include <cmath>
#endif

const int MyWidgetPainter::SZ_OF_PEN_RECT=2;
const int MyWidgetPainter::PADLOCK_X=130;
const int MyWidgetPainter::PADLOCK_Y=206;
const int MyWidgetPainter::DICE_X=225;
const int MyWidgetPainter::DICE_Y=225;
const int HoverLabel::PT_SZ_ON_HOVER=8;
const int OutputLabel::PEN_SZ_CIRCLED=5;

void MyWidgetPainter::drawPadlock(QWidget *wid) const
{
  static const int STRT_BASEY=86,H_CHORD=18;
  const float RATIO(float(PADLOCK_Y)/float(PADLOCK_X));
  QPainter painter(wid);
  int width(wid->width()),height(wid->height());
  if(float(height)>RATIO*float(width))
    painter.setViewport(0,int(height-RATIO*float(width))/2,width,width*RATIO);
  else
    painter.setViewport((float(width)-height/RATIO)/2,0,height/RATIO,height);//width-height/RATIO
  painter.setRenderHint(QPainter::Antialiasing,true);
  painter.setWindow(0,-STRT_BASEY,PADLOCK_X,PADLOCK_Y);
  painter.setPen(QPen(Qt::black,SZ_OF_PEN_RECT,Qt::SolidLine,Qt::SquareCap));
  
  float radius=PADLOCK_X*PADLOCK_X/(8*H_CHORD)+H_CHORD/2;
  float startAngle=acos(PADLOCK_X/(2.*radius))/M_PI*180*16;
  float spanAngle=2*asin(PADLOCK_X/(2*radius))/M_PI*180*16;
  
  //
  QColor darkYellow(194,170,99),brightYellow(255,250,219),zeYellow(255,212,80);
  QBrush brush(painter.brush());
  QLinearGradient grad(0,0,PADLOCK_X,0);
  grad.setColorAt(0.,darkYellow);
  grad.setColorAt(0.8,brightYellow);
  grad.setColorAt(1.,zeYellow);
  painter.setBrush(grad);
  //painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);// SourceOver by default
  painter.drawRect(QRect(SZ_OF_PEN_RECT,SZ_OF_PEN_RECT,PADLOCK_X-2*SZ_OF_PEN_RECT,PADLOCK_Y-STRT_BASEY-2*SZ_OF_PEN_RECT));
  //
  painter.setBrush(brush);
  painter.drawRect(QRect(0+SZ_OF_PEN_RECT/2,0+SZ_OF_PEN_RECT/2,PADLOCK_X-SZ_OF_PEN_RECT,PADLOCK_Y-STRT_BASEY-SZ_OF_PEN_RECT));
  QTransform t,t2;
  t.translate(0,-(STRT_BASEY+H_CHORD)) ; t.rotate(180) ; t.translate(-PADLOCK_X,-STRT_BASEY-2*H_CHORD);
  painter.setWorldTransform(t);
  painter.drawChord(QRect(-(radius-PADLOCK_X/2),0,2*radius,2*radius),180*16-startAngle,-spanAngle);
  painter.setWorldTransform(t2);
  //
  painter.drawLine(6,0,6,-24); painter.drawLine(25,0,25,-24);
  //painter.drawLine(103,0,103,-24); painter.drawLine(122,0,122,-24);
  const int r1(39),r2(58);//78==103-25 116==122-6
  const int center[2]={64,-24};
  float p1x=cos((180.-_angle)/180.*M_PI)*r1+center[0];
  float p1y=-sin((180.-_angle)/180.*M_PI)*r1+center[1];
  float p2x=cos((180.-_angle)/180.*M_PI)*r2+center[0];
  float p2y=-sin((180.-_angle)/180.*M_PI)*r2+center[1];
  painter.drawArc(QRect(25,-63,2*r1,2*r1),180*16,-_angle*16);
  painter.drawArc(QRect(6,-82,2*r2,2*r2),180*16,-_angle*16);
  painter.drawLine(p1x,p1y,p2x,p2y);
}

void MyWidgetPainter::drawDice(QWidget *wid) const
{
  QPainter painter(wid);
  int width(wid->width()),height(wid->height());
  if(height>width)
    painter.setViewport(0,(height-width)/2,width,width);
  else
    painter.setViewport((width-height)/2,0,height,height);
  painter.setRenderHint(QPainter::Antialiasing,true);
  painter.setWindow(0,0,DICE_X,DICE_Y);
  const QPoint face0[4]={QPoint(22,35),QPoint(113,110),QPoint(90,224),QPoint(0,148)};
  const QPoint face1[4]={QPoint(113,110),QPoint(224,73),QPoint(196,185),QPoint(90,224)};
  const QPoint face2[4]={QPoint(22,35),QPoint(113,110),QPoint(224,73),QPoint(126,0)};
  QLinearGradient grad(0,0,PADLOCK_X,0);
  QColor red0(154,18,20),red1(87,11,13),red2(205,25,24);
  painter.setBrush(QBrush(red0)); painter.drawPolygon(face0,4);
  painter.setBrush(QBrush(red1)); painter.drawPolygon(face1,4);
  painter.setBrush(QBrush(red2)); painter.drawPolygon(face2,4);
  //32x16 rot=12*16
  QColor grey(209,209,209);
  painter.setBrush(grey);
  //
  const int NB_POINTS=14;
  const float refs[NB_POINTS*2]={50,29,76,48,104,70,112,10,140,29,168,50,
                                 33,57,53,114,72,171,21,127,87,103,
                                 109,192,148,145,186,102
  };
  const int angles[NB_POINTS]={12,12,12,12,12,12,72,72,72,72,72,-47,-47,-47};
  for(int i=0;i<NB_POINTS;i++)
    {
      QTransform t;
      float angle(angles[i]);
      float angler(-angle/180*M_PI);
      float refX(refs[2*i]),refY(refs[2*i+1]);
      t.rotate(angle) ; t.translate((cos(angler)*refX-sin(angler)*refY),(sin(angler)*refX+cos(angler)*refY));
      painter.setWorldTransform(t);
      painter.drawEllipse(0,0,32,16);
    }
}

void MyWidgetPainter::changeAngle()
{
  const int resolution(5);
  _angle=(((_angle-120)+resolution)%60)+120;
}

void MyWidgetPainter::paintDataCommonPart(QPaintEvent *event, const QString& text)
{
  int width0(getWidget()->width()),height0(getWidget()->height());
  QRect refRect(0,0,width0,height0);
  QPainter painter(getWidget());
  //const QRect& refRect(event->rect());//getWidget()->frameGeometry()
  QRect refRect2(refRect);//event->rect());
  painter.setPen(QPen(Qt::red,SZ_OF_PEN_RECT,Qt::SolidLine,Qt::RoundCap));
  refRect2.translate(SZ_OF_PEN_RECT,SZ_OF_PEN_RECT);
  refRect2.setWidth(refRect2.width()-2*SZ_OF_PEN_RECT);;
  refRect2.setHeight(refRect2.height()-2*SZ_OF_PEN_RECT);
  painter.drawRect(refRect2);
  painter.setPen(QPen(Qt::black,10,Qt::SolidLine,Qt::RoundCap));
  QRect rect(painter.boundingRect(refRect,Qt::AlignTop,text));//Qt::AlignCenter
  const QFont& ft(getWidget()->font());
  QFontMetrics fm(ft);
  painter.drawText(QPoint(refRect.x()+SZ_OF_PEN_RECT+refRect2.width()/2-rect.width()/2,
                          refRect.y()+refRect.height()/2+rect.height()/2-fm.descent()),text);
}

template<class FontBrushSetGet>
void MyWidgetPainterNoFocus::prepareFontAndBrush(FontBrushSetGet *fbsg)
{
  QFont ft(fbsg->font());
  ft.setBold(true);
  fbsg->setFont(ft);
}

void MyWidgetPainterNoFocus::paintEvent(QPaintEvent *event)
{
  prepareFontAndBrush(getWidget());
  if(!getWidget()->isRandom())
    {
      if(getWidget()->text().isEmpty())
        drawPadlock(getWidget());
      else
        paintDataCommonPart(event,getWidget()->text());
    }
  else
    drawDice(getWidget());
}

QSize MyWidgetPainterNoFocus::sizeHint() const
{
  if(!getWidget()->isRandom())
    {
      if(getWidget()->text().isEmpty())
        return QSize(PADLOCK_X/6,PADLOCK_Y/6);
      else
        {
          QPixmap px(1000,1000);
          QPainter painter(&px);
          painter.setPen(QPen(Qt::black,5,Qt::SolidLine,Qt::RoundCap));
          prepareFontAndBrush(&painter);
          QRect rect(painter.boundingRect(QRect(),Qt::AlignTop,getWidget()->text()));
          return QSize(rect.width()+2*SZ_OF_PEN_RECT,rect.height()+2*SZ_OF_PEN_RECT);
        }
    }
  else
    return QSize(DICE_X/5,DICE_Y/5);
}

void MyWidgetPainterNoFocus::appendLetter(const QString& letter)
{
}

void MyWidgetPainterNoFocus::supprLetter()
{
}

template<class FontBrushSetGet>
void MyWidgetPainterFocus::prepareFontAndBrush(FontBrushSetGet *fbsg)
{
  QFont ft(fbsg->font());
  ft.setBold(false);
  fbsg->setFont(ft);
}

void MyWidgetPainterFocus::paintEvent(QPaintEvent *event)
{
  prepareFontAndBrush(getWidget());
  paintDataCommonPart(event,_textEntered);
}

QSize MyWidgetPainterFocus::sizeHint() const
{
  QPixmap px(1000,1000);
  QPainter painter(&px);
  painter.setPen(QPen(Qt::black,5,Qt::SolidLine,Qt::RoundCap));
  prepareFontAndBrush(&painter);
  QRect rect(painter.boundingRect(QRect(),Qt::AlignTop,_textEntered));
  return QSize(rect.width(),rect.height());
}

void MyWidgetPainterFocus::appendLetter(const QString& letter)
{
  _textEntered+=letter;
  getWidget()->updateGeometry();
  getWidget()->update();
}

void MyWidgetPainterFocus::supprLetter()
{
  _textEntered.chop(1);
  getWidget()->updateGeometry();
  getWidget()->update();
}

/////////////

YDFXGUIInputPortValueEditor::YDFXGUIInputPortValueEditor(YACSEvalInputPort *inp):_inp(inp),_zePainter(new MyWidgetPainterNoFocus(this)),_valid(0)
{
  _valid=BuildValidatorFromPort(this,_inp);
  QString txt(text());
  setText(txt);
  setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);//Fixed
  setFocusPolicy(Qt::ClickFocus);
}

YDFXGUIInputPortValueEditor::~YDFXGUIInputPortValueEditor()
{
  delete _zePainter;
}

QString YDFXGUIInputPortValueEditor::text() const
{
  QString txt;
  if(_inp->hasDefaultValueDefined())
    {
      YACSEvalAny *val(_inp->getDefaultValueDefined());
      txt=BuidStringFromAny(val);
      delete val;
    }
  return txt;
}

void YDFXGUIInputPortValueEditor::setText(const QString& text)
{
  bool wasKO(!_inp->hasDefaultValueDefined());
  YACSEvalAny *val(BuildAnyFromString(text,_inp));
  _inp->setDefaultValue(val);
  QString text2(BuidStringFromAny(val));
  delete val;
  QString toolTipText(QString("Default = %1").arg(text2));
  if(text.isNull())
    toolTipText="No default set !";
  setToolTip(toolTipText);
  if(text.isEmpty())
    {
      connect(this,SIGNAL(changeLook()),this,SLOT(applyModificationOfLook()));
    }
  else
    {
      if(wasKO)
        {
          emit statusChanged();
          disconnect(this,SIGNAL(changeLook()),this,SLOT(applyModificationOfLook()));
        }
    }
}

void YDFXGUIInputPortValueEditor::paintEvent(QPaintEvent *event)
{
  _zePainter->paintEvent(event);
}

void YDFXGUIInputPortValueEditor::mousePressEvent(QMouseEvent *event)
{
  delete _zePainter;
  _zePainter=new MyWidgetPainterFocus(this);
  updateGeometry();
  update();
}

void YDFXGUIInputPortValueEditor::mouseReleaseEvent(QMouseEvent *event)
{
  QClipboard *cb(QApplication::clipboard());
  if(event->button()==Qt::MidButton && cb->supportsSelection())
    {
      QString text(cb->text(QClipboard::Selection));
      int pos;
      if(_valid->validate(text,pos)==QValidator::Acceptable)
        {
          delete _zePainter;
          _zePainter=new MyWidgetPainterNoFocus(this);
          setText(text);
          update();
        }
    }
}

void YDFXGUIInputPortValueEditor::keyPressEvent(QKeyEvent *event)
{
  int ekey(event->key());
  if(ekey==Qt::Key_Return || ekey==Qt::Key_Enter)
    {
      clearFocus();
      return ;
    }
  if(ekey==Qt::Key_Escape)
    {
      delete _zePainter;
      _zePainter=new MyWidgetPainterNoFocus(this);
      clearFocus();
      return ;
    }
  if(ekey==Qt::Key_Backspace)
    {
      _zePainter->supprLetter();
      return ;
    }
  if((ekey>=Qt::Key_Exclam && ekey<=Qt::Key_Z))
    {
      QString st(event->text());
      _zePainter->appendLetter(st);
    }
}

void YDFXGUIInputPortValueEditor::focusOutEvent(QFocusEvent * event)
{
  MyWidgetPainterFocus *zePainter(dynamic_cast<MyWidgetPainterFocus *>(_zePainter));
  if(zePainter)
    {
      int pos;
      QString zeSt(zePainter->getText());
      delete _zePainter;
      _zePainter=new MyWidgetPainterNoFocus(this);
      if(_valid->validate(zeSt,pos)==QValidator::Acceptable)
        setText(zeSt);
    }
  updateGeometry();
  update();
}

QSize YDFXGUIInputPortValueEditor::sizeHint() const
{
  return _zePainter->sizeHint();
}

QSize YDFXGUIInputPortValueEditor::minimumSizeHint() const
{
  return _zePainter->sizeHint();
}

bool YDFXGUIInputPortValueEditor::isOK() const
{
  return IsOK(_inp);
}

bool YDFXGUIInputPortValueEditor::IsOK(YACSEvalInputPort *inp)
{
  return inp->isRandomVar() || inp->hasDefaultValueDefined();
}

bool YDFXGUIInputPortValueEditor::isRandom() const
{
  return _inp->isRandomVar();
}

bool YDFXGUIInputPortValueEditor::toggleRandom()
{
  bool oldOK(isOK());
  bool oldStatus(_inp->isRandomVar());
  _inp->declareRandomnessStatus(!oldStatus);
  emit statusChanged(); // always emit because this even if newOK and oldOK are the same the upon status can changed !
  updateGeometry();
  update();
  return _inp->isRandomVar();
}

void YDFXGUIInputPortValueEditor::applyModificationOfLook()
{
  _zePainter->changeAngle();
  if(!isOK())
    this->update();
}

//////////////////

HoverLabel::HoverLabel(QWidget *parent):QLabel(parent),_isin(false),_isPressed(false)
{
  //setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

void HoverLabel::mousePressEvent(QMouseEvent *event)
{
  if(event->button()==Qt::LeftButton)
    {
      _isPressed=true;
      pressOccured();
      updateGeometry();
      update();
    }
}

void HoverLabel::mouseReleaseEvent(QMouseEvent *event)
{
  if(event->button()==Qt::LeftButton)
    {
      _isPressed=false;
      updateGeometry();
      update();
    }
}

void HoverLabel::enterEvent(QEvent *event)
{
  _isin=true;
  updateGeometry();
  update();
}

void HoverLabel::leaveEvent(QEvent *event)
{
  _isin=false;
  updateGeometry();
  update();
}

void HoverLabel::paintEvent(QPaintEvent *event)
{
  if(!_isin)
    return paintIfNotOn(event);
  //
  QPainter painter(this);
  QFont ft(painter.font());
  AssignFontOnHover(ft);
  painter.setFont(ft);
  QFontMetrics fm(ft);
  QPalette pal(QApplication::palette("QPushButton"));
  QRect refRect(rect());
  std::vector<ColoredString> sts(textForEmulatedPushButton());
  QRect rect3(painter.boundingRect(refRect,Qt::AlignTop,sts[0].str()));
  int deltaY((refRect.height()-sts.size()*rect3.height())/(sts.size()+1));
  QBrush b0(pal.color(_isPressed?QPalette::Dark:QPalette::Button));
  painter.setBrush(b0);
  painter.setPen(QPen(pal.color(QPalette::Highlight),1,Qt::SolidLine,Qt::SquareCap));
  painter.drawRoundedRect(QRect(0,0,refRect.width()-1,refRect.height()-1),3,3);
  painter.setBrush(QBrush());
  painter.setPen(QPen(pal.color(QPalette::Shadow),1,Qt::SolidLine,Qt::SquareCap));
  painter.drawRoundedRect(QRect(1,1,refRect.width()-3,refRect.height()-3),3,3);
  painter.setPen(QPen(pal.color(QPalette::Dark),1,Qt::SolidLine,Qt::SquareCap));
  painter.drawRoundedRect(QRect(2,2,refRect.width()-5,refRect.height()-5),5,5);
  painter.setPen(QPen(pal.color(QPalette::Midlight),1,Qt::SolidLine,Qt::SquareCap));
  painter.drawRoundedRect(QRect(3,3,refRect.width()-7,refRect.height()-7),3,3);
  painter.setPen(QPen(pal.color(QPalette::Light),1,Qt::SolidLine,Qt::SquareCap));
  painter.drawRoundedRect(QRect(4,4,refRect.width()-9,refRect.height()-9),3,3);
  //QPalette::Button QPalette::ButtonText
  painter.setPen(QPen(pal.color(QPalette::ButtonText),2,Qt::SolidLine,Qt::SquareCap));
  int posY(deltaY);
  foreach(ColoredString st,sts)
    {
      QRect rect2(painter.boundingRect(refRect,Qt::AlignTop,st.str()));
      QPen pen(painter.pen());
      QPen pen2(pen);
      pen2.setColor(st.color());
      painter.setPen(pen2);
      painter.drawText(QPoint((refRect.width()-rect2.width())/2,posY+rect2.height()-fm.descent()),st.str());
      painter.setPen(pen);
      posY+=deltaY+rect2.height();
    }
}

QSize HoverLabel::sizeHint() const
{
  if(!_isin)
    return sizeHintNotHovered();
  else
    {
      QFont ft(font());
      AssignFontOnHover(ft);
      QFontMetrics fm(ft);
      std::vector<ColoredString> sts(textForEmulatedPushButton());
      int h(0),w(0);
      foreach(ColoredString st,sts)
        {
          QSize elt(fm.boundingRect(st.str()).size());
          h+=elt.height();
          w=std::max(w,elt.width());
        }
      QSize ret;
      ret.setHeight(h); ret.setWidth(w);//3*PT_SZ_ON_HOVER
      return ret;
    }
}

void HoverLabel::AssignFontOnHover(QFont& ft)
{
  ft.setBold(true);
  ft.setItalic(true);
  ft.setPointSize(PT_SZ_ON_HOVER);
}

//////////////////

InputLabel::InputLabel(YACSEvalInputPort *inp, QWidget *parent):HoverLabel(parent),_inp(inp)
{
  AssignTextAndTooltip(this,_inp);
}

QSize InputLabel::sizeHintNotHovered() const
{
  return QLabel::sizeHint();
}

void InputLabel::paintIfNotOn(QPaintEvent *event)
{
  QLabel::paintEvent(event);
}

void InputLabel::pressOccured()
{
  emit randomnessStatusChanged();
}

std::vector<ColoredString> InputLabel::textForEmulatedPushButton() const
{
  std::vector<ColoredString> ret;
  if(!_inp->isRandomVar())
    ret.push_back(QString(" Parametrize %1 ").arg(text()));
  else
    ret.push_back(QString(" Unparametrize %1 ").arg(text()));
  return ret;
}

void InputLabel::AssignTextAndTooltip(QLabel *wid, YACSEvalInputPort *port)
{
  QString txt(port->getName().c_str());
  wid->setText(txt);
  wid->setToolTip(QString("%1 (%2)").arg(txt).arg(QString(port->getTypeOfData().c_str())));
}

//////////////////

InputLabelNonToggle::InputLabelNonToggle(YDFXGUIInputPortValueEditor *wid, QWidget *parent):QLabel(parent),_wid(wid)
{
  setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  InputLabel::AssignTextAndTooltip(this,_wid->getPort());
}

//////////////////

OutputLabel::OutputLabel(YACSEvalOutputPort *outp, QWidget *parent):HoverLabel(parent),_outp(outp)
{
  QString txt(_outp->getName().c_str());
  setText(txt);
  setToolTip(txt);
}

bool OutputLabel::isQOfInt() const
{
  return _outp->isQOfInterest();
}

QSize OutputLabel::sizeHintNotHovered() const
{
  if(!isQOfInt())
    return QLabel::sizeHint();
  const QFont& ft(font());
  QFontMetrics fm(ft);
  QString txt(text());
  QPointF pts[7];
  ComputePointsToCircle(txt,rect(),font(),pts);
  QPainterPath path;
  path.moveTo(pts[0]);
  path.cubicTo(pts[1],pts[2],pts[3]);
  path.cubicTo(pts[4],pts[5],pts[6]);
  QSize ret(path.boundingRect().toRect().size());
  int& rh(ret.rheight()),&rw(ret.rwidth());
  rh+=2*PEN_SZ_CIRCLED; rw+=2*PEN_SZ_CIRCLED; 
  return ret;
}

void OutputLabel::paintIfNotOn(QPaintEvent *event)
{
  QLabel::paintEvent(event);
  if(!isQOfInt())
    return ;
  QString txt(text());
  //
  const QFont& ft(font());
  QFontMetrics fm(ft);
  QPainter painter(this);
  //
  QPointF pts[7];
  ComputePointsToCircle(txt,rect(),font(),pts);
  //
  QPainterPath path;
  path.moveTo(pts[0]);
  path.cubicTo(pts[1],pts[2],pts[3]);
  path.cubicTo(pts[4],pts[5],pts[6]);
  QPen pen(painter.pen());
  pen.setColor(Qt::green); pen.setWidth(PEN_SZ_CIRCLED);
  painter.strokePath(path,pen);
}

void OutputLabel::pressOccured()
{
  bool oldStatus(_outp->isQOfInterest());
  _outp->setQOfInterestStatus(!oldStatus);
  emit clicked();
}

std::vector<ColoredString> OutputLabel::textForEmulatedPushButton() const
{
  std::vector<ColoredString> ret;
  if(!isQOfInt())
    {
      ret.push_back(QString("Make"));
      ColoredString elt(QString("%1").arg(_outp->getName().c_str()));
      elt.setColor(Qt::red);
      ret.push_back(elt);
    }
  else
    {
      ret.push_back(QString("No more"));
      ColoredString elt(QString("%1").arg(_outp->getName().c_str()));
      elt.setColor(Qt::red);
      ret.push_back(elt);
      ret.push_back(QString("as"));
    }
  ret.push_back(QString("quantity"));
  ret.push_back(QString("of"));
  ret.push_back(QString("interest"));
  return ret;
}

QSize OutputLabel::minimumSizeHint() const
{
  return sizeHint();
}

void OutputLabel::ComputePointsToCircle(const QString& txt, const QRect& refRect, const QFont& font, QPointF pts[8])
{
  QFontMetrics fm(font);
  QSize rect2(fm.boundingRect(txt).size());
  QPointF refPt(refRect.width()/2,refRect.height()/2+fm.descent());
  for(int i=0;i<7;i++)
    pts[i]=refPt;
  qreal& p0x(pts[0].rx()),&p0y(pts[0].ry()),&p1x(pts[1].rx()),&p1y(pts[1].ry()),&p2x(pts[2].rx()),&p2y(pts[2].ry()),&p3x(pts[3].rx()),&p3y(pts[3].ry());
  p0x+=rect2.width()/2.; p0y-=rect2.height();
  p1x+=rect2.width(); p1y-=1.2*rect2.height()/2.;
  p2x+=rect2.width(); p2y+=1.2*rect2.height();
  p3y+=1.2*rect2.height();
  qreal &p4x(pts[4].rx()),&p4y(pts[4].ry()),&p5x(pts[5].rx()),&p5y(pts[5].ry()),&p6x(pts[6].rx()),&p6y(pts[6].ry());
  p4x-=1.1*rect2.width(); p4y+=1.2*rect2.height();
  p5x-=rect2.width(); p5y-=1.3*rect2.height()/2.;
  p6x+=rect2.width()/2.; p6y-=1.4*rect2.height();
}

/////////////

void YDFXGUIOKCls::initOK()
{
  _isOK=isOK();
}

//////////////////

void YDFXGUIGatherPorts::somebodyChangedStatus()
{
  bool newStatus(isOK());
  if(newStatus!=wasOK())
    {
      updateGeometry();
      update();
      setWasOKStatus(newStatus);
    }
  emit theGlobalStatusChanged(newStatus);// emit signal always because of input ports and sequences definitions.
}

//////////////////

YDFXGUIInputPortsSelector::YDFXGUIInputPortsSelector(YACSEvalYFXWrap *efx, QWidget *parent):YDFXGUIGatherPorts(parent),_timerId(-1)
{
  _inps=efx->getFreeInputPorts();
  fillWidget();
  initOK();
}

bool YDFXGUIInputPortsSelector::isOK() const
{
  const QObjectList &children(this->children());
  int nbOfRandom(0);
  foreach(QObject *child,children)
    {
      YDFXGUIInputPortValueEditor *childc(qobject_cast<YDFXGUIInputPortValueEditor *>(child));
      if(childc)
        {
          if(!childc->isOK())
            return false;
          if(childc->isRandom())
            nbOfRandom++;
        }
    }
  return nbOfRandom>=1;
}

bool YDFXGUIInputPortsSelector::areSeqWellDefined() const
{
  const QObjectList &children(this->children());
  int nbOfRandom(0),refSz(std::numeric_limits<int>::max());
  foreach(QObject *child,children)
    {
      YDFXGUIInputPortValueEditor *childc(qobject_cast<YDFXGUIInputPortValueEditor *>(child));
      if(!childc)
        continue;
      if(!childc->isOK())
        return false;
      if(!childc->isRandom())
        continue;
      YACSEvalInputPort *port(childc->getPort());
      YACSEvalSeqAny *seq(port->getSequenceOfValuesToEval());
      if(!seq)
        return false;
      if(nbOfRandom==0)
        refSz=seq->size();
      if(refSz!=seq->size())
        return false;
      nbOfRandom++;
    }
  return nbOfRandom>=1;
}

void YDFXGUIInputPortsSelector::fillWidget()
{
  QVBoxLayout *mainLayout(new QVBoxLayout);
  for(std::vector< YACSEvalInputPort * >::const_iterator it=_inps.begin();it!=_inps.end();it++)
    {
      QHBoxLayout *lineLayout(new QHBoxLayout);
      YDFXGUIInputPortValueEditor *elt1(new YDFXGUIInputPortValueEditor(*it));
      QLabel *elt0(0);
      if((*it)->getTypeOfData()==YACSEvalAnyDouble::TYPE_REPR)
        {
          InputLabel *elt0c(new InputLabel(*it,this));
          elt0=elt0c;
          connect(elt0c,SIGNAL(randomnessStatusChanged()),elt1,SLOT(toggleRandom()));
        }
      else
        elt0=new InputLabelNonToggle(elt1,this);
      lineLayout->addWidget(elt0);
      connect(elt1,SIGNAL(statusChanged()),this,SLOT(somebodyChangedStatus()));
      lineLayout->addWidget(elt1);
      mainLayout->addLayout(lineLayout);
    }
  this->setLayout(mainLayout);
}

YDFXGUIInputPortsSelector::~YDFXGUIInputPortsSelector()
{
  if(_timerId!=-1)
    killTimer(_timerId);
}

void YDFXGUIInputPortsSelector::showEvent(QShowEvent *e)
{
  _timerId=startTimer(100);
}

void YDFXGUIInputPortsSelector::timerEvent(QTimerEvent *e)
{
  if(e->timerId()==_timerId)
    {
      const QObjectList &children(this->children());
      foreach(QObject *child,children)
        {
          YDFXGUIInputPortValueEditor *childc(qobject_cast<YDFXGUIInputPortValueEditor *>(child));
          if(childc)
            {
              emit childc->changeLook();
            }
        }
    }
}

void YDFXGUIInputPortsSelector::DrawWarningSign(QPainter& painter, int width0, int height0)
{
  const int SZP(12);
  static const int WARN_Y=176,WARN_X=200;
  const float RATIO(float(WARN_X)/float(WARN_Y));
  //QPen(QColor(255,203,189)
  if(float(width0)>RATIO*float(height0))
    painter.setViewport(int(width0-RATIO*float(height0))/2,0,height0*RATIO,height0);
  else
    painter.setViewport(0,(float(height0)-width0/RATIO)/2,width0,width0/RATIO);//width-height/RATIO
  painter.setRenderHint(QPainter::Antialiasing,true);
  painter.setWindow(0,0,WARN_X,WARN_Y);
  //
  painter.setPen(QPen(QColor(255,203,189),SZP,Qt::SolidLine,Qt::RoundCap));
  painter.drawLine(QPoint(100,13),QPoint(11,164));
  painter.drawLine(QPoint(11,164),QPoint(185,164));
  painter.drawLine(QPoint(185,164),QPoint(100,13));
  QColor lightBlack(200,200,200);
  painter.setBrush(QBrush(lightBlack));
  painter.setPen(QPen(lightBlack,2,Qt::SolidLine,Qt::RoundCap));
  painter.drawEllipse(87,47,24,24);
  painter.drawEllipse(93,105,12,12);
  painter.drawEllipse(90,129,18,18);
  const QPoint points[4]={QPoint(87,59),QPoint(93,111),QPoint(105,111),QPoint(111,59)};
  painter.drawPolygon(points,4);
}

void YDFXGUIInputPortsSelector::paintEvent(QPaintEvent *e)
{
  QPainter painter(this);
  const int SZP(12);
  int width0(width()),height0(height());
  //
  QRect refRect(0,0,width0,height0);
  painter.eraseRect(refRect);
  if(!isOK())
    DrawWarningSign(painter,width0,height0);
}

/////////////

YDFXGUIOutputPortsSelector::YDFXGUIOutputPortsSelector(YACSEvalYFXWrap *efx, QWidget *parent):YDFXGUIGatherPorts(parent)
{
  _outps=efx->getFreeOutputPorts();
  fillWidget();
  initOK();
}

void YDFXGUIOutputPortsSelector::fillWidget()
{
  QVBoxLayout *mainLayout(new QVBoxLayout);
  for(std::vector< YACSEvalOutputPort * >::const_iterator it=_outps.begin();it!=_outps.end();it++)
    {
      if((*it)->getTypeOfData()==YACSEvalAnyDouble::TYPE_REPR)
        {
          OutputLabel *elt1(new OutputLabel(*it,this));
          mainLayout->addWidget(elt1);
          connect(elt1,SIGNAL(clicked()),this,SLOT(somebodyChangedStatus()));
        }
    }
  this->setLayout(mainLayout);
}

void YDFXGUIOutputPortsSelector::paintEvent(QPaintEvent *e)
{
  if(isOK())
    {
      QWidget::paintEvent(e);
      return ;
    }
  QPainter painter(this);
  YDFXGUIInputPortsSelector::DrawWarningSign(painter,width(),height());
}

bool YDFXGUIOutputPortsSelector::isOK() const
{
  int nbOfOutputsQOI(0);
  foreach(QObject *child,children())
    {
      OutputLabel *childc(qobject_cast<OutputLabel *>(child));
      if(!childc)
        continue;
      if(childc->isQOfInt())
        nbOfOutputsQOI++;
    }
  return nbOfOutputsQOI>=1;
}

/////////////

QScrollArea *YDFXGUIAbstractPorts::setupWidgets(const QString& title, QWidget *zeWidget)
{
  QHBoxLayout *mainLayout(new QHBoxLayout(this));
  QGroupBox *gb(new QGroupBox(title,this));
  QVBoxLayout *vbox(new QVBoxLayout);
  QVBoxLayout *vbox2(new QVBoxLayout);
  QFrame *frame(new QFrame(this));
  frame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  vbox->addWidget(frame);
  gb->setLayout(vbox);
  QScrollArea *sa(new QScrollArea(this));
  sa->setWidgetResizable(true);
  sa->setWidget(zeWidget);
  vbox2->addWidget(sa);
  frame->setLayout(vbox2);
  mainLayout->addWidget(gb);
  this->setLayout(mainLayout);
  return sa;
}

/////////////

YDFXGUIInputPorts::YDFXGUIInputPorts(YACSEvalYFXWrap *efx, QWidget *parent):YDFXGUIAbstractPorts(efx,parent),_inputsSelector(new YDFXGUIInputPortsSelector(efx,this))
{
  QScrollArea *sa(setupWidgets("Inputs",_inputsSelector));
  connect(efx,SIGNAL(lockSignal(bool)),_inputsSelector,SLOT(setDisabled(bool)));
  sa->setMinimumWidth(320);
  sa->setMinimumHeight(360);
}

/////////////

YDFXGUIOutputPorts::YDFXGUIOutputPorts(YACSEvalYFXWrap *efx, QWidget *parent):YDFXGUIAbstractPorts(efx,parent),_outputsSelector(new YDFXGUIOutputPortsSelector(efx,this))
{
  QScrollArea *sa(setupWidgets("Outputs",_outputsSelector));
  connect(efx,SIGNAL(lockSignal(bool)),_outputsSelector,SLOT(setDisabled(bool)));
  sa->setMinimumWidth(100);
}
/////////////

YDFXGUIAllPorts::YDFXGUIAllPorts(YACSEvalYFXWrap *efx, QWidget *parent):QWidget(parent),_in(new YDFXGUIInputPorts(efx,this)),_out(new YDFXGUIOutputPorts(efx,this))
{
  QVBoxLayout *mainLayout(new QVBoxLayout(this));
  QSplitter *sp(new QSplitter(Qt::Horizontal,this));
  sp->addWidget(_in); sp->setStretchFactor(0,3);
  sp->addWidget(_out); sp->setStretchFactor(1,1);
  mainLayout->addWidget(sp);
  connect(_in->getPortsManager(),SIGNAL(theGlobalStatusChanged(bool)),this,SLOT(somethingChangedInPorts(bool)));
  connect(_out->getPortsManager(),SIGNAL(theGlobalStatusChanged(bool)),this,SLOT(somethingChangedInPorts(bool)));
}

bool YDFXGUIAllPorts::isOK() const
{
  return _in->getPortsManager()->isOK() && _out->getPortsManager()->isOK();
}

void YDFXGUIAllPorts::somethingChangedInPorts(bool status)
{
  bool step1(_in->isOK() && _out->isOK());
  if(!step1)
    {
      emit sequencesCanBeDefinedSignal(false);
      emit canBeExecutedSignal(false);
      return ;
    }
  emit sequencesCanBeDefinedSignal(true);
  emit canBeExecutedSignal(_in->areSeqWellDefined());
}
