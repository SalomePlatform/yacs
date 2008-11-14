
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

      void CreateInputPort(SubjectElementaryNode* seNode,
                           std::string name,
                           YACS::ENGINE::Catalog *catalog,
                           std::string type = "double",
                           SubjectDataPort* before = 0);

      void CreateOutputPort(SubjectElementaryNode* seNode, 
                           std::string name,
                           YACS::ENGINE::Catalog *catalog,
                           std::string type = "double",
                            SubjectDataPort* before = 0);
      void DeleteSubject();
      void DeleteSubject(Subject* parent,
                         Subject* toRemove);
      void rebuildLinks();
    };
  }
}

#endif
