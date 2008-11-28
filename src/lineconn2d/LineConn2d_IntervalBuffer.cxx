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
// File:      LineConn2d_IntervalBuffer.cpp
// Created:   21.02.05 15:54:12
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005
//
#include <LineConn2dAfx.h>

#include <LineConn2d.h>
#include <LineConn2d_IntervalBuffer.h>
#include <Precision.hxx>

#include <stdio.h>

//=======================================================================
//function : Add
//purpose  : adding one interval to the buffer
//=======================================================================

void LineConn2d_IntervalBuffer::Add (const LineConn2d_ZInterval& theInterval)
{
  LineConn2d_ZInterval * anInt[2] = {0L, 0L};  // the inserted and the previous

  // Scan the collection and find the available interval that is greater than
  // the given one, then insert the given one before the found interval.
  NCollection_List<LineConn2d_ZInterval>::Iterator anIter (myIntervals);
  for (; anIter.More(); anIter.Next()) {
    const Standard_Real anXint = anIter.Value().myX[1];
    if (theInterval.myX[1] < anXint) {
      anInt[0] = &myIntervals.InsertBefore (theInterval, anIter);
      break;
    }
    anInt[1] = &anIter.ChangeValue();
  }
  // The given interval is to be appended as the last one in the collection.
  if (anInt[0] == 0L) {
    if (myIntervals.IsEmpty() == Standard_False)
      anInt[1] = (LineConn2d_ZInterval *) &myIntervals.Last();
    anInt[0] = &myIntervals.Append (theInterval);
  }
  // Now merge the added interval with all relevant ones already available.
  if (anInt[1])
    Cut (anInt);             // merge with the previous
  for (; anIter.More() && anInt[0]; anIter.Next()) {
    anInt[1] = &anIter.ChangeValue();
    anInt[0] = Cut (anInt);             // merge with the next
  }
}

//=======================================================================
//function : Cut
//purpose  : merge two intervals
//=======================================================================

LineConn2d_ZInterval * LineConn2d_IntervalBuffer::Cut
                                        (LineConn2d_ZInterval * theInt[2])
{
  const Standard_Real aTol = Precision::Confusion();
  LineConn2d_ZInterval * aResult = 0L;
  // test for intersection
  if (theInt[0]->myX[1] - theInt[1]->myX[0] > aTol &&
      theInt[1]->myX[1] - theInt[0]->myX[0] > aTol)
  {
    Standard_Integer iLow = (theInt[0]->Z() < theInt[1]->Z()) ? 0 : 1;
    LineConn2d_ZInterval& anIntLow = * theInt[iLow];
    LineConn2d_ZInterval& anIntUp  = * theInt[1-iLow];
    // there is intersection, now find the intersection interval
    Standard_Integer aTest = 0;
    if (anIntUp.myX[0] > anIntLow.myX[0] + Precision::Confusion())
      aTest = 0x2;
    if (anIntUp.myX[1] > anIntLow.myX[1] - Precision::Confusion())
      aTest |= 0x1;
// Now the value aTest is this:
//       00      IntUp      |----------|
//               IntLow          |-----------|
//
//       11      IntUp           |----------|
//               IntLow     |-----------|
//
//       10      IntUp           |---|
//               IntLow     |--------------|
//
//       01      IntUp      |--------------|
//               IntLow          |---|

    switch (aTest) {
    case 0x0:
      anIntLow.myX[0] = anIntUp.myX[1];
      if (iLow == 0)
        aResult = theInt[0];
      break;
    case 0x1: {
      NCollection_List<LineConn2d_ZInterval>::Iterator anIter (myIntervals);
      for (; anIter.More(); anIter.Next())
        if (anIntLow == anIter.Value()) {
          myIntervals.Remove (anIter);
          break;
        }
      if (iLow == 1)
        aResult = theInt[0];
      break;
    }
    case 0x2: {
      LineConn2d_ZInterval aNewInterval (anIntLow.Z(),
                                     anIntLow.myX[0], anIntUp.myX[0]);
      anIntLow.myX[0] = anIntUp.myX[1];
      aNewInterval.SetAttribute (anIntLow.Attribute());
      Add (aNewInterval);
      break;
    }
    case 0x3:
      anIntLow.myX[1] = anIntUp.myX[0];
      if (iLow == 1)
        aResult = theInt[0];
      break;
    }
  }
  return aResult;
}

//=======================================================================
//function : GetPolyline
//purpose  : 
//=======================================================================

