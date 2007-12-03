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

#include <YACSGui_Table.h>

#include <QtxIntSpinBox.h>
#include <QtxDblSpinBox.h>

#include <qcheckbox.h>
#include <qlineedit.h>
#include <qpainter.h>
#include <qcombobox.h>

/*!
 * The YACSGui_TableItem class provides the cell content for YACSGui_Table cells.
 */
class YACSGui_TableItem : public QTableItem
{
public:
  YACSGui_TableItem( QTable*, YACSGui_Table::CellType );
  virtual ~YACSGui_TableItem();

  /*!
   * Returns type of item 
   */
  YACSGui_Table::CellType  type() const;

  /*!
   * Sets text for current item
   */
  virtual void     setText( const QString& );

  /*!
   * This virtual function creates an editor which the user can interact with to edit the cell's contents. 
   */
  virtual QWidget* createEditor() const;

  /*!
   * Updates cell contents after cell editing has been finished. Retrieves values from the editor widget.
   */
  virtual void     setContentFromEditor( QWidget* );

  /*!
   * Returnss flag read only for item.
   */
  bool readOnly() const;

  /*!
   * Sets flag read only for item.
   */
  void setReadOnly( const bool );

private:
  bool                       myIsReadOnly;
  YACSGui_Table::CellType     myType;
};

/*! constructor
 */
YACSGui_TableItem::YACSGui_TableItem( QTable* tab, YACSGui_Table::CellType t )
: QTableItem( tab, QTableItem::WhenCurrent ),
  myIsReadOnly( false ),
  myType( t )
{
}

/*! Destructor
 */
YACSGui_TableItem::~YACSGui_TableItem()
{
}

/*! Function: setText
 */
void YACSGui_TableItem::setText( const QString& txt )
{
  QTableItem::setText( txt );
  table()->updateCell( row(), col() );
}

/*! Function: readOnly
 */
bool YACSGui_TableItem::readOnly() const
{
  return myIsReadOnly;
}

/*! Function: setReadOnly
 */
void YACSGui_TableItem::setReadOnly( const bool theReadOnly )
{
  myIsReadOnly = theReadOnly;
}

/*! Function: type
 */
YACSGui_Table::CellType YACSGui_TableItem::type() const
{
  return myType;
}

/*! Function: createEditor
 */
QWidget* YACSGui_TableItem::createEditor() const
{
  if( myIsReadOnly )
    return 0;

  QWidget* res = 0;
  YACSGui_Table* tab = dynamic_cast<YACSGui_Table*>( table() );
  if( !tab )
    return 0;

  switch( type() )
  {
  case YACSGui_Table::String:
    res = new QLineEdit( text(), 0 );
    break;

  case YACSGui_Table::Int:
    {
      QtxIntSpinBox* sb = new QtxIntSpinBox( 0 );
      bool ok = false;
      int d = text().toInt( &ok );
      sb->setValue( ok ? d : 0 );
      res = sb;
    }
    break;

  case YACSGui_Table::Double:
    {
      QtxDblSpinBox* sb = new QtxDblSpinBox( 0 );
      bool ok = false;
      double d = text().toDouble( &ok );
      sb->setValue( ok ? d : 0.0 );
      res = sb;
    }
    break;
  case YACSGui_Table::Combo:
    {
      QComboBox* cb = new QComboBox( (QWidget*)0 );
      QStringList aL = tab->Params( row(), col() );
      if( !aL.empty() )
      {
	for( QStringList::Iterator it = aL.begin(); it != aL.end(); it++ )
          cb->insertItem( *it );
      }
      res = cb;
    }
    break;
  default:
    break;
  }

  if( res && tab->isEditorSync() )
  {
    QString sign;
    switch( type() )
    {
    case YACSGui_Table::String:
      sign = SIGNAL( textChanged( const QString& ) );
      break;

    case YACSGui_Table::Int:
      sign = SIGNAL( valueChanged( int ) );
      break;

    case YACSGui_Table::Double:
      sign = SIGNAL( valueChanged( double ) );
      break;

    case YACSGui_Table::Combo:
      sign = SIGNAL( textChanged( const QString& ) );
      break;

    default:
      break;
    }
    if( !sign.isEmpty() )
      QObject::connect( res, sign, tab, SLOT( onSync() ) );
  }

  return res;
}

