// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "HXX2SALOME_GENERIC_CLASS_NAMEGUI.h"

#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_Preferences.h>

#include <SALOME_LifeCycleCORBA.hxx>

#define COMPONENT_NAME "HXX2SALOME_GENERIC_CLASS_NAME"

using namespace std;

// Constructor
HXX2SALOME_GENERIC_CLASS_NAMEGUI::HXX2SALOME_GENERIC_CLASS_NAMEGUI() :
  SalomeApp_Module( COMPONENT_NAME ) // Module name
{
  // Initializations
  default_bool = false;
  default_int = 0;
  default_spinInt = 0;
  default_spinDbl = 0.;
  default_selection = QString("");
  
  // List for the selector
  selector_strings.clear();
  selector_strings.append( tr( "PREF_LIST_TEXT_0" ) );
  selector_strings.append( tr( "PREF_LIST_TEXT_1" ) );
  selector_strings.append( tr( "PREF_LIST_TEXT_2" ) );
}

// Gets a reference to the module's engine
HXX2SALOME_GENERIC_CLASS_NAME_ORB::HXX2SALOME_GENERIC_CLASS_NAME_Gen_ptr HXX2SALOME_GENERIC_CLASS_NAMEGUI::InitHXX2SALOME_GENERIC_CLASS_NAMEGen( SalomeApp_Application* app )
{
  Engines::EngineComponent_var comp = app->lcc()->FindOrLoad_Component( "FactoryServer",COMPONENT_NAME );
  HXX2SALOME_GENERIC_CLASS_NAME_ORB::HXX2SALOME_GENERIC_CLASS_NAME_Gen_ptr clr = HXX2SALOME_GENERIC_CLASS_NAME_ORB::HXX2SALOME_GENERIC_CLASS_NAME_Gen::_narrow(comp);
  ASSERT(!CORBA::is_nil(clr));
  return clr;
}

// Module's initialization
void HXX2SALOME_GENERIC_CLASS_NAMEGUI::initialize( CAM_Application* app )
{
  // Get handle to Application, Desktop and Resource Manager
  SalomeApp_Module::initialize( app );

  InitHXX2SALOME_GENERIC_CLASS_NAMEGen( dynamic_cast<SalomeApp_Application*>( app ) );

  QWidget* aParent = app->desktop();
  
  SUIT_ResourceMgr* aResourceMgr = app->resourceMgr();
  
  // GUI items
  // --> Create actions: 190 is linked to item in "File" menu 
  //     and 901 is linked to both specific menu and toolbar
  createAction( 190, tr( "TLT_MY_NEW_ITEM" ), QIconSet(), tr( "MEN_MY_NEW_ITEM" ), tr( "STS_MY_NEW_ITEM" ), 0, aParent, false,
		this, SLOT( OnMyNewItem() ) );

  QPixmap aPixmap = aResourceMgr->loadPixmap( COMPONENT_NAME,tr( "ICON_HXX2SALOME_GENERIC_CLASS_NAME" ) );
  createAction( 901, tr( "TLT_HXX2SALOME_GENERIC_CLASS_NAME_ACTION" ), QIconSet( aPixmap ), tr( "MEN_HXX2SALOME_GENERIC_CLASS_NAME_ACTION" ), tr( "STS_HXX2SALOME_GENERIC_CLASS_NAME_ACTION" ), 0, aParent, false,
		this, SLOT( OnCallAction() ) );

  // --> Create item in "File" menu
  int aMenuId;
  aMenuId = createMenu( tr( "MEN_FILE" ), -1, -1 );
  createMenu( separator(), aMenuId, -1, 10 );
  aMenuId = createMenu( tr( "MEN_FILE_HXX2SALOME_GENERIC_CLASS_NAME" ), aMenuId, -1, 10 );
  createMenu( 190, aMenuId );

  // --> Create specific menu
  aMenuId = createMenu( tr( "MEN_HXX2SALOME_GENERIC_CLASS_NAME" ), -1, -1, 30 );
  createMenu( 901, aMenuId, 10 );

  // --> Create toolbar item
  int aToolId = createTool ( tr( "TOOL_HXX2SALOME_GENERIC_CLASS_NAME" ) );
  createTool( 901, aToolId );
}

