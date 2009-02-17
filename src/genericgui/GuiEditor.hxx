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
#ifndef _GUIEDITOR_HXX_
#define _GUIEDITOR_HXX_


#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class Catalog;
  };

  namespace HMI
  {
    class Subject;
    class SubjectElementaryNode;
    class SubjectComposedNode;
    class SubjectDataPort;
    class ItemMimeData;

    class GuiEditor
    {
    public:
      GuiEditor();
      virtual ~GuiEditor();

      void CreateNodeFromCatalog(const ItemMimeData* myData,
                                 SubjectComposedNode *cnode);
      void AddTypeFromCatalog(const ItemMimeData* myData);
      void CreateNode(std::string typeNode);
      void CreateBloc();
      void CreateForLoop();
      void CreateForEachLoop();
      void CreateWhileLoop();
      void CreateSwitch();
      void CreateContainer();

      SubjectDataPort* CreateInputPort(SubjectElementaryNode* seNode,
                                       std::string name,
                                       YACS::ENGINE::Catalog *catalog,
                                       std::string type = "double",
                                       SubjectDataPort* before = 0);

      SubjectDataPort* CreateOutputPort(SubjectElementaryNode* seNode, 
                                        std::string name,
                                        YACS::ENGINE::Catalog *catalog,
                                        std::string type = "double",
                                        SubjectDataPort* before = 0);
      void DeleteSubject();
      void DeleteSubject(Subject* parent,
                         Subject* toRemove);

      void CutSubject();
      void CopySubject();
      void PasteSubject();

      void rebuildLinks();
      void arrangeNodes(bool isRecursive);

    protected:
      void _createNode(YACS::ENGINE::Catalog* catalog,
                       SubjectComposedNode *cnode,
                       std::string service,
                       std::string compoName);
    };
  }
}

#endif
