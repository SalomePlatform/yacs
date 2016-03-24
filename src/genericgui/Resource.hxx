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

#ifndef _RESOURCE_HXX_
#define _RESOURCE_HXX_

#include "GenericGuiExport.hxx"

#include <QColor>
#include <QFont>

// Define the general names of the resources
// -----------------------------------------

#define _COMPONENT_INSTANCE_NEW "COMPONENT_INSTANCE_NEW"

// Define the names of the resources for colors
// --------------------------------------------

#define _editedNodeBrushColor "editedNodeBrushColor"
#define _normalNodeBrushColor "normalNodeBrushColor"
#define _runNodeBrushColor    "runNodeBrushColor"
#define _validNodeColor       "validNodeColor"
#define _invalidNodeColor     "invalidNodeColor"

#define _NOTYETINITIALIZED    "NOTYETINITIALIZED"
#define _INITIALISED          "INITIALISED"
#define _RUNNING              "RUNNING"
#define _WAITINGTASKS         "WAITINGTASKS"
#define _PAUSED               "PAUSED"
#define _FINISHED             "FINISHED"
#define _STOPPED              "STOPPED"
#define _UNKNOWN              "UNKNOWN"

#define _UNDEFINED            "UNDEFINED"
#define _INVALID              "INVALID"
#define _READY                "READY"
#define _TOLOAD               "TOLOAD"
#define _LOADED               "LOADED"
#define _TOACTIVATE           "TOACTIVATE"
#define _ACTIVATED            "ACTIVATED"
#define _DESACTIVATED         "DESACTIVATED"
#define _DONE                 "DONE"
#define _SUSPENDED            "SUSPENDED"
#define _LOADFAILED           "LOADFAILED"
#define _EXECFAILED           "EXECFAILED"
#define _PAUSE                "PAUSE"
#define _INTERNALERR          "INTERNALERR"
#define _DISABLED             "DISABLED"
#define _FAILED               "FAILED"
#define _ERROR                "ERROR"
#define _DEFAULT              "DEFAULT"

#define _Scene_pen              "Scene_pen"
#define _Scene_hiPen            "Scene_hiPen"
#define _Scene_brush            "Scene_brush"
#define _Scene_hiBrush          "Scene_hiBrush"
#define _ComposedNode_brush     "ComposedNode_brush"
#define _ComposedNode_hiBrush   "ComposedNode_hiBrush"
#define _ComposedNode_pen       "ComposedNode_pen"
#define _ComposedNode_hiPen     "ComposedNode_hiPen"
#define _ElementaryNode_brush   "ElementaryNode_brush"
#define _ElementaryNode_hiBrush "ElementaryNode_hiBrush"
#define _ElementaryNode_pen     "ElementaryNode_pen"
#define _ElementaryNode_hiPen   "ElementaryNode_hiPen"
#define _Header_brush           "Header_brush"
#define _Header_hiBrush         "Header_hiBrush"
#define _Header_pen             "Header_pen"
#define _Header_hiPen           "Header_hiPen"
#define _CtrlPort_brush         "CtrlPort_brush"
#define _CtrlPort_hiBrush       "CtrlPort_hiBrush"
#define _CtrlPort_pen           "CtrlPort_pen"
#define _CtrlPort_hiPen         "CtrlPort_hiPen"
#define _DataPort_brush         "DataPort_brush"
#define _DataPort_hiBrush       "DataPort_hiBrush"
#define _DataPort_pen           "DataPort_pen"
#define _DataPort_hiPen         "DataPort_hiPen"
#define _DataStreamPort_brush         "DataStreamPort_brush"
#define _DataStreamPort_hiBrush       "DataStreamPort_hiBrush"
#define _DataStreamPort_pen           "DataStreamPort_pen"
#define _DataStreamPort_hiPen         "DataStreamPort_hiPen"

#define __dragOver       "dragOver"

// Default values
// --------------

#define COMPONENTINSTANCENEW    true
#define PYTHONFONT              QFont("Courier")
#define PYTHONEXTERNALEDITOR    ""
#define USERCATALOG             "YACSUserCatalog.xml"
#define ADDROWCOLS              true
#define STRAIGHTLINKS           false
#define AUTOCOMPUTELINKS        true
#define SIMPLIFYLINK            true
#define ENSUREVISIBLEWHENMOVED  true
#define TABPANELSUP             true
#define DOCKWIDGETPRIORITY      0
#define PROGRESSBARCOLOR        Qt::darkGreen
#define PROGRESSBARLABEL        2

