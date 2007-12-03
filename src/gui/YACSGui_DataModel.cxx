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

#include <YACSGui_DataModel.h>
#include <YACSGui_DataObject.h>
#include <YACSGui_Module.h>

#include <CAM_Module.h>
#include <CAM_Application.h>
#include <LightApp_Application.h>
#include <LightApp_Study.h>
#include <LightApp_Module.h>
#include <SalomeApp_Study.h>

#include <SALOMEDSClient.hxx>

#include <InPort.hxx>
#include <OutPort.hxx>
#include <OutputDataStreamPort.hxx>
#include <CalStreamPort.hxx>

#include <Switch.hxx>
#include <ForLoop.hxx>
#include <WhileLoop.hxx>
#include <ForEachLoop.hxx>

#include <ComponentInstance.hxx>
#include <CORBAComponent.hxx>

#include <qdatetime.h>
#include <qfileinfo.h>

using namespace YACS::ENGINE;

/*!
  Method for conversion enum value to string value
*/
QString YACSGui_DataModel::Type2Str( YACSGui_DataModel::ObjectType type )
{
  switch( type )
  {
  case SchemaObject:
    return "SchemaObject";
  case RunObject:
    return "RunObject";
  }
  return "UnknownObject";
}

YACSGui_DataModel::ObjectType YACSGui_DataModel::Str2Type( const QString& name)
{
  if ( name ==  "SchemaObject")
    return SchemaObject;
  else if ( name ==  "RunObject")
    return RunObject;

  return UnknownObject;
}

/*!
  Constructor
*/
YACSGui_DataModel::YACSGui_DataModel( CAM_Module* theModule ) :
  SalomeApp_DataModel(theModule)
{
}

/*!
  Destructor
*/
YACSGui_DataModel::~YACSGui_DataModel()
{
}

bool YACSGui_DataModel::open( const QString& theFileName, CAM_Study* theStudy, QStringList theListOfFiles)
{
  SalomeApp_DataModel::open( theFileName, theStudy, theListOfFiles );
}

bool YACSGui_DataModel::save( QStringList& theListOfFiles )
{
  SalomeApp_DataModel::save( theListOfFiles );
}

bool YACSGui_DataModel::saveAs( const QString& theFileName, CAM_Study* theStudy, QStringList& theListOfFiles )
{
  SalomeApp_DataModel::saveAs( theFileName, theStudy, theListOfFiles );
}


QString generateSchemeName( const _PTR(Study) theStudy,
                            const _PTR(SObject) theParent,
                            const QString thePattern)
{
  QString result("");
  if ( theStudy )
  {
    int index = 0;
    _PTR(GenericAttribute) anAttr;
    _PTR(AttributeName)    aName;
    _PTR(ChildIterator) it = theStudy->NewChildIterator(theParent);
    it->InitEx(true);
    while( it->More() )
    {
      _PTR(SObject) child = it->Value();
      if ( child->FindAttribute(anAttr, "AttributeName") )
      {
        aName = _PTR(AttributeName) ( anAttr );
        QString name ( aName->Value() );
        if (name.length() > thePattern.length())
        {
          if ( name.left( thePattern.length() ) == thePattern )
          {
            bool ok = true;
            int loc_index = name.right( name.length()- thePattern.length() ).toInt( &ok );
            if ( ok && index < loc_index)
              index = loc_index;                     
          }
        }
      }
      it->Next();
    }
    index++;
    result = thePattern + QString().setNum(index);
  }

  return result;
}

void YACSGui_DataModel::createNewSchema( QString& theName, Proc* proc)
{
  SalomeApp_ModuleObject* aRoot = dynamic_cast<SalomeApp_ModuleObject*>( root() );
  if ( aRoot )
  {
    _PTR(SComponent) aSComp( aRoot->object() );
    
    
    _PTR(Study)            aStudy = getStudy()->studyDS();
    _PTR(StudyBuilder)     aBuilder ( aStudy->NewBuilder() );
    _PTR(GenericAttribute) anAttr;
    _PTR(AttributeName)    aName;
    _PTR(AttributePixMap)  aPixmap;
    _PTR(AttributeParameter)  aType;
    
    _PTR(SObject) aSObj;
    // create a new schema SObject
    aSObj = aBuilder->NewObject(aSComp);
    
    anAttr =  aBuilder->FindOrCreateAttribute(aSObj, "AttributeName");
    aName = _PTR(AttributeName) ( anAttr );
    
    if ( !theName.isEmpty() )
      aName->SetValue( QFileInfo(theName).fileName() );
    else
    {
      theName = generateSchemeName(aStudy, aSComp, "Schema");
      aName->SetValue( theName );
    }

    anAttr = aBuilder->FindOrCreateAttribute(aSObj, "AttributePixMap");
    aPixmap = _PTR(AttributePixMap)( anAttr );
    aPixmap->SetPixMap( "ICON_SCHEMA_OBJECT" );

    anAttr = aBuilder->FindOrCreateAttribute(aSObj, "AttributeParameter");
    aType = _PTR(AttributeParameter)( anAttr );
    aType->SetInt( "ObjectType", SchemaObject );
    aType->SetString( "FilePath", theName );

    myObjectProcMap.insert( std::make_pair( aSObj->GetID(), proc ) );
  }
}