// Module's engine IOR
QString HXX2SALOME_GENERIC_CLASS_NAMEGUI::engineIOR() const
{
  CORBA::String_var anIOR = getApp()->orb()->object_to_string( InitHXX2SALOME_GENERIC_CLASS_NAMEGen( getApp() ) );
  return QString( anIOR.in() );
}

// Module's activation
bool HXX2SALOME_GENERIC_CLASS_NAMEGUI::activateModule( SUIT_Study* theStudy )
{
  bool bOk = SalomeApp_Module::activateModule( theStudy );

  setMenuShown( true );
  setToolShown( true );

  return bOk;
}

// Module's deactivation
bool HXX2SALOME_GENERIC_CLASS_NAMEGUI::deactivateModule( SUIT_Study* theStudy )
{
  setMenuShown( false );
  setToolShown( false );

  return SalomeApp_Module::deactivateModule( theStudy );
}

// Default windows
void HXX2SALOME_GENERIC_CLASS_NAMEGUI::windows( QMap<int, int>& theMap ) const
{
  theMap.clear();
  theMap.insert( SalomeApp_Application::WT_ObjectBrowser, Qt::DockLeft );
  theMap.insert( SalomeApp_Application::WT_PyConsole,     Qt::DockBottom );
}

// Action slot: Launched with action 190
void HXX2SALOME_GENERIC_CLASS_NAMEGUI::OnMyNewItem()
{
  SUIT_MessageBox::warn1( getApp()->desktop(),tr( "INF_HXX2SALOME_GENERIC_CLASS_NAME_TITLE" ), tr( "INF_HXX2SALOME_GENERIC_CLASS_NAME_TEXT" ), tr( "BUT_OK" ) );
}

// Action slot: Launched with action 901
void HXX2SALOME_GENERIC_CLASS_NAMEGUI::OnCallAction()
{
  // Create a HXX2SALOME_GENERIC_CLASS_NAME component
  HXX2SALOME_GENERIC_CLASS_NAME_ORB::HXX2SALOME_GENERIC_CLASS_NAME_Gen_ptr HXX2SALOME_GENERIC_CLASS_NAMEgen = HXX2SALOME_GENERIC_CLASS_NAMEGUI::InitHXX2SALOME_GENERIC_CLASS_NAMEGen( getApp() );
  
  // Do the job...
  //
  // HXX2SALOME_GENERIC_CLASS_NAMEgen->method( arg1, arg2, ... );
  
  // Open a dialog showing Preferences values (just to display something)
  
  // ****** Direct access to preferences: implementation at 12/12/05 ******
  // Comment out this section when "preferencesChanged" called back
  SUIT_ResourceMgr* mgr = SUIT_Session::session()->resourceMgr();
  
  default_bool = mgr->booleanValue(COMPONENT_NAME, "default_bool", false);

  default_int = mgr->integerValue(COMPONENT_NAME, "default_integer", 3);

  default_spinInt = mgr->integerValue(COMPONENT_NAME, "default_spinint", 4);

  default_spinDbl = mgr->doubleValue(COMPONENT_NAME, "default_spindbl", 4.5);

  int selectorIndex = mgr->integerValue(COMPONENT_NAME, "default_selector");
  default_selection = (0<=selectorIndex && selectorIndex<=selector_strings.count() ? selector_strings[selectorIndex]: QString("None"));
  // ****** End of section to be commented out ******
  
  QString SUC = ( default_bool ? QString( tr ("INF_HXX2SALOME_GENERIC_CLASS_NAME_CHECK") ) : QString( tr("INF_HXX2SALOME_GENERIC_CLASS_NAME_UNCHECK") ) ) ;
    
  QString textResult = QString( tr( "RES_HXX2SALOME_GENERIC_CLASS_NAME_TEXT" ) ).arg(SUC).arg(default_int).arg(default_spinInt).arg(default_spinDbl).arg(default_selection);
  SUIT_MessageBox::info1( getApp()->desktop(), tr( "RES_HXX2SALOME_GENERIC_CLASS_NAME_TITLE" ), textResult, tr( "BUT_OK" ) );
}

