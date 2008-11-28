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
// File:      LineConn2d_PathIterator.cpp
// Created:   16.08.05 10:38:26
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005
//
#include <LineConn2dAfx.h>

#include <LineConn2d_PathIterator.h>

//=======================================================================
//function : LineConn2d_PathIterator()
//purpose  : Constructor
//=======================================================================

LineConn2d_PathIterator::LineConn2d_PathIterator
                        (const LineConn2d_Path&                   theRoot,
                         const Handle(NCollection_BaseAllocator)& theAlloc)
   : myStack (theAlloc),
     myLimit (1024)
{
  PathListIterator anIter (theRoot.myBranches);
  findNext (anIter);
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void LineConn2d_PathIterator::Next ()
{
  while (myStack.IsEmpty() == Standard_False) {
    PathListIterator& anIter = myStack.ChangeTop();
    anIter.Next();
    if (anIter.More()) {
      PathListIterator aNewIter (anIter.Value().myBranches);
      if (findNext (aNewIter))
        break;
    }
    myStack.Pop();
  }
}

//=======================================================================
//function : findNext
//purpose  : 
//=======================================================================

Standard_Boolean LineConn2d_PathIterator::findNext
                        (LineConn2d_PathIterator::PathListIterator& theIter)
{
  Standard_Boolean aResult (Standard_False);
  if (myStack.Depth() < myLimit) {
    while (theIter.More()) {
      myStack.Push (theIter);
      theIter = PathListIterator (theIter.Value().myBranches);
    }
    aResult = Standard_True;
  }
  return aResult;
}
