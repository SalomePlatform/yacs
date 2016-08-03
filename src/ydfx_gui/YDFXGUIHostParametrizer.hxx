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

#ifndef __YDFXGUIHOSTPARAMETRIZER_HXX__
#define __YDFXGUIHOSTPARAMETRIZER_HXX__

#include "ydfxwidgetsExport.hxx"

#include <QWidget>
#include <QValidator>

class QComboBox;
class QLineEdit;
class QSettings;
class QDialogButtonBox;

class YACSEvalYFXWrap;
class YDFXGUIBatchInfo;

class YDFXGUIHostParametrizer : public QWidget
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUIHostParametrizer(QWidget *parent, YACSEvalYFXWrap *efx);
  YDFXWIDGETS_EXPORT QDialogButtonBox *getDialogButtonBox() { return _buttonBox; }
  YDFXWIDGETS_EXPORT void loadFromSettings(const QSettings& settings);
  YDFXWIDGETS_EXPORT void learnSettings(QSettings& settings) const;
  YDFXWIDGETS_EXPORT void applyToEFX();
  YDFXWIDGETS_EXPORT QString getNameOfHost();
public slots:
  YDFXWIDGETS_EXPORT void changeMachine(const QString& newMachineSelected);
  YDFXWIDGETS_EXPORT void clusterAdvParamStatusChanged(bool newStatus);
signals:
  void readyForRunSignal(bool);
  void interactivityChanged(bool);
public:
  YDFXWIDGETS_EXPORT static const char MACHINE[];
private:
  QComboBox *_hostSelector;
  YDFXGUIBatchInfo *_clusterAdvInfo;
  QDialogButtonBox *_buttonBox;
  YACSEvalYFXWrap *_efx;
  bool _wasInteractive;
};

class YDFXGUIDurationValidator : public QValidator
{
public:
  YDFXWIDGETS_EXPORT YDFXGUIDurationValidator(QObject *parent):QValidator(parent) { }
  YDFXWIDGETS_EXPORT QValidator::State validate(QString & input, int & pos) const;
};

class YACSEvalParamsForCluster;

class YDFXGUIBatchInfo : public QWidget
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUIBatchInfo(QWidget *parent, YACSEvalYFXWrap *efx);
  YDFXWIDGETS_EXPORT void showEvent(QShowEvent *event);
  YDFXWIDGETS_EXPORT bool isOK() const;
  YDFXWIDGETS_EXPORT void loadFromSettings(const QSettings& settings);
  YDFXWIDGETS_EXPORT void learnSettings(QSettings& settings) const;
  YDFXWIDGETS_EXPORT void applyToParams(YACSEvalParamsForCluster& ps) const;
  YDFXWIDGETS_EXPORT QString getNameOfHost();
public slots:
  YDFXWIDGETS_EXPORT void somethingChanged();
public:
  YDFXWIDGETS_EXPORT static const char NBPROCS[];
  YDFXWIDGETS_EXPORT static const char REMOTEDIR[];
  YDFXWIDGETS_EXPORT static const char LOCALDIR[];
  YDFXWIDGETS_EXPORT static const char WCKEY[];
  YDFXWIDGETS_EXPORT static const char MAXDUR[];
signals:
  void statusOfEntryChanged(bool newStatus);
private:
  static QString NormalizeDuration(const QString& txt);
private:
  YACSEvalYFXWrap *_efx;
  QLineEdit *_nbProcs;
  QLineEdit *_remoteDir;
  QLineEdit *_localDir;
  QLineEdit *_WCKey;
  QLineEdit *_maxDuration;
  bool _wasOK;
};

#endif