void HXX2SALOME_GENERIC_CLASS_NAMEGUI::createPreferences()
{
  // A sample preference dialog
  
  // One only tab
  int genTab = addPreference( tr( "PREF_TAB_GENERAL" ) );

  // One only group
  int defaultsGroup = addPreference( tr( "PREF_GROUP_DEFAULTS" ), genTab );
  
  // A checkbox
  addPreference( tr( "PREF_DEFAULT_BOOL" ), defaultsGroup, LightApp_Preferences::Bool, COMPONENT_NAME, "default_bool" );
  
  // An entry for integer
  addPreference( tr( "PREF_DEFAULT_INTEGER" ), defaultsGroup, LightApp_Preferences::Integer, COMPONENT_NAME, "default_integer" );

  // An integer changed by spinbox
  int spinInt = addPreference( tr( "PREF_DEFAULT_SPININT" ), defaultsGroup, LightApp_Preferences::IntSpin, COMPONENT_NAME, "default_spinint" );
  setPreferenceProperty( spinInt, "min", 0 );
  setPreferenceProperty( spinInt, "max", 20 );
  setPreferenceProperty( spinInt, "step", 2 );

  // A Double changed by spinbox
  int spinDbl = addPreference( tr( "PREF_DEFAULT_SPINDBL" ), defaultsGroup, LightApp_Preferences::DblSpin, COMPONENT_NAME, "default_spindbl" );
  setPreferenceProperty( spinDbl, "min", 1 );
  setPreferenceProperty( spinDbl, "max", 10 );
  setPreferenceProperty( spinDbl, "step", 0.1 );

  // A choice in a list
  int options = addPreference( tr( "PREF_DEFAULT_SELECTOR" ), defaultsGroup, LightApp_Preferences::Selector, COMPONENT_NAME, "default_selector" );
  QValueList<QVariant> indices;
  indices.append( 0 );
  indices.append( 1 );
  indices.append( 2 );
  setPreferenceProperty( options, "strings", selector_strings );
  setPreferenceProperty( options, "indexes", indices );
}

void HXX2SALOME_GENERIC_CLASS_NAMEGUI::preferencesChanged( const QString& sect, const QString& name )
{
// ****** This is normal way: Not yet called back at 12/12/05 ******
  SUIT_ResourceMgr* mgr = SUIT_Session::session()->resourceMgr();
  if( sect==COMPONENT_NAME )
  {
    if( name=="default_bool" )
	default_bool = mgr->booleanValue(COMPONENT_NAME, "default_bool", false);
    if( name=="default_integer" )
	default_int = mgr->integerValue(COMPONENT_NAME, "default_integer", 3);
    if( name=="default_spinint" )
	default_spinInt = mgr->integerValue(COMPONENT_NAME, "default_spinint", 4);
    if( name=="default_spindbl" )
	default_spinDbl = mgr->doubleValue(COMPONENT_NAME, "default_spindbl", 4.5);
    if( name=="default_selector" )
    {
  	int selectorIndex = mgr->integerValue(COMPONENT_NAME, "default_selector");
  	default_selection = (0<=selectorIndex && selectorIndex<=selector_strings.count() ? selector_strings[selectorIndex]: QString("None"));
    }
  }
}

// Export the module
extern "C" {
  CAM_Module* createModule()
  {
    return new HXX2SALOME_GENERIC_CLASS_NAMEGUI();
  }
}
