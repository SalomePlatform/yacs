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

#ifndef __YDFXGUIWRAP_HXX__
#define __YDFXGUIWRAP_HXX__

#include "ydfxwidgetsExport.hxx"

#include <QObject>

class YACSEvalYFX;
class YACSEvalSession;
class YACSEvalObserver;
class YACSEvalInputPort;
class YACSEvalExecParams;
class YACSEvalOutputPort;
class YACSEvalListOfResources;

class YACSEvalYFXWrap : public QObject
{
  Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YACSEvalYFXWrap(YACSEvalYFX *efx):_efx(efx),_isSeqOfValsSet(false),_isRunning(false) { }
  YDFXWIDGETS_EXPORT YACSEvalYFXWrap(const YACSEvalYFXWrap& other):_efx(other._efx) { }
  YDFXWIDGETS_EXPORT std::vector< YACSEvalInputPort * > getFreeInputPorts() const;
  YDFXWIDGETS_EXPORT std::vector< YACSEvalOutputPort * > getFreeOutputPorts() const;
  YDFXWIDGETS_EXPORT YACSEvalExecParams *getParams() const;
  YDFXWIDGETS_EXPORT bool isLocked() const;
  YDFXWIDGETS_EXPORT void unlockAll();
  YDFXWIDGETS_EXPORT YACSEvalListOfResources *giveResources();
  YDFXWIDGETS_EXPORT bool run(YACSEvalSession *session, int& nbOfBranches);
  YDFXWIDGETS_EXPORT void registerObserver(YACSEvalObserver *observer);
  YDFXWIDGETS_EXPORT ~YACSEvalYFXWrap();
public:// modified API
  YDFXWIDGETS_EXPORT void lockPortsForEvaluation();
  YDFXWIDGETS_EXPORT int getNbOfItems() const;
public:
  YDFXWIDGETS_EXPORT void setRunningStatus(bool status);
  YDFXWIDGETS_EXPORT bool isRunning() const { return _isRunning; }
  YDFXWIDGETS_EXPORT void updateSequencesStatus();
  YDFXWIDGETS_EXPORT bool computeSequencesStatus(int& nbOfVals);
signals:
  void lockSignal(bool);
  void sequencesAreSetSignal(bool);
  void runSignal(bool);
private:
  YACSEvalYFX *_efx;
  bool _isSeqOfValsSet;
  bool _isRunning;
};

#endif