#define EDITEDNODEBRUSHCOLOR    QColor(255, 255, 190)
#define NORMALNODEBRUSHCOLOR    QColor(230, 235, 255)
#define RUNNODEBRUSHCOLOR       QColor(205, 218, 255)
#define VALIDNODECOLOR          QColor(128, 255, 128)
#define INVALIDNODECOLOR        QColor(255, 128, 128)

#define EMPHASIZEBRUSHCOLOR     QColor(  0, 255,   0)

#define NOTYETINITIALIZED_      QColor(255, 243, 205)
#define INITIALISED_            QColor(230, 255, 205)
#define RUNNING_                QColor(205, 255, 218)
#define WAITINGTASKS_           QColor(205, 255, 255)
#define PAUSED_                 QColor(205, 218, 255)
#define FINISHED_               QColor(230, 205, 255)
#define STOPPED_                QColor(255, 205, 243)
#define UNKNOWN_                QColor(255, 205, 205)

#define UNDEFINED_      Qt::lightGray
#define INVALID_        Qt::red
#define READY_          Qt::gray
#define TOLOAD_         Qt::darkYellow
#define LOADED_         Qt::darkMagenta
#define TOACTIVATE_     Qt::darkCyan
#define ACTIVATED_      Qt::darkBlue
#define DESACTIVATED_   Qt::gray
#define DONE_           Qt::darkGreen
#define SUSPENDED_      Qt::gray
#define LOADFAILED_     QColor(255, 0, 170)
#define EXECFAILED_     QColor(255, 85, 0)
#define PAUSE_          QColor(0, 255, 255)
#define INTERNALERR_    QColor(255, 0, 85)
#define DISABLED_       QColor(255, 170, 0)
#define FAILED_         QColor(170,  85, 0)
#define ERROR_          QColor(255, 0, 0)
#define DEFAULT_        Qt::lightGray

#define LINKDRAW_COLOR            QColor(  0,   0, 192)
#define LINK_SELECT_COLOR         QColor(192, 192, 255)
#define STREAMLINKDRAW_COLOR      QColor(192, 192,   0)
#define STREAMLINK_SELECT_COLOR   QColor(255, 255, 192)
#define CTRLLINKDRAW_COLOR        QColor(192,   0, 192)
#define CTRLLINK_SELECT_COLOR     QColor(255, 192, 255)
#define LINK_PEN_DARKNESS         150
#define LINK_THICKNESS            1.0
#define LINK_SEPARATION_WEIGHT    10

#define Scene_pen_              QColor(  0,   0, 128)
#define Scene_hiPen_            QColor(  0,   0, 190)
#define Scene_brush_            QColor(128, 128, 128)
#define Scene_hiBrush_          QColor(190, 190, 190)
#define ComposedNode_brush_     QColor(213, 213, 213)
#define ComposedNode_hiBrush_   QColor(225, 225, 225)
#define ComposedNode_pen_       QColor(120, 120, 120)
#define ComposedNode_hiPen_     QColor( 60,  60,  60)
#define ElementaryNode_brush_   QColor(189, 230, 185)
#define ElementaryNode_hiBrush_ QColor(209, 255, 205)
#define ElementaryNode_pen_     QColor( 15, 180,   0)
#define ElementaryNode_hiPen_   QColor( 11, 128,   0)
#define Header_brush_           QColor(215, 220, 238)
#define Header_hiBrush_         QColor(161, 178, 238)
#define Header_pen_             QColor(120, 120, 120)
#define Header_hiPen_           QColor( 60,  60,  60)
#define CtrlPort_brush_         QColor(205, 210, 227)
#define CtrlPort_hiBrush_       QColor(161, 176, 227)
#define CtrlPort_pen_           QColor(120, 120, 120)
#define CtrlPort_hiPen_         QColor( 60,  60,  60)
#define DataPort_brush_         QColor(158, 227, 151)
#define DataPort_hiBrush_       QColor(127, 227, 116)
#define DataPort_pen_           QColor( 15, 180,   0)
#define DataPort_hiPen_         QColor( 11, 128,   0)
#define DataStreamPort_brush_   QColor(158, 227, 151)
#define DataStreamPort_hiBrush_ QColor(127, 227, 116)
#define DataStreamPort_pen_     QColor( 15, 180,   0)
#define DataStreamPort_hiPen_   QColor( 11, 128,   0)

