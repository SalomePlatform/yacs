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
#ifndef _ITEMMIMEDATA_HXX_
#define _ITEMMIMEDATA_HXX_

#include <QMimeData>
#include <string>

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
      virtual Subject* getSubject() const;
      virtual YACS::ENGINE::Catalog* getCatalog() const;
      virtual std::string getCataName() const;
      virtual std::string getCompo() const;
      virtual std::string getType() const;
      virtual int getCase() const;
    protected:
      Subject *_sub;
      YACS::ENGINE::Catalog *_catalog;
      std::string _cataName;
      std::string _compoName;
      std::string _typeName;
      int _swCase;
    };
    
  }
}

#endif