void YACSGui_DataModel::createNewRun( _PTR(SObject) parent, YACS::ENGINE::Proc* proc)
{
  SalomeApp_ModuleObject* aRoot = dynamic_cast<SalomeApp_ModuleObject*>( root() );
  if ( aRoot )
  {
    _PTR(Study)            aStudy = getStudy()->studyDS();
    _PTR(StudyBuilder)     aBuilder ( aStudy->NewBuilder() );
    _PTR(GenericAttribute) anAttr;
    _PTR(AttributeName)    aName;
    _PTR(AttributePixMap)  aPixmap;
    _PTR(AttributeParameter) aType;
    
    _PTR(SObject) aSObj;
    // create a new schema SObject

    QDateTime curTime = QDateTime::currentDateTime();
   
    QString name = curTime.toString( Qt::ISODate );//"24/10/2007-12:00";// test time
    if (parent->FindAttribute( anAttr, "AttributeName" ))
    {
      aName = _PTR(AttributeName) ( anAttr );
      name = aName->Value() + QString("-") + name;
    }
    
    aSObj = aBuilder->NewObject(parent);
    
    anAttr =  aBuilder->FindOrCreateAttribute(aSObj, "AttributeName");
    aName = _PTR(AttributeName) ( anAttr );
    
    aName->SetValue( name);

    anAttr = aBuilder->FindOrCreateAttribute(aSObj, "AttributePixMap");
    aPixmap = _PTR(AttributePixMap)( anAttr );
    aPixmap->SetPixMap( "ICON_RUN_OBJECT" );

    anAttr = aBuilder->FindOrCreateAttribute(aSObj, "AttributeParameter");
    aType = _PTR(AttributeParameter)( anAttr );
    aType->SetInt( "ObjectType", RunObject );

    myObjectProcMap.insert( std::make_pair( aSObj->GetID(), proc ) );
  }
}

YACSGui_DataModel::ObjectType YACSGui_DataModel::objectType( const QString& entry ) const
{
  _PTR(Study) aStudy = getStudy()->studyDS();
  if ( aStudy )
  {
    _PTR(SObject) sobj = aStudy->FindObjectID( entry );
    if ( sobj )
    {
      _PTR(GenericAttribute) anAttr;
      _PTR(AttributeInteger)  anType;
      if ( sobj->FindAttribute( anAttr, "AttributeParameter" ) )
        return (ObjectType) (_PTR(AttributeParameter)(anAttr))->GetInt("ObjectType");
    }
  }

  return UnknownObject;
}

YACS::ENGINE::Proc* YACSGui_DataModel::getProc( _PTR(SObject) theObject ) const
{
  Proc* aProc = 0;

  std::map<std::string, YACS::ENGINE::Proc*>::const_iterator aPIt = myObjectProcMap.find( theObject->GetID() );
  if ( aPIt != myObjectProcMap.end() ) aProc = (*aPIt).second;
 
  return aProc;
}

SUIT_DataObject* YACSGui_DataModel::getDataObject( YACS::ENGINE::Proc* theProc ) const
{
  SUIT_DataObject* aRetObj = 0;

  YACSGui_Module* aModule = dynamic_cast<YACSGui_Module*>( module() );

  if ( theProc && aModule )
  {
    // get a list of all schemas
    DataObjectList objlist = root()->children( true );
    
    for ( DataObjectListIterator it( objlist ); it.current(); ++it )
    {
      SalomeApp_DataObject* obj = dynamic_cast<SalomeApp_DataObject*>( it.current() );
      
      if ( obj && aModule && aModule->getApp()->checkDataObject(obj) )
        if ( getProc( obj->object() ) == theProc )
        {
          aRetObj = obj;
          break;
        }
    }
  }
    
  return aRetObj;
}

void YACSGui_DataModel::updateItem( YACS::ENGINE::Proc* theProc, bool theIsRecursive )
{
  if ( SalomeApp_DataObject* anObj = dynamic_cast<SalomeApp_DataObject*>( getDataObject(theProc) ) )
  {
    _PTR(Study)            aStudy = getStudy()->studyDS();
    _PTR(StudyBuilder)     aBuilder ( aStudy->NewBuilder() );
    _PTR(GenericAttribute) anAttr;
    _PTR(AttributeName)    aName;

    _PTR(SObject) aSObj = anObj->object();
    anAttr =  aBuilder->FindOrCreateAttribute(aSObj, "AttributeName");
    aName = _PTR(AttributeName) ( anAttr );
    aName->SetValue( theProc->getName() );    

    if ( theIsRecursive )
    {
      _PTR(SObject) aResult;
      _PTR(ChildIterator) anIterator ( aStudy->NewChildIterator(aSObj) );
      for (; anIterator->More(); anIterator->Next()) {
	if (anIterator->Value()->FindAttribute(anAttr, "AttributeName")) {
	  aName = _PTR(AttributeName) ( anAttr );
	  
	  QString aRunName = aName->Value();
	  int aRLength = QDateTime::currentDateTime().toString( Qt::ISODate ).length()+1;
	  aRunName = QString( theProc->getName() ) + aRunName.right( aRLength );

	  aName->SetValue( aRunName );
	  if ( Proc* aRunProc = getProc(anIterator->Value()) )
	    aRunProc->setName( aRunName );
	}
      }
    }

  }
}
