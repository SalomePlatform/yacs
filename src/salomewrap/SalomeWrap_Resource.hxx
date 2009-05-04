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
#ifndef _SALOMEWRAP_RESOURCE_HXX_
#define _SALOMEWRAP_RESOURCE_HXX_

#include <QString>
#include <QFont>
#include <QColor>

class SUIT_ResourceMgr;

class SalomeWrap_Resource {
  public:
    SalomeWrap_Resource(SUIT_ResourceMgr* r);

    int     integerValue( const QString& name, const int     def = 0         ) const;
    double  doubleValue ( const QString& name, const double  def = 0         ) const;
    bool    booleanValue( const QString& name, const bool    def = false     ) const;
    QFont   fontValue   ( const QString& name, const QFont   def = QFont()   ) const;
    QColor  colorValue  ( const QString& name, const QColor  def = QColor()  ) const;
    QString stringValue ( const QString& name, const QString def = QString() ) const;

  private:
    SUIT_ResourceMgr* _resource;
};

#endif
