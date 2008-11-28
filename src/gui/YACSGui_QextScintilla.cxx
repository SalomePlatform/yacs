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
#include "yacs_config.h"
#include "YACSGui_QextScintilla.h"

#include <qevent.h>

#ifdef HAVE_QEXTSCINTILLA_H

YACSGui_QextScintilla::YACSGui_QextScintilla(QWidget *parent, const char *name, WFlags f)
  : QextScintilla(parent, name, f)
{
}

YACSGui_QextScintilla::~YACSGui_QextScintilla()
{
}

bool YACSGui_QextScintilla::event(QEvent *e)
{
  if (e->type() == QEvent::AccelOverride)
    {
      QKeyEvent *ke = (QKeyEvent*)e;
      if ( ((ke->state() == Qt::NoButton) || (ke->state() == Qt::ShiftButton))
           && (ke->key() != Qt::Key_Escape))
        {
          ke->accept();
          return true;
        }
    }
  return QextScintilla::event(e);
}

#endif