/*! Function: setContentFromEditor
 */
void YACSGui_TableItem::setContentFromEditor( QWidget* w )
{
  QString res;
  switch( type() )
  {
  case YACSGui_Table::String:
    {
      QLineEdit* le = dynamic_cast<QLineEdit*>( w );
      if( le )
        res = le->text();
    }
    break;

  case YACSGui_Table::Int:
    {
      QtxIntSpinBox* sb = dynamic_cast<QtxIntSpinBox*>( w );
      if( sb )
        res = QString::number( sb->value() );
    }
    break;

  case YACSGui_Table::Double:
    {
      QtxDblSpinBox* sb = dynamic_cast<QtxDblSpinBox*>( w );
      if( sb )
        res = QString::number( sb->value() );
    }
    break;
    
  case YACSGui_Table::Combo:
    {
      QComboBox* cb = dynamic_cast<QComboBox*>( w );
      if( cb )
	res = cb->currentText();
    }
    break;

  default:
    break;
  }

  setText( res );
}


/*! constructor
 */
YACSGui_Table::YACSGui_Table( const int numRows, const int numCols, QWidget* parent, CellType defType )
: QtxTable( numRows, numCols, parent ),
  myDefType( defType ),
  myIsSync( false ),
  myColTypes( numCols, defType )
{
  for ( int i = 0; i < numCols; i++ )
  {
    QMap<int, CellType> aRTypes;
    for ( int j = 0; j < numRows; j++ )
      aRTypes.insert( j, defType );
    myCellTypes.insert( i, aRTypes );
  }
}

/*! Destructor
 */
YACSGui_Table::~YACSGui_Table()
{
}

/*! Function: defCellType
 */
YACSGui_Table::CellType YACSGui_Table::defCellType() const
{
  return myDefType;
}

/*! Function: setDefCellType
 */
void YACSGui_Table::setDefCellType( CellType t )
{
  myDefType = t;
}

/*! Function: cellType
 */
YACSGui_Table::CellType YACSGui_Table::cellType( const int indR, const int indC, bool& ok ) const
{
  ok = false;

  if ( indR < 0 ) // i.e. row is not defined
  {
    for( int i=0; i<numRows(); i++ )
    {
      QTableItem* it = item( i, indC );
      if( it )
      {
	ok = true;
	return itemType( it );
      }
    }
  }
  else
  {
    QTableItem* it = item( indR, indC );
    if( it )
    {
      ok = true;
      return itemType( it );
    }
  }

  if ( myCellTypes.contains(indC) && myCellTypes[indC].contains(indR) )
  {
    ok = true; 
    return myCellTypes[indC][indR];
  }
  else
    return defCellType();
}

/*! Function: setCellType
 */
void YACSGui_Table::setCellType( const int indR, const int indC, CellType t )
{
  if ( indC < myColTypes.size() )
    myColTypes[indC] = t;

  if ( indR < 0 ) 
  { // i.e. row is not defined
    // set the same types for all rows in the given column
    for( int i=0; i<numRows(); i++ )
    {
      if ( myCellTypes.contains(indC) )
      {
	if ( !myCellTypes[indC].contains(i) )
	  myCellTypes[indC].insert(i,t);
	else
	  myCellTypes[indC][i] = t;
      }
      setItem( i, indC, createItem( i, indC, t ) );
      emit itemCreated(i,indC);
    }
  }
  else
  {
    if ( myCellTypes.contains(indC) )
    {
      if ( !myCellTypes[indC].contains(indR) )
	myCellTypes[indC].insert( indR, t);
      else
	myCellTypes[indC][indR] = t;
    }
    setItem( indR, indC, createItem( indR, indC, t ) );
    emit itemCreated(indR,indC);
  }
}

/*! Function: setNumCols
 */
