// Copyright (C) 2006-2026  CEA, EDF
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

#pragma once

#include "YACSBasesExport.hxx"
#include "Exception.hxx"

#include <iostream>
#include <sstream>

#ifdef _DEVDEBUG_
#define DEBTRACE(msg) { std::cerr << std::flush << __FILE__ << " [" << __LINE__ << "] : " << msg << std::endl << std::flush; }
#else
#define DEBTRACE(msg)
#endif

namespace YACS
{
  extern YACSBASES_EXPORT int traceLevel;
  void AppendTimeClock(std::ostream& os);
}

//! YACSTRACE macro for dynamic trace: print only if YACS_TRACELEVEL environment variable is set and level is less than  its value
#define YACSTRACE(level,msg) { if(YACS::traceLevel >=level)                                  \
  {                                                                                          \
    AppendTimeClock(std::cerr);                                                              \
    std::cerr << __FILE__ << " [" << __LINE__ << "] : " << msg << std::endl;                 \
  } }

#define ERROR_YACSTRACE(msg) YACSTRACE(0,msg)   
   
#define WARNING_YACSTRACE(msg) YACSTRACE(1,msg)

#define INFO_YACSTRACE(msg) YACSTRACE(2,msg)

#define DEBUG_YACSTRACE(msg) YACSTRACE(7,msg)

#define GURU_YACSTRACE(msg) YACSTRACE(99,msg)

//! YASSERT macro is always defined, used like assert, but throw a YACS::Exception instead of abort

#define YASSERT(val) { if( !(val) ) { std::ostringstream mess; mess << __FILE__ << " [" <<__LINE__<< "] : assertion " << #val << " failed"; throw YACS::Exception(mess.str()); } }

void AttachDebugger();
