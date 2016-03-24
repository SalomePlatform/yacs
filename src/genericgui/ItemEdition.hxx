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

#ifndef _ITEMEDITION_HXX_
#define _ITEMEDITION_HXX_

#include <QTextEdit>
#include <string>
#include "guiObservers.hxx"

#include "FormEditItem.hxx"

namespace YACS
{
  namespace HMI
  {

    class TablePortsEdition;

    class ItemEditionBase: public GuiObserver
    {
    public:
      ItemEditionBase(Subject* subject);
      virtual ~ItemEditionBase();
      virtual void select(bool isSelected);
      virtual void synchronize();
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual Subject* getSubject();
    protected:
      Subject* _subject;
      int _stackId;
      std::string _name;
      std::string _type;
      std::string _category;
    };

    class ItemEdition: public FormEditItem, public ItemEditionBase
    {
      Q_OBJECT

    public slots:
      virtual void onApply();
      virtual void onCancel();
      virtual void onModifyName(const QString &text);

    public:
      ItemEdition(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~ItemEdition();
      virtual void synchronize();
      virtual void select(bool isSelected);
      virtual void setName(std::string name);
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual void setEdited(bool isEdited);
      static std::string filterName(const std::string& name);

    protected:
      bool _isEdited;
      bool _haveScript;
    };

    class ItemEditionRoot: public ItemEdition
    {
    public:
      ItemEditionRoot(Subject* subject,
                      QWidget* parent = 0,
                      const char* name = 0);
      virtual ~ItemEditionRoot();
    };

  }
}



#endif
