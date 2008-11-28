//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File:      LineConn2d.cpp
// Created:   04.08.05 22:28:20
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005
//
#include <LineConn2dAfx.h>

#include <LineConn2d.h>
#include <LineConn2d_SegmentIterator.h>

//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

LineConn2d::Orient LineConn2d::Orientation (const gp_XY& theDir)
{
  const Standard_Real aConf = Precision::Confusion();
  Orient aResult = Indefinite;
  if (IsSmall (theDir.Y())) {
    if (theDir.X() > aConf)
      aResult = E;
    else if (theDir.X() < - aConf)
      aResult = W;
  } else if (IsSmall (theDir.X())) {
    if (theDir.Y() > aConf)
      aResult = N;
    else if (theDir.Y() < - aConf)
      aResult = S;
  }
  return aResult;
}

//=======================================================================
//function : IsIntersect
//purpose  : Detect intersection between two Objects
//=======================================================================

Standard_Boolean LineConn2d::IsIntersect (const LineConn2d_Object& anObj1,
                                          const LineConn2d_Object& anObj2)
{
  Standard_Boolean aResult (Standard_False);
  ((LineConn2d_Object&)anObj1).Update();
  ((LineConn2d_Object&)anObj2).Update();
  if (anObj1.Box().IsOut (anObj2.Box()) == Standard_False) {
    LineConn2d_SegmentIterator anIter1 (anObj1);
    for (; anIter1.More(); anIter1.Next()) {
      const LineConn2d_Segment& aSeg1 = anIter1.Value();
      LineConn2d_SegmentIterator anIter2 (anObj2);
      for (; anIter2.More(); anIter2.Next()) {
        const LineConn2d_Segment& aSeg2 = anIter2.Value();
        Standard_Real anIntAlpha[2];
        if (aSeg1.IsIntersect (aSeg2, anIntAlpha)) {
          aResult = Standard_True;
          break;
        }
      }
      if (anIter2.More())
        break;
    }
  }
  return aResult;
}


