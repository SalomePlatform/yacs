// Copyright (C) 2006-2024  CEA, EDF
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

#ifndef _EDITIONCONTAINER_HXX_
#define _EDITIONCONTAINER_HXX_

#include "ItemEdition.hxx"

class FormContainerDecorator;

namespace YACS
{
  namespace HMI
  {

    class EditionContainer: public ItemEdition
    {
      Q_OBJECT

    public:
      EditionContainer(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionContainer();
      virtual void update(GuiEvent event, int type, Subject* son);

    public slots:
      virtual void fillContainerPanel();
      virtual void onApply();
      virtual void onCancel();

    private:

      YACS::ENGINE::Container *getContainer();

    protected:
      FormContainerDecorator *_wContainer;
    };
  }
}
#endif
