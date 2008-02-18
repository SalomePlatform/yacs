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

// #include <Python.h>
// #include <PythonNode.hxx>

// #include <SALOME_ResourcesManager.hxx>
// #include <SALOME_LifeCycleCORBA.hxx>

#include <YACSGui_InputPanel.h>
#include <YACSGui_Module.h>
// #include <YACSGui_PlusMinusGrp.h>
// #include <YACSGui_Table.h>
// #include <YACSGui_Graph.h>
// #include <YACSGui_Executor.h>
// #include <YACSGui_TreeView.h>
// #include <YACSGui_TreeViewItem.h>
// #include <YACSGui_LogViewer.h>

// #include <YACSPrs_ElementaryNode.h>

// #include <QxGraph_Canvas.h>

// #include <LightApp_Application.h>
// #include <CAM_Application.h>
// #include <SUIT_Session.h>
// #include <SUIT_Application.h>
// #include <SUIT_MessageBox.h>
// #include <LogWindow.h>

// #include <InPort.hxx>
// #include <OutPort.hxx>
// #include <InputPort.hxx>
// #include <OutputPort.hxx>
// #include <CalStreamPort.hxx>
// #include <ConditionInputPort.hxx>
// #include <ForLoop.hxx>
// #include <ForEachLoop.hxx>
// #include <WhileLoop.hxx>
// #include <Switch.hxx>
// #include <CORBAPorts.hxx>
// #include <PythonPorts.hxx>
// #include <XMLPorts.hxx>
// #include <InlineNode.hxx>
// #include <ServiceNode.hxx>
// #include <ServiceInlineNode.hxx>
// #include <CORBANode.hxx>
// #include <SalomePythonNode.hxx>
// #include <CppNode.hxx>
// #include <XMLNode.hxx>
// #include <Exception.hxx>
// #include <Catalog.hxx>
// #include <ComponentDefinition.hxx>
// #include <CORBAComponent.hxx>
// #include <CppComponent.hxx>
// #include <SalomeComponent.hxx>
// #include <SalomePythonComponent.hxx>
// #include <RuntimeSALOME.hxx>

// #include <qwindowsstyle.h>
// #include <qpushbutton.h>
// #include <qlayout.h>
// #include <qhbox.h>
// #include <qlabel.h>
// #include <qlineedit.h>
// #include <qbuttongroup.h>
// #include <qradiobutton.h>
// #include <qcombobox.h>
// #include <qtextedit.h>
// #include <qtoolbutton.h>
// #include <qspinbox.h>
// #include <qprogressbar.h>
// #include <qobjectlist.h>
// #include <qfiledialog.h>
// #include <qcheckbox.h>
#include <qframe.h>

// #include <sstream>

// #define SPACING 5
// #define MARGIN 5

using namespace YACS;
using namespace YACS::ENGINE;
using namespace YACS::HMI;
using namespace std;


/*
  Class : YACSGui_PropertyPage_InputPage
  Description : This class is used for parameters of input panel.
                All pages of input panel must be derived from it
*/

YACSGui_PropertyPage::YACSGui_PropertyPage( QWidget* theParent, YACSGui_Module* theModule )
  : QFrame( theParent ),
    myModule(theModule),
    myView(0)
{
  setLineWidth(2);
  setFrameStyle(QFrame::StyledPanel|QFrame::Raised);
}

YACSGui_PropertyPage::~YACSGui_PropertyPage()
{
}

void YACSGui_PropertyPage::onApply()
{
}