void YACSGui_Table::setNumCols( int c )
{
  int anOldNumCols = numCols();

  QtxTable::setNumCols( c );

  myColTypes.resize( c, defCellType() );

  // 1.) decrease the colunm list
  if ( anOldNumCols > numCols() )
  {
    for( int i=numCols(); i<anOldNumCols; i++ )
    {
      myCellTypes.remove(i);
      for ( int j=0; j<numRows(); j++ )
      {
	if ( myRCComboBoxParam[j].contains(i) ) myRCComboBoxParam[j].remove(i);
	if ( myRCDefValues[j].contains(i) ) myRCDefValues[j].remove(i);
      }
    }
  }
  
  // 2.) increase the colunm list
  if ( anOldNumCols < numCols() )
  {
    for( int i=anOldNumCols; i<numCols(); i++ )
    {
      QMap<int, CellType> aRTypes;
      for ( int j=0; j<numRows(); j++ )
	aRTypes.insert( j, defCellType() );
      myCellTypes.insert( i, aRTypes );
    }
  }
  
  update();
}

/*! Function: setNumRows
 */
void YACSGui_Table::setNumRows( int r )
{
  int anOldNumRows = numRows();

  QtxTable::setNumRows( r );

  for( int i=0; i<numCols(); i++ )
  {
    // 1.) decrease the row list
    if ( anOldNumRows > numRows() )
    {
      for( int j=numRows(); j<anOldNumRows; j++ )
      {
	myCellTypes[i].remove(j);
	
	if ( i == 0 && j > 0 && myRCComboBoxParam.contains(j) ) myRCComboBoxParam.remove(j);
	if ( i == 0 && j > 0 && myRCDefValues.contains(j) ) myRCDefValues.remove(j);
      }
    }
    
    // 2.) increase the row list
    if ( anOldNumRows < numRows() )
    {
      CellType aT;
      if ( i < myColTypes.size() ) aT = myColTypes[i];
      else aT = defCellType();

      for( int j=anOldNumRows; j<numRows(); j++ )
      {
	//bool ok;
	//aT = ( j == 0 ? aT : cellType( j-1, i, ok ) );
	myCellTypes[i].insert( j, aT );

	if ( i == 0 )
	{
	  if ( j > 0 && myRCComboBoxParam.contains(j-1) )
	    myRCComboBoxParam.insert( j, myRCComboBoxParam[j-1] ); // the previous line

	  if ( j > 0 && myRCDefValues.contains(j-1) )
	    myRCDefValues.insert( j, myRCDefValues[j-1] ); // the previous line
	}
      }
    }
  }

  update();
}

/*! Function: insertColumns
 */
void YACSGui_Table::insertColumns( int col, int count )
{
  QtxTable::insertColumns( col, count );
  update();
}

/*! Function: insertRows
 */
void YACSGui_Table::insertRows( int row, int count )
{
  QtxTable::insertRows( row, count );
  update();
}

/*! Function: createItem
 */
QTableItem* YACSGui_Table::createItem( const int row, const int col, CellType t ) const
{
  YACSGui_TableItem* it = new YACSGui_TableItem( ( QTable* )this, t ); 
  it->setText( defValue( row, col ) );
  return it;
}

/*! Function: update
 */
void YACSGui_Table::update()
{
  bool ok;
  for( int i=0; i<numRows(); i++ )
    for( int j=0; j<numCols(); j++ )
    {
      QTableItem* it = item( i, j );
      if( !it )
      {
	setItem( i, j, createItem( i, j, cellType( i, j, ok ) ) );
	emit itemCreated(i,j);
      }
    }
  stopEdit( true );
}

/*! Function: itemType
 */
YACSGui_Table::CellType YACSGui_Table::itemType( QTableItem* i ) const
{
  YACSGui_TableItem* it = dynamic_cast<YACSGui_TableItem*>( i );

  return it->type();
}

/*! Function: doubles
 */
DoubleListVal YACSGui_Table::doubles( const int col, const int except, bool* ok ) const
{
  if( ok )
    *ok = true;

  DoubleListVal aDoubleList;
  if( numRows()-except<1 )
    return aDoubleList;

  double val; 
  bool isOk;
  for( int i=0; i<numRows()-except; i++ )
  {
    QTableItem* it = item( i, col );
    if( it )
      val = it->text().toDouble( &isOk );
    aDoubleList.push_back( isOk ? val : 0.0 );
    if( ok )
      *ok = *ok && isOk;
  }
  return aDoubleList;
}

/*! Function: integers
 */
