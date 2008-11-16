
#ifndef _QTGUICONTEXT_HXX_
#define _QTGUICONTEXT_HXX_

#include "guiContext.hxx"
#include "SchemaModel.hxx"
#include "SchemaItem.hxx"
#include "SceneItem.hxx"
#include "GenericGui.hxx"
#include "FormEditTree.hxx"
#include "GraphicsView.hxx"
#include "GuiExecutor.hxx"
#include <QGraphicsScene>
#include <QItemSelectionModel>
#include <QStackedWidget>
#include <QWidget>
#include <QString>
#include <set>

namespace YACS
{
  namespace HMI
  {
    class QtGuiContext: public GuiContext
    {
    public:
      QtGuiContext(YACS::HMI::GenericGui* gmain);
      virtual ~QtGuiContext();
      virtual void setProc(YACS::ENGINE::Proc* proc);

      inline YACS::HMI::GenericGui* getGMain()                   {return _gmain; };

      inline YACS::HMI::SchemaModel* getSchemaModel()            {return _schemaModel; };
      inline FormEditTree* getEditTree()                         {return _editTree; };
      inline QGraphicsScene* getScene()                          {return _scene; };
      inline YACS::HMI::GraphicsView* getView()                  {return _view; };
      inline QWidget* getWindow()                                {return _window; };
      inline QItemSelectionModel* getSelectionModel()            {return _selectionModel; };
      inline QStackedWidget* getStackedWidget()                  {return _stackedWidget; };
      inline QString getFileName()                               {return _fileName; };
      inline YACS::HMI::GuiExecutor* getGuiExecutor()            {return _guiExecutor; };
      inline bool isEdition()                                    {return _isEdition; };
      inline int getStudyId()                                    {return _studyId; };

      inline YACS::HMI::Subject* getSelectedSubject()            {return _selectedSubject; };

      inline void setSchemaModel(YACS::HMI::SchemaModel* model)  {_schemaModel = model; };
      inline void setEditTree(FormEditTree* editTree)            {_editTree = editTree; };
      inline void setScene(QGraphicsScene *scene)                {_scene = scene; };
      inline void setView(YACS::HMI::GraphicsView *view)         {_view = view; };
      inline void setWindow(QWidget* window)                     {_window = window; };
      inline void setSelectionModel(QItemSelectionModel* selmod) {_selectionModel = selmod; };
      inline void setStackedWidget(QStackedWidget* sw)           {_stackedWidget = sw; };
      inline void setFileName(const QString& fileName)           {_fileName = fileName; };
      inline void setGuiExecutor(YACS::HMI::GuiExecutor* guiEx)  {_guiExecutor = guiEx; };
      inline void setEdition(bool isEdition)                     {_isEdition = isEdition; };
      inline void setStudyId(int studyId)                        {_studyId = studyId; };

      inline void setSelectedSubject(YACS::HMI::Subject* sub)    {_selectedSubject = sub; };

      inline static QtGuiContext* getQtCurrent()             {return _QtCurrent; };
      inline static void setQtCurrent(QtGuiContext* context) { _QtCurrent=context; _current=context; };

      std::map<YACS::HMI::Subject*, YACS::HMI::SchemaItem*> _mapOfSchemaItem;
      std::map<YACS::HMI::Subject*, YACS::HMI::SceneItem*>  _mapOfSceneItem;
      std::set<YACS::HMI::Subject*> _setOfModifiedSubjects;

      static std::set<QtGuiContext*> _setOfContext;

    protected:
      static QtGuiContext* _QtCurrent;
      YACS::HMI::GenericGui* _gmain;
      YACS::HMI::SchemaModel* _schemaModel;
      FormEditTree* _editTree;
      QItemSelectionModel* _selectionModel;
      QGraphicsScene* _scene;
      YACS::HMI::GraphicsView* _view;
      QWidget* _window;
      QStackedWidget * _stackedWidget;
      QString _fileName;
      YACS::HMI::GuiExecutor *_guiExecutor;
      YACS::HMI::Subject* _selectedSubject;
      bool _isEdition;
      int _studyId;
    };

  }
}
#endif
