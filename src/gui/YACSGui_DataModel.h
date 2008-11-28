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
#ifndef YACSGui_DataModel_HeaderFile
#define YACSGui_DataModel_HeaderFile

#include <ServiceNode.hxx>
#include <Proc.hxx>
#include <SalomeContainer.hxx>

#include <SalomeApp_DataModel.h>

#include <map>

class YACSGui_DataModel : public SalomeApp_DataModel
{
 public:
  enum ObjectType
    {
      SchemaObject,
      RunObject,
      UnknownObject
    };

  static QString Type2Str( ObjectType type );
  static ObjectType Str2Type( const QString& );

 public:
  YACSGui_DataModel(CAM_Module* theModule);
  virtual ~YACSGui_DataModel();

  virtual bool open( const QString&, CAM_Study*, QStringList );
  virtual bool save( QStringList& );
  virtual bool saveAs( const QString&, CAM_Study*, QStringList& );
  virtual bool close();

  void       createNewSchema( QString&, YACS::ENGINE::Proc* );
  void       createNewRun( _PTR(SObject), QString& theName, YACS::ENGINE::Proc* );

  ObjectType objectType( const QString& ) const;

  YACS::ENGINE::Proc* getProc( _PTR(SObject) ) const;
  SUIT_DataObject* getDataObject( YACS::ENGINE::Proc* ) const;

  void updateItem( YACS::ENGINE::Proc*, bool theIsRecursive = false );

 private:
  typedef std::map<std::string, YACS::ENGINE::Proc*> ObjectProcMap;
  ObjectProcMap myObjectProcMap;
};

#endif
