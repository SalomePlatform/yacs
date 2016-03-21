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

#ifndef _ITEMMIMEDATA_HXX_
#define _ITEMMIMEDATA_HXX_

#include <QMimeData>
#include <string>
#include <vector>

namespace YACS
{
  namespace ENGINE
  {
    class Catalog;
  }

  namespace HMI
  {
    class Subject;

    /*!
     * ItemMimeData is used for Drag and Drop of Scene Items.
     */
    class ItemMimeData: public QMimeData
    {
    public:
      ItemMimeData();
      virtual ~ItemMimeData();
      virtual void setSubject(Subject *sub);
      virtual void setCatalog(YACS::ENGINE::Catalog *cata);
      virtual void setCataName(std::string cataName);
      virtual void setCompo(std::string compo);
      virtual void setType(std::string aType);
      virtual void setCase(int aCase);
      virtual Subject* getSubject(int i=0) const;
      virtual YACS::ENGINE::Catalog* getCatalog(int i=0) const;
      virtual std::string getCataName(int i=0) const;
      virtual std::string getCompo(int i=0) const;
      virtual std::string getType(int i=0) const;
      virtual int getCase() const;
      virtual bool getControl() const;
      virtual void setControl(bool control);
      virtual int getDataSize() const;
    protected:
      std::vector<Subject*> _sub;
      std::vector<YACS::ENGINE::Catalog*> _catalog;
      std::vector<std::string> _cataName;
      std::vector<std::string> _compoName;
      std::vector<std::string> _typeName;
      int _swCase;
      bool _control;
    };
    
  }
}

#endif
