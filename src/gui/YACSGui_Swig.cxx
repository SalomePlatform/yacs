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

#include <YACSGui_Swig.h>

#include <YACSGui_Module.h>
#include <YACSGui_DataModel.h>
#include <YACSGui_XMLDriver.h>

#include <QxGraph_ViewModel.h>
#include <QxGraph_ViewWindow.h>
#include <QxGraph_ViewManager.h>

#include <SUIT_Session.h>
#include <CAM_Application.h>
#include <SalomeApp_Application.h>

#include "SALOME_Event.hxx"

#include <guiContext.hxx>

using namespace YACS::ENGINE;
using namespace YACS::HMI;

YACSGui_Swig::YACSGui_Swig()
{
}

YACSGui_Swig::~YACSGui_Swig()
{
}

void YACSGui_Swig::displayGraph(YACS::ENGINE::Proc* theProc)
{
  // create presentation for theProc ( note: process this GUI update only in the main GUI thread with help of SALOME_Event )
  ProcessVoidEvent( new TVoidMemFun1ArgEvent<YACSGui_Swig,YACS::ENGINE::Proc*>( this, 
										&YACSGui_Swig::createGraphPresentation,
										theProc ) );
}

//! Creates the view window and presentation of a the calculation graph.
/*!
 * Used if graph is imported from old XML file format.
 * Called from YACSGui_Swig::displayGraph(YACS::ENGINE::Proc* theProc) method
 * with help of SALOME_Event.
 *
 *  \param theProc - engine Proc filled by the content of the XML file.
 */
void YACSGui_Swig::createGraphPresentation( YACS::ENGINE::Proc* theProc )
{
  // get YACSGui module
  CAM_Application* anApp = ( CAM_Application* )(SUIT_Session::session()->activeApplication());
  YACSGui_Module* aModule = dynamic_cast<YACSGui_Module*>( anApp->module( anApp->moduleTitle( "YACSGui" ) ) );
  if ( !aModule ) return;
  
  // create view window for presentation of theProc
  SUIT_ViewManager* aVM  = aModule->getApp()->createViewManager( QxGraph_Viewer::Type() );

  // set caption for graph's view window
  QxGraph_ViewWindow* aVW = dynamic_cast<QxGraph_ViewWindow*>( aVM->getActiveView() );
  if ( aVW ) aVW->setCaption(theProc->getName().c_str());

  // add theProc to data model as not editable graph

  /*abd
    YACSGui_DataModel* aModel = aModule->getDataModel();
  if (aModel)
  aModel->add(theProc, false);*/
  
  // create presentation for theProc (i.e. YACSGui_Graph, presentations of graph's nodes ...) and display it
  if ( aVW )
  {
    YACSGui_Loader::PrsDataMap aPrsData;
    YACSGui_Loader::PortLinkDataMap aPortLinkData;
    YACSGui_Loader::LabelLinkDataMap aLabelLinkData;

    // create GuiContext for Proc*
    GuiContext* aCProc = new GuiContext();
    GuiContext::setCurrent( aCProc );
    aCProc->setProc( theProc );
    
    aModule->displayGraph( aCProc, aPrsData, aPortLinkData, aLabelLinkData );
  }
}
