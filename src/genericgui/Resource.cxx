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

#include "Resource.hxx"

using namespace YACS::HMI;

// Statics for general resources
// -----------------------------

bool Resource::COMPONENT_INSTANCE_NEW = COMPONENTINSTANCENEW;
QString Resource::userCatalog = USERCATALOG;
QString Resource::pythonExternalEditor = PYTHONEXTERNALEDITOR;
bool Resource::addRowCols = ADDROWCOLS;
bool Resource::straightLinks = STRAIGHTLINKS;
bool Resource::autoComputeLinks = AUTOCOMPUTELINKS;
bool Resource::simplifyLink = SIMPLIFYLINK;
bool Resource::ensureVisibleWhenMoved = ENSUREVISIBLEWHENMOVED;
int Resource::dockWidgetPriority = DOCKWIDGETPRIORITY;
QColor Resource::progressBarColor = PROGRESSBARCOLOR;
int Resource::progressBarLabel = PROGRESSBARLABEL;
QFont  Resource::pythonfont = PYTHONFONT;

// Statics for color of states
// ---------------------------

QColor Resource::editedNodeBrushColor = EDITEDNODEBRUSHCOLOR;
QColor Resource::normalNodeBrushColor = NORMALNODEBRUSHCOLOR;
QColor Resource::runNodeBrushColor    = RUNNODEBRUSHCOLOR;
QColor Resource::validNodeColor       = VALIDNODECOLOR;
QColor Resource::invalidNodeColor     = INVALIDNODECOLOR;

QColor Resource::emphasizeBrushColor  = EMPHASIZEBRUSHCOLOR;

QColor Resource::NOTYETINITIALIZED    = NOTYETINITIALIZED_;
QColor Resource::INITIALISED          = INITIALISED_;
QColor Resource::RUNNING              = RUNNING_;
QColor Resource::WAITINGTASKS         = WAITINGTASKS_;
QColor Resource::PAUSED               = PAUSED_;
QColor Resource::FINISHED             = FINISHED_;
QColor Resource::STOPPED              = STOPPED_;
QColor Resource::UNKNOWN              = UNKNOWN_;

QColor Resource::UNDEFINED            = UNDEFINED_;
QColor Resource::INVALID              = INVALID_;
QColor Resource::READY                = READY_;
QColor Resource::TOLOAD               = TOLOAD_;
QColor Resource::LOADED               = LOADED_;
QColor Resource::TOACTIVATE           = TOACTIVATE_;
QColor Resource::ACTIVATED            = ACTIVATED_;
QColor Resource::DESACTIVATED         = DESACTIVATED_;
QColor Resource::DONE                 = DONE_;
QColor Resource::SUSPENDED            = SUSPENDED_;
QColor Resource::LOADFAILED           = LOADFAILED_;
QColor Resource::EXECFAILED           = EXECFAILED_;
QColor Resource::PAUSE                = PAUSE_;
QColor Resource::INTERNALERR          = INTERNALERR_;
QColor Resource::DISABLED             = DISABLED_;
QColor Resource::FAILED               = FAILED_;
QColor Resource::ERROR                = ERROR_;
QColor Resource::DEFAULT              = DEFAULT_;

// Statics for color of links
// --------------------------

QColor Resource::link_draw_color           = LINKDRAW_COLOR;
QColor Resource::stream_link_draw_color    = STREAMLINKDRAW_COLOR;
QColor Resource::stream_link_select_color  = STREAMLINK_SELECT_COLOR;
QColor Resource::link_select_color         = LINK_SELECT_COLOR;
QColor Resource::control_link_select_color = CTRLLINK_SELECT_COLOR;
QColor Resource::control_link_draw_color   = CTRLLINKDRAW_COLOR;

int Resource::link_pen_darkness            = LINK_PEN_DARKNESS;
int Resource::link_separation_weight       = LINK_SEPARATION_WEIGHT;
double Resource::link_thickness            = LINK_THICKNESS;

// Statics for color of nodes
// --------------------------

QColor Resource::Scene_pen              = Scene_pen_;
QColor Resource::Scene_hiPen            = Scene_hiPen_;
QColor Resource::Scene_brush            = Scene_brush_;
QColor Resource::Scene_hiBrush          = Scene_hiBrush_;
QColor Resource::ComposedNode_brush     = ComposedNode_brush_;
QColor Resource::ComposedNode_hiBrush   = ComposedNode_hiBrush_;
QColor Resource::ComposedNode_pen       = ComposedNode_pen_;
QColor Resource::ComposedNode_hiPen     = ComposedNode_hiPen_;
QColor Resource::ElementaryNode_brush   = ElementaryNode_brush_;
QColor Resource::ElementaryNode_hiBrush = ElementaryNode_hiBrush_;
QColor Resource::ElementaryNode_pen     = ElementaryNode_pen_;
QColor Resource::ElementaryNode_hiPen   = ElementaryNode_hiPen_;
QColor Resource::Header_brush           = Header_brush_;
QColor Resource::Header_hiBrush         = Header_hiBrush_;
QColor Resource::Header_pen             = Header_pen_;
QColor Resource::Header_hiPen           = Header_hiPen_;
QColor Resource::CtrlPort_brush         = CtrlPort_brush_;
QColor Resource::CtrlPort_hiBrush       = CtrlPort_hiBrush_;
QColor Resource::CtrlPort_pen           = CtrlPort_pen_;
QColor Resource::CtrlPort_hiPen         = CtrlPort_hiPen_;
QColor Resource::DataPort_brush         = DataPort_brush_;
QColor Resource::DataPort_hiBrush       = DataPort_hiBrush_;
QColor Resource::DataPort_pen           = DataPort_pen_;
QColor Resource::DataPort_hiPen         = DataPort_hiPen_;
QColor Resource::DataStreamPort_brush         = DataStreamPort_brush_;
QColor Resource::DataStreamPort_hiBrush       = DataStreamPort_hiBrush_;
QColor Resource::DataStreamPort_pen           = DataStreamPort_pen_;
QColor Resource::DataStreamPort_hiPen         = DataStreamPort_hiPen_;

QColor Resource::dragOver               = dragOver_;

// Statics for geometry values for nodes
// -------------------------------------

int Resource::CtrlPort_Width  = 24;
int Resource::CtrlPort_Height = 20;
int Resource::DataPort_Width  = 72;
int Resource::DataPort_Height = 25;

int Resource::Thickness       =  1;

int Resource::Border_Margin   =  4;
int Resource::Space_Margin    =  2;

int Resource::Line_Space      =  2;
int Resource::Line_Width      =  2;

int Resource::Text_DX         =  3;

int Resource::Corner_Margin   = ( Resource::Border_Margin + Resource::Space_Margin );
int Resource::Header_Height   = ( Resource::Corner_Margin + Resource::CtrlPort_Height + Resource::Line_Space + Resource::Line_Width + Resource::Line_Space );
int Resource::Radius          =  3;

int Resource::progressBar_Height =  20;

bool Resource::tabPanelsUp = TABPANELSUP;

// Constructor
// -----------

Resource::Resource()
{
}
