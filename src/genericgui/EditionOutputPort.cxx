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

#include "EditionOutputPort.hxx"
#include "QtGuiContext.hxx"
#include "DataPort.hxx"
#include "DataNode.hxx"
#include "InlineNode.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;
using namespace YACS::ENGINE;

EditionOutputPort::EditionOutputPort(Subject* subject,
                                     QWidget* parent,
                                     const char* name)
  : ItemEdition(subject, parent, name)
{
  if (!QtGuiContext::getQtCurrent()->isEdition())
    return;

  SubjectDataPort *sdp = dynamic_cast<SubjectDataPort*>(_subject);
  Node *parentNode = sdp->getPort()->getNode();
  if (parentNode)
    if (dynamic_cast<DataNode*>(parentNode) || dynamic_cast<InlineNode*>(parentNode))
      _wid->le_name->setReadOnly(false);
}

EditionOutputPort::~EditionOutputPort()
{
}

void EditionOutputPort::select(bool isSelected)
{
  DEBTRACE("EditionOutputPort::select " << isSelected);
  if (isSelected)
    {
      Subject *snode = _subject->getParent();
      if (snode)
        {
          if(QtGuiContext::getQtCurrent()->_mapOfEditionItem.count(snode) == 0)
            {
              //panel is not created. Force its creation
              update(YACS::HMI::ADD,snode->getType(),snode);
            }
          QWidget *widget = QtGuiContext::getQtCurrent()->_mapOfEditionItem[snode];
          ItemEdition *item = dynamic_cast<ItemEdition*>(widget);
          item->synchronize();
          QtGuiContext::getQtCurrent()->getStackedWidget()->setCurrentWidget(widget);
        }
      else
        {
          synchronize();
          QtGuiContext::getQtCurrent()->getStackedWidget()->setCurrentWidget(this);
        }
    }
}
