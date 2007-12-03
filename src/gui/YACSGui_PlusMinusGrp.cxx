//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "YACSGui_PlusMinusGrp.h"

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>

#include <YACSGui_Table.h>

#include <qtoolbutton.h>


/* 
  Class       : YACSGui_PlusMinusGrp
  Description : Group box containing data table and "plus" "minus" and "insert" buttons
*/

//================================================================
// Function : YACSGui_PlusMinusGrp
// Purpose  : 
//================================================================
YACSGui_PlusMinusGrp::YACSGui_PlusMinusGrp( QWidget* theParent, 
					    const QString& theTitle,
					    const int theBtns )
: QtxGroupBox( 1, Qt::Horizontal, theTitle, theParent ), 
myAddBtn( 0 ),
myInsertBtn( 0 ),
myRemoveBtn( 0 ),
myUpBtn( 0 ),
myDownBtn( 0 ),
myMinNumRow( 0 ),
myMaxNumRow( -1 ),
myAddBeforeRowNum( -1 )
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();

  // cerate buttons
  if ( theBtns & UpBtn )
  {
    QPixmap upPix = aResMgr->loadPixmap( "YACSGui", tr( "ICON_UP" ) );
    myUpBtn = new QToolButton( 0 );
    myUpBtn->setIconSet( upPix );
    myUpBtn->setFixedSize( upPix.size() );
    insertTitleWidget( myUpBtn );
  }
  else
    myUpBtn = 0;

  if ( theBtns & DownBtn )
  {
    QPixmap downPix = aResMgr->loadPixmap( "YACSGui", tr( "ICON_DOWN" ) );
    myDownBtn = new QToolButton( 0 );
    myDownBtn->setIconSet( downPix );
    myDownBtn->setFixedSize( downPix.size() );
    insertTitleWidget( myDownBtn );
  }
  else
    myDownBtn = 0;

  if ( theBtns & InsertBtn )
  {
    QPixmap insertPix = aResMgr->loadPixmap( "YACSGui", tr( "ICON_INSERT" ) );
    myInsertBtn = new QToolButton( 0 );
    myInsertBtn->setIconSet( insertPix );
    myInsertBtn->setFixedSize( insertPix.size() );
    insertTitleWidget( myInsertBtn );
  }
  else
    myInsertBtn = 0;

  if ( theBtns & SelectBtn )
  {
    QPixmap selectPix = aResMgr->loadPixmap( "YACSGui", tr( "ICON_SELECT" ) );
    mySelectBtn = new QToolButton( 0 );
    mySelectBtn->setIconSet( selectPix );
    mySelectBtn->setFixedSize( selectPix.size() );
    mySelectBtn->setToggleButton( true );
    insertTitleWidget( mySelectBtn );
  }
  else
    mySelectBtn = 0;

  if ( theBtns & PlusBtn )
  {
    QPixmap plusPix = aResMgr->loadPixmap( "YACSGui", tr( "ICON_PLUS" ) );
    myAddBtn = new QToolButton( 0 );
    myAddBtn->setIconSet( plusPix );
    myAddBtn->setFixedSize( plusPix.size() );
    insertTitleWidget( myAddBtn );
  }
  else
    myAddBtn = 0;

  if ( theBtns & MinusBtn )
  {
    QPixmap minusPix = aResMgr->loadPixmap( "YACSGui", tr( "ICON_MINUS" ) );
    myRemoveBtn = new QToolButton( 0 );
    myRemoveBtn->setIconSet( minusPix );
    myRemoveBtn->setFixedSize( minusPix.size() );
    insertTitleWidget( myRemoveBtn );
  }
  else
    myRemoveBtn = 0;

  //create table
  myTable = new YACSGui_Table( 0, 0, this );
  myTable->setSelectionMode( QTable::SingleRow );

  // connect signals and slots
  if ( myUpBtn )
    connect( myUpBtn, SIGNAL( clicked() ), SLOT( onUp() ) );
  if ( myDownBtn )
    connect( myDownBtn, SIGNAL( clicked() ), SLOT( onDown() ) );
  if ( myAddBtn )
    connect( myAddBtn, SIGNAL( clicked() ), SLOT( onAdd() ) );
  if ( myInsertBtn )
    connect( myInsertBtn, SIGNAL( clicked() ), SLOT( onInsert() ) );
  if ( mySelectBtn )
    connect( mySelectBtn, SIGNAL( toggled( bool ) ), SLOT( onSelect( bool ) ) );
  if ( myRemoveBtn )
    connect( myRemoveBtn, SIGNAL( clicked() ), SLOT( onRemove() ) );

  connect( myTable, SIGNAL( selectionChanged() ), this, SLOT( onTableSelectionChanged() ) );
  connect( myTable, SIGNAL( currentChanged( int, int ) ), this, SLOT( onTableSelectionChanged() ) );
}

