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
// File:      LineConn2d_Connection.h
// Created:   03.08.05 21:52:02
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005
//
#ifndef LineConn2d_Connection_HeaderFile
#define LineConn2d_Connection_HeaderFile

#include <NCollection_List.hxx>
#include <LineConn2d_Segment.h>

///  Definition of a relation between two Ports.

class LineConn2d_Connection 
{
 public:
  typedef NCollection_List <LineConn2d_Segment>::Iterator SegIterator;

  // ---------- PUBLIC METHODS ----------

  /// Empty constructor
  LineConn2d_Connection () {}

  /**
   * Query the Port defined by the given index (0 or 1).
   */
  inline Standard_Integer Port          (const Standard_Integer i) const
  { return myPort[i&0x1]; }

  /**
   * Append the segment to the internal list of segments.
   */
  inline void             AddSegment    (const LineConn2d_Segment& theSegment)
  { mySegments.Append (theSegment); }

  /**
   * Prepend the segment to the internal list of segments.
   */
  inline void             PrependSegment(const LineConn2d_Segment& theSegment)
  { mySegments.Prepend (theSegment); }

  /**
   * Clear all contained segments.
   */
  inline void             ClearSegments ()
  { mySegments.Clear(); }

  /**
   * Query the number of contained segments.
   */
  inline Standard_Integer NbSegments    () const
  { return mySegments.Extent(); }

  /**
   * Query the list of contained segments.
   */
  inline SegIterator      SegmentIterator () const
  { return SegIterator (mySegments); }

 protected:
  // ---------- PROTECTED METHODS ----------

  /// Constructor
  Standard_EXPORT LineConn2d_Connection
                        (const Handle_NCollection_BaseAllocator&  theAlloc,
                         const Standard_Integer                   thePort1,
                         const Standard_Integer                   thePort2);

 private:
  // ---------- PRIVATE FIELDS ----------

  Standard_Integer                      myPort[2];
  NCollection_List <LineConn2d_Segment> mySegments;

  friend class LineConn2d_Model;
};

#endif
