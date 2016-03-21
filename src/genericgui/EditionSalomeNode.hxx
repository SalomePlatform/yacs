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

#ifndef _EDITIONSALOMENODE_HXX_
#define _EDITIONSALOMENODE_HXX_

#include "EditionElementaryNode.hxx"
#include "FormContainerDecorator.hxx"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

class FormComponent;
class FormContainer;

namespace YACS
{
  namespace ENGINE
  {
    class ServiceNode;
  }

  namespace HMI
  {
    class PropertyEditor;
      // ports en cablage plus initialisation seulement
      // choix de l'instance de composant
      // panneau depliable instance de composant
      // panneau depliable container

    class EditionSalomeNode: public EditionElementaryNode
    {
      Q_OBJECT

    public:
      EditionSalomeNode(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionSalomeNode();
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual void synchronize();

    public slots:
      virtual void fillComponentPanel();
      virtual void fillContainerPanel();
      virtual void changeInstance(int index);
      virtual void changeContainer(int index);
      virtual void changeHost(int index);
      virtual void onApply();
      virtual void onCancel();

    protected:
      PropertyEditor* _propeditor;
      FormComponent *_wComponent;
      FormContainerDecorator *_wContainer;
      QHBoxLayout *_hbl_method;
      QLabel *_la_method;
      QLineEdit *_le_method;
      YACS::ENGINE::ServiceNode* _servNode;
    };
  }
}
#endif
