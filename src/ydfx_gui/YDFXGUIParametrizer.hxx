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

#ifndef __YDFXGUIPARAMETRIZER_HXX__
#define __YDFXGUIPARAMETRIZER_HXX__

#include "ydfxwidgetsExport.hxx"

#include <QWidget>

class YACSEvalYFX;

class YDFXGUIParametrizer : public QWidget
{
Q_OBJECT
public:
  YDFXWIDGETS_EXPORT YDFXGUIParametrizer(QWidget *parent);
  YDFXWIDGETS_EXPORT YDFXGUIParametrizer(QWidget *parent, YACSEvalYFX *yfx);
  YDFXWIDGETS_EXPORT bool isReadyForEvaluation() const;
  YDFXWIDGETS_EXPORT YACSEvalYFX *getPreparedYFXInstance() const;
private:
  YACSEvalYFX *_yfx;
};

#endif
