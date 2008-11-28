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
#ifndef YACSPRS_SERVICENODE_H
#define YACSPRS_SERVICENODE_H

#include <Python.h>
#include <omniORB4/CORBA.h>
#include <qstring.h>
#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class Any;
    class TypeCode;
  }
}

char * toString(CORBA::Any* theAny, QString& theRetStr);
void toString(PyObject* theObject, QString& theRetStr);
void toString(YACS::ENGINE::Any* theAny, QString& theValue);
void toString(const std::string& value, const YACS::ENGINE::TypeCode * t,QString& theRetStr);
#endif