YACSGui_PlusMinusGrp::~YACSGui_PlusMinusGrp()
{
}

//================================================================
// Function : Table
// Purpose  : 
//================================================================
YACSGui_Table* YACSGui_PlusMinusGrp::Table()
{
  return myTable;
}

//================================================================
// Function : onUp
// Purpose  : 
//================================================================
void YACSGui_PlusMinusGrp::onUp()
{
  int row;
  row = myTable->currentRow( );
  if ( row >= 0 )
  {
    myTable->swapRows( row, row-1 );
    //myTable->updateContents( QRect( myTable->contentsWidth(), 
    //                                myTable->rowHeight(row)+myTable->rowHeight(row-1) ) );
    myTable->ensureCellVisible( row-1, myTable->currentColumn() );
    emit MovedUp( row-1 );
  }

  EnableBtn( AllBtn );
}

//================================================================
// Function : onDown
// Purpose  : 
//================================================================
void YACSGui_PlusMinusGrp::onDown()
{
  int row;
  row = myTable->currentRow( );
  if ( row >= 0 )
  {
    myTable->swapRows( row, row+1 );
    //myTable->updateContents( QRect( myTable->contentsWidth(), 
    //                                myTable->rowHeight(row)+myTable->rowHeight(row+1) ) );
    myTable->ensureCellVisible( row+1, myTable->currentColumn() );
    emit MovedDown( row+1 );
  }
  
  EnableBtn( AllBtn );
}

//================================================================
// Function : onInsert
// Purpose  : 
//================================================================
void YACSGui_PlusMinusGrp::onInsert()
{
  int row;
  row = myTable->currentRow( );
  if ( row < 0 )
    onAdd();
  else
  {
    myTable->insertRows( row );
    myTable->setCurrentCell( row, myTable->currentColumn() );
    myTable->ensureCellVisible( row, myTable->currentColumn() );
    emit Inserted( row );
  }
  
  EnableBtn( AllBtn );
}

//================================================================
// Function : onSelect
// Purpose  : 
//================================================================
void YACSGui_PlusMinusGrp::onSelect( bool on )
{
  printf(">> YACSGui_PlusMinusGrp::onSelect : on = %d\n",on);
  if ( on )
  {
    int row = myTable->currentRow();
    int col = myTable->currentColumn();
    printf(">> row = %d, col = %d\n",row,col);
    if ( row >= 0 && col >= 0 )
    {
      emit initSelection( row, col );
    }
    
    EnableBtn( AllBtn );
  }
  else
  {
    // do nothing (i.e. rollback the selection)
  }
}

//================================================================
// Function : onAdd
// Purpose  : 
//================================================================
void YACSGui_PlusMinusGrp::onAdd()
{
  if ( myAddBeforeRowNum >= 0 )
  {
    myTable->insertRows( myAddBeforeRowNum );
    myTable->ensureCellVisible( myAddBeforeRowNum, myTable->currentColumn() );
    emit Added( myAddBeforeRowNum );
  }
  else
  {
    myTable->setNumRows( myTable->numRows() + 1 );
    myTable->ensureCellVisible( myTable->numRows() - 1, myTable->currentColumn() );
    emit Added( myTable->numRows() - 1 );
  }

  EnableBtn( AllBtn );   
}

//================================================================
// Function : onRemove
// Purpose  : 
//================================================================
void YACSGui_PlusMinusGrp::onRemove()
{
  int row;
  row = myTable->currentRow( );
  if ( row >= 0 )
  {
    emit Removed( row );
    myTable->removeRow( row );
    myTable->ensureCellVisible( row-1, myTable->currentColumn() );
  }
  
  EnableBtn( AllBtn );
}

//================================================================
// Function : onTableSelectionChanged
// Purpose  : 
//================================================================
void YACSGui_PlusMinusGrp::onTableSelectionChanged()
{
  if ( YACSGui_Table* aTable = ( YACSGui_Table* )sender() )
    if ( aTable == myTable && aTable->currentSelection() != -1 ) // the current selection is not empty
    {
      EnableBtn( AllBtn );
      EnableBtn( SelectBtn, true );
    }
}

