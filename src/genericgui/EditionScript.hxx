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

#ifndef _EDITIONSCRIPT_HXX_
#define _EDITIONSCRIPT_HXX_

#include "yacsconfig.h"
#include "EditionElementaryNode.hxx"
#include "FormContainerDecorator.hxx"
#include "FormComponent.hxx"
#include <QRadioButton>

class FormContainer;

#ifdef HAS_PYEDITOR
class PyEditor_Editor;
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
      virtual void onEdit();
      virtual void on_tb_options_toggled(bool checked);
      virtual void on_remote_toggled(bool checked);
      virtual void fillContainerPanel();
      virtual void changeContainer(int);
      virtual void update(GuiEvent event, int type, Subject* son);

    public:
      EditionScript(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionScript();
      virtual void synchronize();

    protected:
      SubjectInlineNode *_subInlineNode;

#ifdef HAS_PYEDITOR
      PyEditor_Editor* _sci;
#else
      QTextEdit* _sci;
#endif
      QVBoxLayout* _glayout;
      QVBoxLayout* _portslayout;
      QPushButton* _editor;
      bool _checked;
      bool _remote;
      QFrame *fr_options;
      QFrame *fr_container;
      ComboBox* cb_container;
      QRadioButton* radiolocal;
      QRadioButton* radioremote;
      FormContainerDecorator* formcontainer;
    };
  }
}
#endif
