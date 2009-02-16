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
#ifndef _EDITIONSCRIPT_HXX_
#define _EDITIONSCRIPT_HXX_

#include "yacsconfig.h"
#include "EditionElementaryNode.hxx"

#if HAS_QSCI4>0
class QsciScintilla;
#endif

namespace YACS
{
  namespace HMI
  {
    class EditionScript: public EditionElementaryNode
    {
      Q_OBJECT

    public slots:
      virtual void onApply();
      virtual void onCancel();
      virtual void onScriptModified();

    public:
      EditionScript(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionScript();

    protected:
      SubjectInlineNode *_subInlineNode;

#if HAS_QSCI4>0
      QsciScintilla* _sci;
#else
      QTextEdit* _sci;
#endif

    };
  }
}
#endif