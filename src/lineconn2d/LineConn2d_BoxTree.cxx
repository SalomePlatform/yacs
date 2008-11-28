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
// File:      LineConn2d_BoxTree.cpp
// Created:   12.08.05 15:19:55
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005
//
#include <LineConn2dAfx.h>

#include <LineConn2d_BoxTree.h>
#include <LineConn2d_Segment.h>

//=======================================================================
//function : LineConn2d_BoxTreeSelector
//purpose  : Constructor
//=======================================================================

LineConn2d_BoxTreeSelector::LineConn2d_BoxTreeSelector
                (const LineConn2d_Segment& theSeg)
{
  myBox.Add (theSeg.Origin());
  myBox.Add (theSeg.Extremity());
}


//=======================================================================
//function : Accept
//purpose  : 
//=======================================================================

Standard_Boolean LineConn2d_BoxTreeSelector::Accept
                                (const LineConn2d_Object * const& theObj)
{
  myList.Append (theObj);
  return Standard_True;
}

//=======================================================================
//function : Reject
//purpose  : 
//=======================================================================

Standard_Boolean LineConn2d_BoxTreeSelector::Reject
                                (const LineConn2d_Box& theBox) const
{
  return theBox.IsOut (myBox);
}

