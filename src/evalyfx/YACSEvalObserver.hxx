// Copyright (C) 2012-2016  CEA/DEN, EDF R&D
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

#ifndef __YACSEVALOBSERVER_HXX__
#define __YACSEVALOBSERVER_HXX__

#include "YACSEvalYFXExport.hxx"

#include <string>

class YACSEvalYFX;

class YACSEvalObserver
{
public:
  YACSEVALYFX_EXPORT YACSEvalObserver():_cnt(1) { }
  YACSEVALYFX_EXPORT YACSEvalObserver(const YACSEvalObserver& other):_cnt(1) { }// counter must be equal to 1 even in case of copy
  YACSEVALYFX_EXPORT void incrRef() const;
  YACSEVALYFX_EXPORT bool decrRef() const;
  YACSEVALYFX_EXPORT int getCnt() const { return _cnt; }
  YACSEVALYFX_EXPORT virtual void startComputation(YACSEvalYFX *sender) = 0;
  YACSEVALYFX_EXPORT virtual void notifySampleOK(YACSEvalYFX *sender, int sampleId) = 0;
  YACSEVALYFX_EXPORT virtual void notifySampleKO(YACSEvalYFX *sender, int sampleId) = 0;
  //YACSEVALYFX_EXPORT virtual void warningHappen(const std::string& warnDetails) = 0;
  //YACSEVALYFX_EXPORT virtual void errorHappen(const std::string& errorDetails) = 0;
protected:
  virtual ~YACSEvalObserver() { }
private:
  mutable int _cnt;
};

#endif
