// File:      LineConn2d_Port.h
// Created:   03.08.05 22:50:38
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005


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
