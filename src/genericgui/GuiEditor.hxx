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

#ifndef _GUIEDITOR_HXX_
#define _GUIEDITOR_HXX_


#include <QWidget>
#include <QString>
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
    class SubjectNode;
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
                                 SubjectComposedNode *cnode,
                                 bool createNewComponentInstance);
      void AddTypeFromCatalog(const ItemMimeData* myData);
      SubjectNode* CreateNode(std::string typeNode);
      void CreateBloc();
      void CreateForLoop();
      void CreateForEachLoop(std::string type );
      void CreateWhileLoop();
      void CreateSwitch();
      void CreateOptimizerLoop();
      void CreateContainer();
      void CreateHPContainer();
      void CreateComponentInstance();

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
      void PutSubjectInBloc();
      std::string PutGraphInBloc();
      void PutGraphInNode(std::string typeNode);
      void shrinkExpand(Qt::KeyboardModifiers kbModifiers = Qt::NoModifier);
      void rebuildLinks();
      void arrangeNodes(bool isRecursive);
      void arrangeProc();
      void showUndo(QWidget *parent = 0);
      void showRedo(QWidget *parent = 0);
      QString asciiFilter(const QString & name);

    protected:
      SubjectNode* _createNode(YACS::ENGINE::Catalog* catalog,
                       SubjectComposedNode *cnode,
                       std::string service,
                       std::string compoName,
                       bool createNewComponentInstance);
      std::string _table;
    };
  }
}

#endif
