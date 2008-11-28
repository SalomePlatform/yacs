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
#ifndef YACSGui_TABLE_HEADER
#define YACSGui_TABLE_HEADER

#include <QtxTable.h>
#include <qvaluevector.h>
#include <qtoolbutton.h>

#include <list>

typedef std::list<int> IntListVal;
typedef std::list<double> DoubleListVal;

/*!
 * Class, intended to create table with different types of values entered to columns. 
 * The following types are possible: int, double (created as spinboxes), 
 * string (created as lineedit) and it's possible to specify combobox.
 */
class YACSGui_Table : public QtxTable
{
  Q_OBJECT

public:
  /*enum, defines types of columns
   */
  typedef enum { String, Int, Double, Combo, ComboSelect } CellType;

public:
  YACSGui_Table( const int numRows, const int numCols, QWidget*, CellType = Int );
  virtual ~YACSGui_Table();

  /*!
   * Returns type assigned to all cells of the table by default
   */
  CellType          defCellType() const;

  /*!
   * Set type of cells, which will be set to table by default
   */
  void             setDefCellType( CellType );

  /*!
   * Returns type of given cell, if cell type is not set 
   * evidently then returns type of cell which is set by default.
   * Parameters: int - row number
   *             int - column number
   *             bool - returns true if it is possible to determine column type, otherwise - false
   */
  CellType          cellType( const int, const int, bool& ) const;

  /*!
   * Set type for cell at the table
   * Parameters: int - row number, if row < 0 ( i.e. row is not defined),
   *                   set the same types for all rows in the given column
   *             int - column number
   *             CellType - cell type 
   */
  void             setCellType( const int, const int, CellType );


  /*!
   * Returns value by default for given type
   */
  QString          defValue( CellType ) const;

  /*!
   * Sets value by default for given type
   */
  void             setDefValue( CellType, const QString& );

  /*!
   * Returns value by default for given row and column (i.e. cell)
   */
  QString          defValue( const int, const int ) const;

  /*!
   * Sets value by default for given row and column (i.e. cell)
   */
  void             setDefValue( const int, const int, const QString& );


  /*!
   * This method finishes ediitng the cell contents in a cell specified by currEditRow() and currEditCol(). 
   * Modifications are accepted or rejected depending on <accept> parameter value
   */
  void             stopEdit( const bool );


  /*!
   * Returns list of double values for given column. If value is not defined 
   * in row of the table then 0 will be added to the list 
   * and parameter bool will return false
   * Paramrters: first int - column number
   *             second int - It is an index of string starting from which 
   *                          all following strings are ignored, i.e. their 
   *                          values are not added to the list 
   *             ok - returns true if all values from the strings 
   *                  had been sucessfully casted to double.
   */
  DoubleListVal    doubles( const int, const int = 0, bool* ok = 0 ) const;

  /*!
   * Fill given column with values, given at list. If size of list is lesser that number 
   * of rows in the table then the rest rows are filled by values set by parameter def.
   * Parameters: col - column number
   *             DoubleListVal- list of values
   *             def- value for rows which are not given at list
   *             Resize - determines if need to change number of rows in the table 
   *                      according to given list size
   */
  void             setDoubles( const int, const DoubleListVal&, const double, const bool = false );

  /*!
   * Returns list of integer values for given column. If value is not defined 
   * in row of the table then 0 will be added to the list 
   * and parameter bool will return false
   * Paramrters: first int - column number
   *             ok - returns true if all values from the strings 
   *                  had been sucessfully casted to double.
   */
  IntListVal       integers( const int, bool* ok = 0 ) const;

  /*!
   * Fill given column with values, given at list. If size of list is lesser that number 
   * of rows in the table then the rest rows are filled by values set by parameter def.
   * Parameters: col - column number
   *             DoubleListVal- list of values
   *             def- value for string which are not given at list
   *             Resize - determines if need to change number of rows in the table 
   *                      according to given list size
   */
  void             setIntegers( const int, const IntListVal&, const int, const bool = false );

  /*!
   * Returns list of string values for given column into QStringList. If value is not defined 
   * in row of the table then QString::null will be added to the list 
   * Paramrters: first int - column number
   *             QStringList - list of string values
   */
  void             strings( const int, QStringList& ) const;

  /*!
   * Fill given column with values, given at list.
   * Parameters: col - column number
   *             DoubleListVal- list of values
   *             Resize - determines if need to change number of rows in the table 
   *                      according to given list size
   */
  void             setStrings( const int, const QStringList&, const bool = false );

  /*!
   * Returns list of integer values for given column. If value is not defined 
   * in row of the table then -1 will be added to the list 
   * Paramrters: int - column number
   */
  IntListVal       valueComboBox( const int ) const;

  /*!
   * Returns values of integer of item for given row and column
   */
  int              intValue( const int, const int, bool* ok = 0 ) const;

  /*!
   * Returns values of double of item for given row and column
   */
  double           doubleValue( const int, const int, bool* ok = 0 ) const;

  /*!
   * Returns values of integer of item for given row and column
   */
  int              intValueCombo( const int, const int ) const;