IntListVal YACSGui_Table::integers( const int col, bool* ok ) const
{
  if( ok )
    *ok = true;
  IntListVal aIntList;
  if( numRows()<1 )
    return aIntList;

  int val;
  bool isOk;
  for( int i=0; i<numRows(); i++ )
  {
    QTableItem* it = item( i, col );
    if( it )
      val = it->text().toInt( &isOk );
    aIntList.push_back( isOk ? val : 0 );
    if( ok )
      *ok = *ok && isOk;
  }

  return aIntList;
}

/*! Function: strings
 */
void YACSGui_Table::strings( const int col, QStringList& mat ) const
{
  mat.clear();
  for( int i=0; i<numRows(); i++ )
  {
    ( ( YACSGui_Table* )this )->endEdit( i, col, false, false );
    QTableItem* it = item( i, col );
    mat.append( it ? it->text() : QString::null );
  }  
}

/*! Function: updateSize
 */
int YACSGui_Table::updateSize( const int count, const bool resize )
{
  int size = -1;
  if( resize )
  {
    setNumRows( count );
    size = count;
  }
  else
    size = QMIN( numRows(), count );
  return size;
}

/*! Function: setDoubles
 */
void YACSGui_Table::setDoubles( const int col, const DoubleListVal& arr, const double def, const bool resize )
{
  if( col<0 || col>=numCols() )
    return;

  int size = updateSize( arr.empty() ? 0 : arr.size(), resize );
  DoubleListVal::const_iterator it = arr.begin();
  for( int i=0; i<numRows() && it != arr.end(); i++, it++ )
    item( i, col )->setText( QString::number( i<size ? *it : def ) );
}

/*! Function: setIntegers
 */
void YACSGui_Table::setIntegers( const int col, const IntListVal& arr, const int def, const bool resize )
{
  if( col<0 || col>=numCols() )
    return;

  int size = updateSize( arr.empty() ? 0 : arr.size(), resize );
  IntListVal::const_iterator it = arr.begin();
  for( int i=0; i<numRows() && it != arr.end(); i++, it++ )
  {
    int num = i<size ? *it : def;
    item( i, col )->setText( QString::number( num  ) );
  }
}

/*! Function: setStrings
 */
void YACSGui_Table::setStrings( const int col, const QStringList& mat, const bool resize )
{
  if( col<0 || col>=numCols() )
    return;

  int size = updateSize( mat.count(), resize );
  for( int i=0; i<numRows(); i++ )
    item( i, col )->setText( i<size ? mat[ i ] : QString::null );
}

/*! Function: isEditorSync
 */
bool YACSGui_Table::isEditorSync() const
{
  return myIsSync;
}

/*! Function: setEditorSync
 */
void YACSGui_Table::setEditorSync( const bool s )
{
  myIsSync = s;
}

/*! Function: onSync
 */
void YACSGui_Table::onSync()
{
  QTableItem* it = item( currEditRow(), currEditCol() );
  if( it && sender() && sender()->inherits( "QWidget" ) )
  {
    it->setContentFromEditor( ( QWidget* )sender() );
    emit valueChanged( currEditRow(), currEditCol() );
  }
}

/*! Function: readOnly
 */
bool YACSGui_Table::readOnly( const int row, const int col ) const
{
  YACSGui_TableItem* it = dynamic_cast<YACSGui_TableItem*>( item( row, col ) );
  return it ? it->readOnly() : false;
}

/*! Function: setReadOnly
 */
void YACSGui_Table::setReadOnly( const int row, const int col, const bool ro )
{
  // if row is not defined - set the read only flag for all rows in the given column
  int r1 = row >= 0 ? row : 0;
  int r2 = row >= 0 ? row : numRows()-1;

  for( int r = r1; r <= r2; r++ )
  {
    YACSGui_TableItem* it = dynamic_cast<YACSGui_TableItem*>( item( r, col ) );
    if( it )
    {
      endEdit( r, col, false, false );
      it->setReadOnly( ro );
      if( ro && !defValue( r, col ).isEmpty() )
	it->setText( defValue( r, col ) );
    }
  }
  update();
}

/*! Function: defValue
 */
QString YACSGui_Table::defValue( CellType t ) const
{
  QString res;
  if( myTDefValues.contains( t ) )
    res = myTDefValues[t];
  return res;
}