//================================================================
// Function : ShowBtn
// Purpose  : 
//================================================================
void YACSGui_PlusMinusGrp::ShowBtn( const int theBtns )
{
  if ( ( theBtns & UpBtn ) && myUpBtn != 0 )
    myUpBtn->show();
  if ( ( theBtns & DownBtn ) && myDownBtn != 0 )
    myDownBtn->show();
  if ( ( theBtns & InsertBtn ) && myInsertBtn != 0 )
    myInsertBtn->show();
  if ( ( theBtns & SelectBtn ) && mySelectBtn != 0 )
    mySelectBtn->show();
  if ( ( theBtns & PlusBtn ) && myAddBtn != 0 )
    myAddBtn->show();
  if ( ( theBtns & MinusBtn ) && myRemoveBtn != 0 )
    myRemoveBtn->show();
}

//================================================================
// Function : HideBtn
// Purpose  : 
//================================================================
void YACSGui_PlusMinusGrp::HideBtn( const int theBtns )
{
  if ( ( theBtns & UpBtn ) && myUpBtn != 0 )
    myUpBtn->hide();
  if ( ( theBtns & DownBtn ) && myDownBtn != 0 )
    myDownBtn->hide();
  if ( ( theBtns & InsertBtn ) && myInsertBtn != 0 )
    myInsertBtn->hide();
  if ( ( theBtns & SelectBtn ) && mySelectBtn != 0 )
    mySelectBtn->hide();
  if ( ( theBtns & PlusBtn ) && myAddBtn != 0 )
    myAddBtn->hide();
  if ( ( theBtns & MinusBtn ) && myRemoveBtn != 0 )
    myRemoveBtn->hide();
}

//================================================================
// Function : EnableBtn
// Purpose  : 
//================================================================
void YACSGui_PlusMinusGrp::EnableBtn( const int theBtns )
{
  if ( myTable->numRows() == 0 ) // an empty table
  {
    EnableBtn( UpBtn | DownBtn | MinusBtn, false );
    return;
  }  

  bool theEnable = true;

  if( myMaxNumRow <= myTable->numRows() && myMaxNumRow != -1 )
  {
    theEnable = false;
  }

  EnableBtn( InsertBtn, theEnable );
  EnableBtn( PlusBtn, theEnable );

  if ( ( theBtns & MinusBtn ) && myRemoveBtn != 0 )
  {
    theEnable = true;
    if( myMinNumRow >= myTable->numRows() && myMinNumRow != 0 )
      theEnable = false;
    myRemoveBtn->setEnabled( theEnable );
  }
  
  if ( myTable->numRows() == 1 )
    EnableBtn( UpBtn | DownBtn, false );
  else if ( myTable->currentRow() == 0 )
  {
    EnableBtn( UpBtn, false );
    EnableBtn( DownBtn, true );
  }
  else if ( myTable->currentRow() == myTable->numRows()-1 )
  {
    EnableBtn( UpBtn, true );
    EnableBtn( DownBtn, false );
  }
  else if ( myTable->currentRow() > 0 )
    EnableBtn( UpBtn | DownBtn, true );
}

//================================================================
// Function : EnableBtn
// Purpose  : 
//================================================================
void YACSGui_PlusMinusGrp::EnableBtn( const int theBtns, bool theEnable )
{
  if ( ( theBtns & UpBtn ) && myUpBtn != 0 )
    myUpBtn->setEnabled( theEnable );
  if ( ( theBtns & DownBtn ) && myDownBtn != 0 )
    myDownBtn->setEnabled( theEnable );
  if ( ( theBtns & InsertBtn ) && myInsertBtn != 0 )
    myInsertBtn->setEnabled( theEnable );
  if ( ( theBtns & SelectBtn ) && mySelectBtn != 0 )
    mySelectBtn->setEnabled( theEnable );
  if ( ( theBtns & PlusBtn ) && myAddBtn != 0 )
    myAddBtn->setEnabled( theEnable );
  if ( ( theBtns & MinusBtn ) && myRemoveBtn != 0 )
    myRemoveBtn->setEnabled( theEnable );
}

//================================================================
// Function : setOffSelect
// Purpose  : 
//================================================================
void YACSGui_PlusMinusGrp::setOffSelect()
{
  if ( mySelectBtn && mySelectBtn->isOn() )
    mySelectBtn->setOn(false);
}
