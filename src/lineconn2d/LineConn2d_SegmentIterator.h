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
// File:      LineConn2d_SegmentIterator.h
// Created:   04.08.05 22:47:27
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005
//
#ifndef LineConn2d_SegmentIterator_HeaderFile
#define LineConn2d_SegmentIterator_HeaderFile

#include <LineConn2d_Segment.h>
#include <LineConn2d_Object.h>

/**
 * Iterator that is invoked on LineConn2d_Object and returns contour
 * segments starting from the one between the last point and the first point
 * of the contour.
 */

class LineConn2d_SegmentIterator
{
 public:
  // ---------- PUBLIC METHODS ----------

  /// Empty constructor
  inline LineConn2d_SegmentIterator ()
    : myIsSeg (Standard_False) {}

  /// Constructor
  inline LineConn2d_SegmentIterator (const LineConn2d_Object& theObject)
    : myListIter (theObject.PointsIterator()),
      myIsSeg    (Standard_False)
  {
    if (theObject.NbPoints() > 1) {
      mySeg.SetOrigin    (theObject.LastPoint());
      mySeg.SetExtremity (myListIter.Value());
      myIsSeg = Standard_True;
    }
  }

  /// Method More()
  inline Standard_Boolean More() const
  { return myIsSeg; }

  /// Iterator increment
  inline void Next ()
  {
    if (myIsSeg) {
      myListIter.Next();
      if (myListIter.More()) {
        mySeg.SetOrigin    (mySeg.Extremity());
        mySeg.SetExtremity (myListIter.Value());
      } else
        myIsSeg = Standard_False;
    }
  }

  /// Query the currently iterated segment
  inline const LineConn2d_Segment& Value () const
  { return mySeg; }

 private:
  // ---------- PRIVATE FIELDS ----------

  NCollection_List <gp_XY>::Iterator    myListIter;
  LineConn2d_Segment                    mySeg;
  Standard_Boolean                      myIsSeg;
};

#endif
