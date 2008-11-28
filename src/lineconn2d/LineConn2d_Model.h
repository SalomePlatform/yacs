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
// File:      LineConn2d_Model.h
// Created:   03.08.05 20:08:48
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005
//
#ifndef LineConn2d_Model_HeaderFile
#define LineConn2d_Model_HeaderFile

#include <LineConn2d_Object.h>
#include <LineConn2d_Connection.h>
#include <LineConn2d_Port.h>
#include <LineConn2d_BoxTree.h>
#include <NCollection_List.hxx>

#include <NCollection_Vector.hxx>

class LineConn2d_Path;

///  Data Model for Line Connection algorithm.

class LineConn2d_Model
{
 public:
  // ---------- PUBLIC METHODS ----------

  /// Empty constructor
  Standard_EXPORT LineConn2d_Model ();

  /// Destructor
//  Standard_EXPORT virtual ~LineConn2d_Model ();

  /**
   * Create a new Object in the model. This is the exclusive way to create
   * new objects.
   * @return
   *   the index of the new object, always positive
   */
  Standard_EXPORT Standard_Integer AddObject    ();

  /**
   * Create a new Port in the model. This method checks the validity of
   * indices provided as parameters. It is the exclusive way to create
   * new ports.
   * @remark
   *   Thanks to Windows Platform SDK, AddPort is a kinda reserved word, we
   *   others cannot use it as identifier. So the method name is not mistyped.
   * @return
   *   the index of the new Port, always positive or 0 on error.
   */
  Standard_EXPORT Standard_Integer AddPoort     (const Standard_Integer iObj,
                                                 const gp_XY&           thePnt,
                                                 const gp_Dir2d&        theDir);

  /**
   * Create a new Connection in the model. This method checks the validity of
   * indices provided as parameters.
   * @param iPort1
   *   index of the first Port
   * @param iPort2
   *   index of the second Port
   * @return
   *   index of the new Connection (always positive) or 0 on error.
   */
  Standard_EXPORT Standard_Integer AddConnection(const Standard_Integer iPort1,
                                                 const Standard_Integer iPort2);

  /**
   * Query of the i-th Connection in the Model. You should be careful to provide
   * a correct parameter value.
   * @param i
   *   index of the object, in the range 1 .. NbConnections inclusive
   * @return
   *   const Connection reference
   */
  inline const LineConn2d_Connection& operator()(const Standard_Integer i) const
  { return myConnections(i); }

  /**
   * Query of the i-th Connection in the Model, non-const interface.
   * You should be careful to provide a correct parameter value.
   * @param i
   *   index of the object, in the range 1 .. NbConnections() inclusive
   * @return
   *   Connection reference
   */
  inline LineConn2d_Connection&     operator()  (const Standard_Integer i)
  { return myConnections(i); }

  /**
   * Query the i-th Object in the Model (const interface).
   * You should be careful to provide a correct parameter value.
   * @param i
   *   index of the object, in the range 1 .. NbObjects() inclusive
   * @return
   *   const Object reference
   */
  inline const LineConn2d_Object&   Object      (const Standard_Integer i) const
  { return myObjects(i); }

  /**
   * Query the i-th Object in the Model.
   * You should be careful to provide a correct parameter value.
   * @param i
   *   index of the object, in the range 1 .. NbObjects() inclusive
   * @return
   *   Object reference
   */
  inline LineConn2d_Object&         ChangeObject(const Standard_Integer i)
  { return myObjects(i); }

  /**
   * Query the i-th Port in the Model (const interface).
   * You should be careful to provide a correct parameter value.
   * @param i
   *   index of the object, in the range 1 .. NbPorts() inclusive
   * @return
   *   const Port reference
   */
  inline const LineConn2d_Port&     Port        (const Standard_Integer i) const
  { return myPorts(i); }

  /**
   * Query the i-th Port in the Model.
   * You should be careful to provide a correct parameter value.
   * @param i
   *   index of the object, in the range 1 .. NbPorts() inclusive
   * @return
   *   Port reference
   */
  inline LineConn2d_Port&           ChangePort  (const Standard_Integer i)
  { return myPorts(i); }

  /**
   * Query the number of currently stored objects.
   */
  inline Standard_Integer           NbObjects () const
  { return myObjects.Length() - 1; }

  /**
   * Query the number of currently stored objects.
   */
  inline Standard_Integer           NbPorts     () const
  { return myPorts.Length() - 1; }

  /**
   * Query the number of currently stored connections.
   */
  inline Standard_Integer           NbConnections () const
  { return myConnections.Length() - 1; }