/*! Function: setDefValue
 */
void YACSGui_Table::setDefValue( CellType t, const QString& txt )
{
  myTDefValues.insert( t, txt );
}

/*! Function: defValue
 */
QString YACSGui_Table::defValue( const int indR, const int indC ) const
{
  if( myRCDefValues.contains( indR ) && myRCDefValues[indR].contains( indC ) )
    return myRCDefValues[indR][indC];
  else
  {
    bool ok;
    return defValue( cellType( indR, indC, ok ) );
  }
}

/*! Function: setDefValue
 */
void YACSGui_Table::setDefValue( const int indR, const int indC, const QString& txt )
{
  myRCDefValues[indR][indC] = txt; //.insert( ind, txt );
}

/*! Function: stopEdit
 */
void YACSGui_Table::stopEdit( const bool accept )
{
  endEdit( currEditRow(), currEditCol(), accept, accept );
}

/*! Function: setParams
 */
void YACSGui_Table::setParams( const int idR, const int idC, const QString& text )
{
  QStringList aL;
  if( !text.isEmpty() )
  {
    int length = text.length();
    int start = 0;
    int idx = 0;
    do
    {
      idx = text.find( ';', start );
      if( idx== -1 ) {
	idx = length;
      }
      aL.append( text.mid( start, idx-start ) );
      start = idx+1;
    } while( start < length );
  }

  if ( idR < 0 ) // i.e. row is not defined
  { // set the same parameters for all rows in the given column
    for( int i=0; i<numRows(); i++ )
    {
      if ( !myRCComboBoxParam.contains( i ) )
	myRCComboBoxParam.insert( i, QMap<int, QStringList>() );
      myRCComboBoxParam[i].insert( idC, aL );
    }
  }
  else
  {
    if ( !myRCComboBoxParam.contains( idR ) )
      myRCComboBoxParam.insert( idR, QMap<int, QStringList>() );
    myRCComboBoxParam[idR].insert( idC, aL );
  }
}

/*! Function: Params
 */
QStringList YACSGui_Table::Params( const int idR, const int idC ) const
{
  if( myRCComboBoxParam.contains( idR ) && myRCComboBoxParam[idR].contains( idC ) )
    return myRCComboBoxParam[idR][idC];
  else 
    return 0;
}

/*! Function: valueComboBox
 */
IntListVal YACSGui_Table::valueComboBox( const int col ) const
{
  IntListVal aIntList;
  if( numRows()<1 )
    return aIntList;

  QString val; int idx;
  for( int i=0; i<numRows(); i++ )
  {
    QStringList aL = Params( i, col );
    
    QTableItem* it = item( i, col );
    if( it )
      val = it->text();
    idx = aL.findIndex( val );
    aIntList.push_back( idx != -1 ? idx : 0 );
  }
  return aIntList;
}

/*! Function: intValue
 */
int YACSGui_Table::intValue( const int col, const int row, bool* ok ) const
{
  if( ok )
    *ok = true;
  if( numRows() < row || numCols() < col )
    return -1;

  int val = 0;

  QTableItem* it = item( row, col );
  if( it )
    val = it->text().toInt( ok );

  return val;
}

/*! Function: doubleValue
 */
double YACSGui_Table::doubleValue( const int col, const int row, bool* ok ) const
{
  if( ok )
    *ok = true;
  if( numRows() < row || numCols() < col )
    return -1.;

  double val = 0.;

  QTableItem* it = item( row, col );
  if( it )
    val = it->text().toDouble( ok );

  return val;
}

/*! Function: intValueCombo
 */
int YACSGui_Table::intValueCombo( const int col, const int row ) const
{
  if( numRows() < row || numCols() < col )
    return -1;

  QString text; 
  int idx, val;
  QStringList aL = Params( row, col );

  QTableItem* it = item( row, col );
  if( it )
    text = it->text();
  idx = aL.findIndex( text );
  val = (idx != -1 ? idx : 0);
  
  return val;
}

/*! Function: selectedRow
 */
int YACSGui_Table:: selectedRow() const
{
  QTableSelection sel = selection( currentSelection() );
  if ( sel.isActive() )
    return sel.topRow();
  return -1;
}

