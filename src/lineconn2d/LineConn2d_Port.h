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
// File:      LineConn2d_Port.h
// Created:   03.08.05 22:50:38
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005
//
#ifndef LineConn2d_Port_HeaderFile
#define LineConn2d_Port_HeaderFile

#include <gp_Ax2d.hxx>
#include <LineConn2d.h>

class LineConn2d_Object;

///  Class LineConn2d_Port
//

class LineConn2d_Port : public gp_Ax2d
{
 public:
  // ---------- PUBLIC METHODS ----------

  /// Empty constructor
  inline LineConn2d_Port () : myObject (0L) {}

  /**
   * Query the associated Object.
   */
  inline const LineConn2d_Object&     Object () const
  { return * myObject; }

 protected:
  // ---------- PROTECTED METHODS ----------

  /// Constructor
  inline LineConn2d_Port (const LineConn2d_Object& theObj,
                          const gp_XY              theLocation,
                          const gp_Dir2d           theDir)
    : gp_Ax2d (theLocation, theDir), myObject (&theObj) {}

 private:
  // ---------- PRIVATE FIELDS ----------

  const LineConn2d_Object       * myObject;

  friend class LineConn2d_Model;
};

#endif