  /*!
   * Allow getting value of special table field: when this field is true, 
   * the contents of the cell being edited are synchroinized with the contents of 
   * the cell editor widget, and valueChanged() signal is emitted each time the 
   * editor contents change - for instance, each time the user types a character 
   * in a line edit that is used as a cell editor.
   */
  bool             isEditorSync() const;

  /*!
   * Allow setting value of special table field: when this field is true, 
   * the contents of the cell being edited are synchroinized with the contents of 
   * the cell editor widget, and valueChanged() signal is emitted each time the 
   * eidtor contents change - for instance, eahc time the user types a character 
   * in a line edit that is used as a cell editor.
   */
  void             setEditorSync( const bool );

  /*!
   * Returns values(true/false) of item for given row and column
   * Parameters: first int - row - row number
   *             second int - col - column number
   */
  bool             readOnly( const int, const int ) const;

  /*!
   * Sets flag read only for given row and column.
   * Parameters: first int - row - row number, if row < 0 ( i.e. row is not defined),
   *                               set the read only flag for all rows in the given column
   *             second int - col - column number
   */
  void             setReadOnly( const int, const int, const bool );

  /*!
   * Rebuilds table items if necessary, for instance when the number of rows 
   * or columns changes.
   */
  void             update();

  /*!
   * Generates list of strings for given row and column (i.e. cell) which will be used at combobox
   * Parameters: int - row - row number, if row < 0 ( i.e. row is not defined),
   *                   set the same parameters for all rows in the given column
   *             int - col - column number
   *             Text - values for combobox for given column separated by ";"
   */
  void             setParams( const int, const int, const QString& );

  /*!
   * Returns list of strings for indexes of row and column (i.e. cell) which defines it's combobox contents
   * Parameters: int - row - row number
   *             int - col - column number
   */
  QStringList      Params( int, int ) const;

  /*!
   * Returns number of selected row
   */
  int              selectedRow() const; 
  
  virtual QRect    cellGeometry ( int row, int col ) const;

signals:
  void             itemCreated( const int, const int );
  void             selectButtonClicked( const int, const int );
  
public slots:
  /*!
   * Sets the number of the columns at the table. called method update()
   */
  virtual void setNumCols( int c );

  /*!
   * Sets the number of the rows at the table. called method update()
   */
  virtual void setNumRows( int r );

  /*!
   * Inserts "count" empty rows at row "row"
   */
  virtual void insertRows( int row, int count = 1 );

  /*!
   * Inserts "count" empty columns at column "col"
   */
  virtual void insertColumns( int col, int count = 1 );

  /*!
   * Redefine this slot to show/hide the cell's selection control
   */ 
  virtual void setCurrentCell ( int row, int col );

  void onSelectButton();

protected:
  /*!
   * creates new item for tables with type,
   * defined by CellType and by value, defined by default at row row and column col
   */
  virtual QTableItem*  createItem( const int, const int, CellType ) const;
  /*!
   * Returns type of the table item 
   */
  virtual CellType   itemType( QTableItem* ) const;

  virtual void endEdit( int row, int col, bool accept, bool replace );

protected slots:
  virtual void columnWidthChanged( int col );

  virtual void rowHeightChanged( int row );

private slots:
  /*!
   * slot which is called when syncronization is enabled - it 
   * receives new value from editor and saves it ro cell and emits valueChanged()
   */
  void onSync();

private:
  /*!
   * change number of strings according to count when resize == true, 
   * otherwise retur miniman value from count or numRows
   */
  int    updateSize( const int, const bool ); 

private:
  CellType                    myDefType;
  bool                       myIsSync;
  QMap<CellType, QString>     myTDefValues;
  
  QMap<int, QMap<int, QString> > myRCDefValues; // row <-> ( column <-> DefValue )

  QMap<int, QMap<int, QStringList> > myRCComboBoxParam; // row <-> ( column <-> Params )
  
  QValueVector<CellType>      myColTypes;
  QMap<int, QMap<int, CellType> > myCellTypes; //column <-> ( row <-> cellType )

};

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
   * Returns flag read only for item.
   */
  bool readOnly() const;

  /*!
   * Sets flag read only for item.
   */
  void setReadOnly( const bool );

protected:
  bool                       myIsReadOnly;
  YACSGui_Table::CellType     myType;

private:
};

/*!
 * The YACSGui_TableComboSelectItem class provides the cell content with combo box and selection control for YACSGui_Table cells.
 */
class YACSGui_TableComboSelectItem : public YACSGui_TableItem
{
public:
  YACSGui_TableComboSelectItem( QTable*, bool );
  virtual ~YACSGui_TableComboSelectItem();

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

  virtual void     showEditor( bool theIsEditing = false );
  virtual void     hideEditor();

  QToolButton*     selectButton() const { return mySelectBtn; }

  void             showText( QString theText );
  
private:
  void             createSelectButton();
  void             placeEditor( bool theIsEditing );
  void             updateSelectButtonState();

  QToolButton*     mySelectBtn;
  bool             myEditable;
};

#endif
