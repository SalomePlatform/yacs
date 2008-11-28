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
// File:      LineConn2d_IntervalBuffer.hxx
// Created:   21.02.05 14:38:12
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005
//
#ifndef LineConn2d_IntervalBuffer_HeaderFile
#define LineConn2d_IntervalBuffer_HeaderFile

#include <LineConn2d_ZInterval.h>
#include <NCollection_List.hxx>
#include <gp_XY.hxx>

typedef NCollection_List <gp_XY>         LineConn2d_ListOfXY;
typedef NCollection_List <Standard_Real> LineConn2d_ListOfReal;

/**
 *  Buffer to store an outline of horizontal segments (intervals) in XOZ, each
 *  inteval defining its Z coordinate and extremities X(0) and X(1). The result
 *  can be obtained as sequence of points to create a polyline.
 */

class LineConn2d_IntervalBuffer 
{
 public:
  // ---------- PUBLIC METHODS ----------

  /// Empty constructor
  LineConn2d_IntervalBuffer        (const Handle(NCollection_BaseAllocator)& A)
     : myIntervals (A) {}

  /// Query if the buffer contains 1 or more intervals.
  inline Standard_Boolean IsEmpty  () const
  { return myIntervals.IsEmpty(); }

  /// Add an interval
  Standard_EXPORT void Add         (const LineConn2d_ZInterval& theInterval);

  /// Extract polyline from the buffer, between X coordinates
  /// theLim[0] and theLim[1]
  Standard_EXPORT void GetPolyline (LineConn2d_ListOfXY&    outLine,
                                    const Standard_Real     theLim[2]) const;

  /// Extract the list of local minima of the outline, with each minimum
  /// being wider than theTol.
  Standard_EXPORT void GetMinima   (LineConn2d_ListOfReal&  outList,
                                    const Standard_Real     theTol) const;

  Standard_EXPORT void Dump        (Standard_OStream&       theStream,
                                    const                   Standard_Real)const;

  Standard_EXPORT void CheckBuffer();

 private:
  // ---------- PRIVATE (PROHIBITED) METHODS ----------

  /// Copy constructor
  LineConn2d_IntervalBuffer (const LineConn2d_IntervalBuffer& theOther);

  /// Assignment
  LineConn2d_IntervalBuffer& operator = (const LineConn2d_IntervalBuffer&);

 protected:
  // ---------- PROTECTED METHODS ----------

  /**
   * Test if theOther interval is intersected. If so, one of the intervals is
   * cut by the other; the cutting interval is the one with the greater Z. See
   * the source file LineConn2d_IntervalBuffer.cpp for this method definition.
   * This method should only be called with both arguments being members of
   * the collection myIntervals.
   * @param theInt
   *   theInt[0] - newly added interval; theInt[1] - a previous interval. 
   * @return
   *   Pointer to the interval that should be checked with the following
   *   intervals in the buffer.
   */
  LineConn2d_ZInterval * Cut (LineConn2d_ZInterval * theInt[2]);

 private:
  // ---------- PRIVATE FIELDS ----------

  NCollection_List<LineConn2d_ZInterval>    myIntervals;
};


#endif
