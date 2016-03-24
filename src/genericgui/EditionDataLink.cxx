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

#include "EditionDataLink.hxx"
#include "PropertyEditor.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

EditionDataLink::EditionDataLink(Subject* subject,
                                 QWidget* parent,
                                 const char* name)
  : ItemEdition(subject, parent, name)
{
  SubjectLink * sub = dynamic_cast<SubjectLink*>(_subject);
  if(dynamic_cast<SubjectInputDataStreamPort*>(sub->getSubjectInPort()))
    {
      _propeditor=new PropertyEditor(_subject);
      _wid->gridLayout1->addWidget(_propeditor);
    }
}

EditionDataLink::~EditionDataLink()
{
}

void EditionDataLink::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("EditionDataLink::update " <<eventName(event) << " " << type);
  if(event == SETVALUE && _propeditor)
    _propeditor->update();
}
