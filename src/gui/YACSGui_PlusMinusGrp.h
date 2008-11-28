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
#ifndef YACSGui_PlusMinusGrp_H 
#define YACSGui_PlusMinusGrp_H 
 
#include "QtxGroupBox.h" 
#include <qstringlist.h> 
 
class YACSGui_Table; 
class QToolButton; 
 
/*!  
  Class       : YACSGui_PlusMinusGrp 
  Description : Group box containing data table and "plus" "minus" "insert" "up" and "down" buttons 
*/ 
 
 
class YACSGui_PlusMinusGrp : public QtxGroupBox 
{ 
  Q_OBJECT 
 
public: 
 
  enum { PlusBtn = 0x1, MinusBtn = 0x2, InsertBtn = 0x4, UpBtn = 0x8, DownBtn = 0x10, SelectBtn = 0x20,
	 AllBtn = PlusBtn | MinusBtn | InsertBtn | UpBtn | DownBtn | SelectBtn }; 

public: 
  YACSGui_PlusMinusGrp( QWidget* theParent, const QString& theTitle, const int theBtns = AllBtn ); 
  virtual ~YACSGui_PlusMinusGrp(); 

  YACSGui_Table*                      Table(); 
  void                                ShowBtn( const int = AllBtn ); 
  void                                HideBtn( const int = AllBtn );
 
  /*! buttons become enabled or disabled depending on number of rows at a table 
      and max allowed number of rows for this table
      parameter: defines which buttons will be enabled/disabled
   */ 
  void                                EnableBtn( const int = AllBtn );
  void                                EnableBtn( const int, bool theEnable );

  /*! Returns the tool button with given id
      parameter: defines tool buttons' id
   */ 
  QToolButton*                        Btn( const int ) const;

  /*! Set min allowed number of rows at the table. 
      It is 0 by default.
   */
  inline void                         SetMinNumRow( const int theMinRow ) { myMinNumRow = theMinRow; };

  /*!
     returns min allowed number of strings for table or 0
   */
  inline int                          MinNumRow() { return myMinNumRow; }

  /*! Set max allowed number of rows at the table. 
      It is -1 by default. That means that number of rows is not limited
   */
  inline void                         SetMaxNumRow( const int theMaxRow ) { myMaxNumRow = theMaxRow; };

  /*!
     returns max allowed number of strings for table or -1
   */
  inline int                          MaxNumRow() { return myMaxNumRow; }

  /*! Set the row number before which a new row will be added in case of "Plus" button
      or "Insert" button with empty rows selection in the table. 
      It is -1 by default. That means that row is added to the end of the table
   */
  inline void                         SetAddBeforeRowNum( const int theAddBeforeRowNum ) { myAddBeforeRowNum = theAddBeforeRowNum; };

  /*!
     returns the row number before which a new row will be added or -1
   */
  inline int                          AddBeforeRowNum() { return myAddBeforeRowNum; }

  /*!
     untoggled the selection button
   */
  void                                setOffSelect();

signals:
  void                                MovedUp( const int theUpRowId );
  void                                MovedDown( const int theDownRowId );
  void                                Inserted( const int theInsRowId );
  void                                initSelection( const int theRowId, const int theColId );
  void                                Added( const int theRowId );
  void                                Removed( const int theRowId );

private slots: 
 
  /*! SLOT which called when button Up is pressed.
      Moves up the selected row by one row.
   */
  void                                onUp(); 

  /*! SLOT which called when button Down is pressed.
      Moves down the selected row by one row.
   */
  void                                onDown(); 

  /*! SLOT which called when button Insert is pressed.
      Adds new row before selected.
   */
  void                                onInsert(); 

  /*! SLOT which called when button Select is pressed.
      Emits the signal to start the selection in the tree.
      The selected object have to be put into the current cell.
   */
  void                                onSelect( bool ); 

  /*! SLOT which called when button Add is pressed.
      Adds new string at table's bottom.
   */
  void                                onAdd();  

  /*! SLOT which called when button Remove is pressed.
      Removed selected row from table.
   */
  void                                onRemove(); 

  /*! SLOT which called whenever a selection in the table changes
      or the current cell has changed.
      Apdated the enable/disable states of the tool buttons.
   */
  void                                onTableSelectionChanged();
 
private: 
  int                                 myMinNumRow;
  int                                 myMaxNumRow;
  int                                 myAddBeforeRowNum;
  
  YACSGui_Table*                      myTable; 
  QToolButton*                        myAddBtn; 
  QToolButton*                        myInsertBtn; 
  QToolButton*                        myRemoveBtn; 
  QToolButton*                        myUpBtn; 
  QToolButton*                        myDownBtn; 
  QToolButton*                        mySelectBtn;
}; 
 
#endif 
