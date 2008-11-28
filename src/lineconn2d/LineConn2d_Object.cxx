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
// File:      LineConn2d_Object.cpp
// Created:   03.08.05 21:28:05
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005
//
#include <LineConn2dAfx.h>

#include <LineConn2d_Object.h>
#include <LineConn2d.h>
#include <Precision.hxx>

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void LineConn2d_Object::Update ()
{
  // Create the bounding box of the polyline
  if (myIsModified) {
    myBox.Clear();
    PointIterator anIter (myPoints);
    for (; anIter.More(); anIter.Next()) {
      myBox.Add (anIter.Value());
    }
    myIsModified = 0;
  }
}

//=======================================================================
//function : IsOut
//purpose  : 
//=======================================================================

Standard_Boolean LineConn2d_Object::IsOut (const gp_XY&         thePoint,
                                           const Standard_Real  theTol) const
{
  const Standard_Real aConfusion   = Precision::Confusion();
  const Standard_Real aUniConf     = 1. + aConfusion;
  const Standard_Real aSqConfusion = aConfusion * aConfusion;
  Standard_Real anYprev (1e+100);
  const gp_XY * aPnt[2] = { 0L, 0L };

  Standard_Integer aNIntersect (0);
  PointIterator anIter (myPoints);
  if (anIter.More()) {
    aPnt[1] = &myPoints.Last();
    for (; anIter.More(); anIter.Next()) {
      aPnt[0] = aPnt[1];
      aPnt[1] = &anIter.Value();
      const gp_XY& aPntLow = * (aPnt[0]->X() < aPnt[1]->X() ? aPnt[0]: aPnt[1]);
      const gp_XY& aPntUp  = * (aPnt[0]->X() < aPnt[1]->X() ? aPnt[1]: aPnt[0]);
      const gp_XY aVec (aPntUp -  aPntLow);

      // Check the distance (condition ON)
      Standard_Real aDist (-1.);
      const Standard_Real aSegLen = aVec.Modulus();
      if (aSegLen < aConfusion)
        // Degenerated case
        aDist = (thePoint - (aPntLow + aPntUp) * 0.5).Modulus();
      else {
        // General case
        const gp_XY aVecN = aVec / aSegLen;

        // Obtain the coordinate of the projection on the segment 
        const gp_XY aVecP = thePoint - aPntLow;
        const Standard_Real aCoord = aVecN * aVecP;

        // Check if it projects before the 1st point => distance to 1st point
        if (aCoord < aConfusion)
          aDist = aVecP.Modulus();
        // Check if it projects before the 2nd point => distance to 2nd point
        else if (aCoord > aSegLen - aConfusion)
          aDist = (thePoint - aPntUp).Modulus();
        // Otherwise retrieve the projection point inside the segment
        else
          aDist = LineConn2d_FABS (aVecN ^ aVecP);
      }
      if (aDist < theTol)
        // ON detected, we stop the analysis
        return Standard_False;

      // Rejection test
      if (thePoint.X() > aPntUp.X() + aConfusion || thePoint.X() < aPntLow.X())
          continue;

      // Treatment of a vertical contour segment
      if (aVec.X() <  gp::Resolution() &&
          aVec.X() > -gp::Resolution())
      {
        if (LineConn2d::IsSmall (anYprev - aPntLow.Y()))
          anYprev = aPntUp.Y();
        else if (LineConn2d::IsSmall (anYprev - aPntUp.Y()))
          anYprev = aPntLow.Y();
        continue;
      }

      // Evaluate the intersection
      const Standard_Real anAlpha = (thePoint.X() - aPntLow.X()) / aVec.X();
      if (anAlpha < aUniConf) {
        const Standard_Real anYint = aPntLow.Y() + anAlpha * aVec.Y();
        if (anYint > thePoint.Y()) {
          const Standard_Real aD2 = anYint - anYprev;
          if (aD2 * aD2 > aSqConfusion) {
            ++aNIntersect;
            anYprev = anYint;
          }
        }
      }
    }
  }
  return ((aNIntersect & 0x01) == 0);
}
