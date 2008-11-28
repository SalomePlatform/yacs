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
// File:      LineConn2d_Connection.cpp
// Created:   03.08.05 21:55:54
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005
//
#include <LineConn2dAfx.h>

#include <LineConn2d_Connection.h>

//=======================================================================
//function : LineConn2d_Connection()
//purpose  : Constructor
//=======================================================================

LineConn2d_Connection::LineConn2d_Connection
                        (const Handle(NCollection_BaseAllocator)& theAlloc,
                         const Standard_Integer                   thePort1,
                         const Standard_Integer                   thePort2)
   : mySegments (theAlloc)
{
  myPort[0] = thePort1;
  myPort[1] = thePort2;
}