  /**
   * Set the Tolerance value.
   */
  inline void                       SetTolerance (const Standard_Real theTol)
  { myTolerance = theTol; }

  /**
   * Query the Tolerance value.
   */
  inline Standard_Real              Tolerance   () const
  { return myTolerance; }

  /**
   * Set the port length parameter. By default it is equal to 2 * Tolerance().
   */
  inline void                       SetPortLength (const Standard_Real theLen)
  { myPortLength = theLen; }

  /**
   * Sets the value of mySearchDepth. The lower this value, the faster the
   * calculations, though for some connections the process may fail due to
   * not enough search depth allowed. Roughly, this parameter means the number
   * of breaks (direction changes) above the minimal, allowed for each branch
   * to propagate with new paths.
   * <p> The default value of this parameter is 4.
   */
  inline void                       SetSearchDepth(const Standard_Real theDep)
  { mySearchDepth = ::Max (0.1, theDep); }

  /**
   * Query the Tree structure. Should be called after Update().
   */
  inline const LineConn2d_BoxTree&  GetTree     () const
  { return myTree; }
  
  /**
   * Update the internal structures (including those of each contained Object)
   * when all data have been loaded.
   */
  Standard_EXPORT void              Update      ();

  /**
   * Query the bounding box of the Model. Should be called after Update().
   */
  inline const LineConn2d_Box&      Box         () const
  { return myTreeBox; }

  /**
   * Query the sum of the width and the height of the Model Box.
   */
  Standard_EXPORT Standard_Real     HalfPerimeter () const;

  /**
   * Query the allocator for temporary data. This allocator should be used
   * ONLY from the code called in Compute(); the allocator is destroyed in
   * the end of Compute, so please take care that no references to any
   * allocated objects are propagated at the moment when Compute() terminates.
   */ 
  Standard_EXPORT const Handle_NCollection_BaseAllocator&
                                    TempAlloc   () const;

  /**
   * Calculate the connections on already initialized Model.
   */
  Standard_EXPORT Standard_Boolean  Compute     ();

 private:
  Standard_Boolean createPath (LineConn2d_Path&                 thePath,
                               const gp_XY&                     theStart,
                               const Standard_Integer           iSide) const;

  void             pathOutline(NCollection_List<Standard_Real>& outList,
                               const LineConn2d_Path            thePath,
                               const Standard_Integer           theSide);

  Standard_Boolean checkPort  (const LineConn2d_Port& thePort) const;

  // ---------- PRIVATE (prohibited) METHODS ----------

  /// Copy constructor
  LineConn2d_Model (const LineConn2d_Model& theOther);

  /// Assignment operator
  LineConn2d_Model& operator = (const LineConn2d_Model& theOther);

 private:
  // ---------- PRIVATE FIELDS ----------

  /**
   * Allocator to manage allocations of all contained object data.
   */
  const Handle_NCollection_IncAllocator         myAlloc;

  /**
   * Allocator to manage allocations of all temporary data.
   */
  Handle_NCollection_IncAllocator               myTempAlloc;

  /**
   * Container of all Objects.
   */
  NCollection_Vector <LineConn2d_Object>        myObjects;

  /**
   * Container of all Connections.
   */
  NCollection_Vector <LineConn2d_Connection>    myConnections;

  /**
   * Container of all Ports.
   */
  NCollection_Vector <LineConn2d_Port>          myPorts;

  /**
   * Tree of bounding boxes indexing the Objects.
   */
  LineConn2d_BoxTree                            myTree;

  /**
   * Bounding box including all objects and port locations. Initialized in
   * the method Update().
   */
  LineConn2d_Box                                myTreeBox;

  /**
   * Tolerance value: the minimal distance from path segments model Objects.
   */
  Standard_Real                                 myTolerance;

  /**
   * Length of the short line connecting a port with the connection trajectory.
   */
  Standard_Real                                 myPortLength;

  /**
   * Parameter of the algorithm implemented in Compute(): the maximal price
   * interval in which the tree us updated by new paths. This value is
   * multiplied by LineConn2d_Path::PriceOfBreak(), so the rough meaning of
   * this field is the number of extra breaks allowed for each branch during
   * its propagation.
   */
  Standard_Real                                 mySearchDepth;

  // temporary fields, used inside the method Compute()
  Standard_Real                                 _PortLen;       ///< temporary
  gp_XY                                         _PntTgt;        ///< temporary
  LineConn2d_Segment                            _SegTgtPort;    ///< temporary

 public:
// Declaration of CASCADE RTTI
//DEFINE_STANDARD_RTTI (LineConn2d_Model)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
//DEFINE_STANDARD_HANDLE (LineConn2d_Model, )


#endif