#define dragOver_               QColor(255,   0,   0)

// Define Resource class
// ---------------------

namespace YACS {
  namespace HMI {
    class GENERICGUI_EXPORT Resource {
      public:
        Resource();

      public:
        // General resource
        static bool COMPONENT_INSTANCE_NEW;
        static QString userCatalog;
        static QString pythonExternalEditor;
        static bool addRowCols;
        static bool straightLinks;
        static bool autoComputeLinks;
        static bool simplifyLink;
        static bool ensureVisibleWhenMoved;
        static int  dockWidgetPriority;
        static QColor progressBarColor;
        static int    progressBarLabel;

        // Colors of state of nodes
        static QColor editedNodeBrushColor;
        static QColor normalNodeBrushColor;
        static QColor runNodeBrushColor;
        static QColor validNodeColor;
        static QColor invalidNodeColor;

        static QColor emphasizeBrushColor;

        static QColor NOTYETINITIALIZED;
        static QColor INITIALISED;
        static QColor RUNNING;
        static QColor WAITINGTASKS;
        static QColor PAUSED;
        static QColor FINISHED;
        static QColor STOPPED;
        static QColor UNKNOWN;

        static QColor UNDEFINED;
        static QColor INVALID;
        static QColor READY;
        static QColor TOLOAD;
        static QColor LOADED;
        static QColor TOACTIVATE;
        static QColor ACTIVATED;
        static QColor DESACTIVATED;
        static QColor DONE;
        static QColor SUSPENDED;
        static QColor LOADFAILED;
        static QColor EXECFAILED;
        static QColor PAUSE;
        static QColor INTERNALERR;
        static QColor DISABLED;
        static QColor FAILED;
        static QColor ERROR;
        static QColor DEFAULT;

        // Python font
        static QFont  pythonfont;

        // Link colors
        static QColor link_draw_color;
        static QColor stream_link_draw_color;
        static QColor link_select_color;
        static QColor stream_link_select_color;
        static QColor control_link_draw_color;
        static QColor control_link_select_color;

        static int link_pen_darkness;
        static int link_separation_weight;
        static double link_thickness;

        // Node colors
        static QColor Scene_pen;
        static QColor Scene_hiPen;
        static QColor Scene_brush;
        static QColor Scene_hiBrush;
        static QColor ComposedNode_brush;
        static QColor ComposedNode_hiBrush;
        static QColor ComposedNode_pen;
        static QColor ComposedNode_hiPen;
        static QColor ElementaryNode_brush;
        static QColor ElementaryNode_hiBrush;
        static QColor ElementaryNode_pen;
        static QColor ElementaryNode_hiPen;
        static QColor Header_brush;
        static QColor Header_hiBrush;
        static QColor Header_pen;
        static QColor Header_hiPen;
        static QColor CtrlPort_brush;
        static QColor CtrlPort_hiBrush;
        static QColor CtrlPort_pen;
        static QColor CtrlPort_hiPen;
        static QColor DataPort_brush;
        static QColor DataPort_hiBrush;
        static QColor DataPort_pen;
        static QColor DataPort_hiPen;
        static QColor DataStreamPort_brush;
        static QColor DataStreamPort_hiBrush;
        static QColor DataStreamPort_pen;
        static QColor DataStreamPort_hiPen;

        static QColor dragOver;

        // Node parameters
        static int CtrlPort_Width;
        static int CtrlPort_Height;
        static int DataPort_Width;
        static int DataPort_Height;

        static int Thickness;

        static int Border_Margin;
        static int Space_Margin;

        static int Line_Space;
        static int Line_Width;

        static int Text_DX;

        static int Corner_Margin;
        static int Header_Height;
        static int Radius;

        static int progressBar_Height;

        static bool tabPanelsUp;
    };
  }
}

#endif
