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

#ifndef _SALOMEGUI_RESOURCE_HXX_
#define _SALOMEGUI_RESOURCE_HXX_

#include <QColor>
#include <QFont>
#include <QString>

class Yacsgui;
class SUIT_ResourceMgr;

class Yacsgui_Resource
{
  public:
    Yacsgui_Resource(SUIT_ResourceMgr* r);
    virtual void createPreferences(Yacsgui* swm);
    virtual void preferencesChanged( const QString& sect, const QString& name );
    virtual void preferencesChanged();

  protected:
    int     integerValue( const QString& name, const int     def = 0          ) const;
    double  doubleValue ( const QString& name, const double  def = 0          ) const;
    bool    booleanValue( const QString& name, const bool    def = false      ) const;
    QFont   fontValue   ( const QString& name, const QFont&  def = QFont()    ) const;
    QColor  colorValue  ( const QString& name, const QColor& def = QColor()   ) const;
    QColor  colorValue  ( const QString& name, const Qt::GlobalColor c        ) const;
    QColor  colorValue  ( const QString& name, const int h, const int s, const int v) const;
    QString stringValue ( const QString& name, const QString& def = QString() ) const;

    void setValue( const QString& name, const int     val );
    void setValue( const QString& name, const double  val );
    void setValue( const QString& name, const bool    val );
    void setValue( const QString& name, const QFont&   val );
    void setValue( const QString& name, const QColor&  val );
    void setValue( const QString& name, const QString& val );

  protected:
    SUIT_ResourceMgr* _resource;

};

#endif