void LineConn2d_IntervalBuffer::GetPolyline (LineConn2d_ListOfXY& outLine,
                                             const Standard_Real theLim[2])const
{
  outLine.Clear();
  NCollection_List<LineConn2d_ZInterval>::Iterator anIter (myIntervals);
  for (; anIter.More(); anIter.Next()) {
    const LineConn2d_ZInterval& anInterval = anIter.Value();
    //const Standard_Real aZ = anInterval.Z();
    Standard_Real anX[2] = { anInterval.X(0), anInterval.X(1) };
    if (anX[0] < theLim[1] - Precision::Confusion() &&
        anX[1] > theLim[0] + Precision::Confusion())
    {
      if (anX[0] < theLim[0]) anX[0] = theLim[0]; 
      if (anX[1] > theLim[1]) anX[1] = theLim[1]; 
      outLine.Append (gp_XY (anX[0], anInterval.Z()));
      outLine.Append (gp_XY (anX[1], anInterval.Z()));
    }
  }
}

//=======================================================================
//function : GetMinima
//purpose  : 
//=======================================================================

void LineConn2d_IntervalBuffer::GetMinima   (LineConn2d_ListOfReal& outList,
                                             const Standard_Real  theTol) const
{
  outList.Clear();
  if (myIntervals.Extent() < 2)
    return;
  //const Standard_Real aConf = Precision::Confusion();
  NCollection_List<LineConn2d_ZInterval>::Iterator anIter (myIntervals);
  Standard_Real aZ[3] = {0., 0., 1.}; // the history of Z along the buffer
  Standard_Integer aCount (0);
  const LineConn2d_ZInterval * aPrevInt = 0L;
  for (; anIter.More(); anIter.Next()) {
    const LineConn2d_ZInterval& anInterval = anIter.Value();
    if (anInterval.X(1) - anInterval.X(0) < theTol)
      continue;
    if (aCount++ == 0) {
      // Initialize the history buffer
      aZ[1] = ::RealLast();
      aZ[2] = anInterval.Z();
      aPrevInt = &anInterval;
      continue;
    } else {
      aZ[0] = aZ[1];
      aZ[1] = aZ[2];
      aZ[2] = anInterval.Z();
    }
    if (aZ[1] < aZ[0] && aZ[1] < aZ[2])
      outList.Append ((aPrevInt->X(0) + aPrevInt->X(1)) * 0.5);
    aPrevInt = &anInterval;
  }
  if (aZ[2] < aZ[1])
    outList.Append ((aPrevInt->X(0) + aPrevInt->X(1)) * 0.5);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void LineConn2d_IntervalBuffer::Dump (Standard_OStream& theStream,
                                      const Standard_Real theTol) const
{
  theStream << "  === Dump of buffer ( " << myIntervals.Extent()
            << " intervals)" << endl;
  NCollection_List<LineConn2d_ZInterval>::Iterator anIter (myIntervals);
  for (; anIter.More(); anIter.Next()) {
    const LineConn2d_ZInterval& anInt = anIter.Value();
    char buf[120];
    sprintf (buf, "(%8.4f, %8.4f) Z = %.8g",
             anInt.X(0), anInt.X(1), anInt.Z());
    theStream << buf << endl;
  }
  LineConn2d_ListOfReal lst;
  GetMinima (lst, theTol);
  LineConn2d_ListOfReal::Iterator anIter1 (lst);
  if (anIter1.More()) {
    theStream <<  "Minima:";
    for (; anIter1.More(); anIter1.Next())
      theStream << ' ' << anIter1.Value();
    theStream << endl;
  }
}

//=======================================================================
//function : CheckBuffer
//purpose  : 
//=======================================================================

void LineConn2d_IntervalBuffer::CheckBuffer () {
  // test the consistency of the buffer
  LineConn2d_ZInterval * anInt[2] = {0L, 0L};  // the inserted and the previous
  for (Standard_Boolean isKO = Standard_True; isKO;) {
    isKO = Standard_False;
    NCollection_List<LineConn2d_ZInterval>::Iterator anIter (myIntervals);
    anInt[1] = (LineConn2d_ZInterval *) &anIter.Value();
    for (anIter.Next(); anIter.More(); anIter.Next()) {
      if (anIter.Value().myX[1] < anInt[1]->myX[0] + Precision::Confusion()) {
        LineConn2d_ZInterval aTmp = anIter.Value();
        myIntervals.Remove (anIter);
        Add (aTmp);
        isKO = Standard_True;
        break;
      }
      if (anInt[1]->myX[1] > anIter.Value().myX[0] + Precision::Confusion()) {
        anInt[0] = (LineConn2d_ZInterval *) &anIter.Value();
        Cut (anInt);
        isKO = Standard_True;
        break;
      }
      anInt[1] = (LineConn2d_ZInterval *) &anIter.Value();
    }
  }
}
