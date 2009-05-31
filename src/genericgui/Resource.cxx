//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
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

#include "Resource.hxx"

using namespace YACS::HMI;

// Statics for general resources
// -----------------------------

bool Resource::COMPONENT_INSTANCE_NEW = COMPONENTINSTANCENEW;
QString Resource::userCatalog = USERCATALOG;

// Statics for color of states
// ---------------------------

QColor Resource::editedNodeBrushColor = EDITEDNODEBRUSHCOLOR;
QColor Resource::normalNodeBrushColor = NORMALNODEBRUSHCOLOR;
QColor Resource::runNodeBrushColor    = RUNNODEBRUSHCOLOR;
QColor Resource::validNodeColor       = VALIDNODECOLOR;
QColor Resource::invalidNodeColor     = INVALIDNODECOLOR;

QColor Resource::NOTYETINITIALIZED    = NOTYETINITIALIZED_;
QColor Resource::INITIALISED          = INITIALISED_;
QColor Resource::RUNNING              = RUNNING_;
QColor Resource::WAITINGTASKS         = WAITINGTASKS_;
QColor Resource::PAUSED               = PAUSED_;
QColor Resource::FINISHED             = FINISHED_;
QColor Resource::STOPPED              = STOPPED_;
QColor Resource::UNKNOWN              = UNKNOWN_;

QColor Resource::UNDEFINED            = QColor();
QColor Resource::INVALID              = QColor();
QColor Resource::READY                = QColor();
QColor Resource::TOLOAD               = QColor();
QColor Resource::LOADED               = QColor();
QColor Resource::TOACTIVATE           = QColor();
QColor Resource::ACTIVATED            = QColor();
QColor Resource::DESACTIVATED         = QColor();
QColor Resource::DONE                 = QColor();
QColor Resource::SUSPENDED            = QColor();
QColor Resource::LOADFAILED           = QColor();
QColor Resource::EXECFAILED           = QColor();
QColor Resource::PAUSE                = QColor();
QColor Resource::INTERNALERR          = QColor();
QColor Resource::DISABLED             = QColor();
QColor Resource::FAILED               = QColor();
QColor Resource::ERROR                = QColor();
QColor Resource::DEFAULT              = QColor();

// Statics for color of links
// --------------------------

QFont  Resource::pythonfont = PYTHONFONT;

QColor Resource::link_draw_color           = LINKDRAW_COLOR;
QColor Resource::stream_link_draw_color    = STREAMLINKDRAW_COLOR;
QColor Resource::stream_link_select_color  = STREAMLINK_SELECT_COLOR;
QColor Resource::link_select_color         = LINK_SELECT_COLOR;
QColor Resource::control_link_select_color = CTRLLINK_SELECT_COLOR;
QColor Resource::control_link_draw_color   = CTRLLINKDRAW_COLOR;

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

QColor Resource::dragOver               = dragOver_;

// Statics for node parameters
// ---------------------------

int Resource::CtrlPort_Width  = CtrlPort_Width_ ;
int Resource::CtrlPort_Height = CtrlPort_Height_;
int Resource::DataPort_Width  = DataPort_Width_ ;
int Resource::DataPort_Height = DataPort_Height_;

// Constructor
// -----------

Resource::Resource() {
}
