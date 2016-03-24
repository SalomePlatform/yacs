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

#ifndef _QTGUICONTEXT_HXX_
#define _QTGUICONTEXT_HXX_

#include "GenericGuiExport.hxx"
#include "guiContext.hxx"
#include "SchemaModel.hxx"
#include "SchemaItem.hxx"
#include "SceneItem.hxx"
#include "ItemEdition.hxx"
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

#include <SuitWrapper.hxx>

class counters;

namespace YACS
{
  namespace HMI
  {
    class GENERICGUI_EXPORT QtGuiContext: public GuiContext
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
      inline YACS::HMI::ItemEditionRoot* getEditionRoot()        {return _rootEdit; };
      inline QWidget* getWindow()                                {return _window; };
      inline QItemSelectionModel* getSelectionModel()            {return _selectionModel; };
      inline QStackedWidget* getStackedWidget()                  {return _stackedWidget; };
      inline QString getFileName()                               {return _fileName; };
      inline YACS::HMI::GuiExecutor* getGuiExecutor()            {return _guiExecutor; };
      inline bool isEdition()                                    {return _isEdition; };
      inline int getStudyId()                                    {return _studyId; };
      inline bool isLoadingPresentation()                        {return _isLoadingPresentation; };

      YACS::HMI::Subject* getSubjectToPaste(bool &isCut);

      YACS::HMI::Subject* getSelectedSubject()                   {return _selectedSubject; };

      inline void setSchemaModel(YACS::HMI::SchemaModel* model)  {_schemaModel = model; };
      inline void setEditTree(FormEditTree* editTree)            {_editTree = editTree; };
      inline void setScene(QGraphicsScene *scene)                {_scene = scene; };
      inline void setView(YACS::HMI::GraphicsView *view)         {_view = view; };
      inline void setEditionRoot(YACS::HMI::ItemEditionRoot* re) {_rootEdit = re; };
      inline void setWindow(QWidget* window)                     {_window = window; };
      inline void setSelectionModel(QItemSelectionModel* selmod) {_selectionModel = selmod; };
      inline void setStackedWidget(QStackedWidget* sw)           {_stackedWidget = sw; };
      inline void setFileName(const QString& fileName)           {_fileName = fileName; };
      inline void setGuiExecutor(YACS::HMI::GuiExecutor* guiEx)  {_guiExecutor = guiEx; };
      inline void setEdition(bool isEdition)                     {_isEdition = isEdition; };
      inline void setStudyId(int studyId)                        {_studyId = studyId; };
      inline void setLoadingPresentation(bool isLoadpres)        {_isLoadingPresentation = isLoadpres; };

      void setSubjectToCut(YACS::HMI::Subject* sub);
      void setSubjectToCopy(YACS::HMI::Subject* sub);

      inline void setSelectedSubject(YACS::HMI::Subject* sub)    {_selectedSubject = sub; };

      inline static QtGuiContext* getQtCurrent()             {return _QtCurrent; };
      inline static void setQtCurrent(QtGuiContext* context) { _QtCurrent=context; _current=context; };

      std::map<YACS::HMI::Subject*, YACS::HMI::SchemaItem*> _mapOfSchemaItem;
      std::map<YACS::HMI::Subject*, YACS::HMI::SceneItem*>  _mapOfSceneItem;
      std::map<YACS::HMI::Subject*, QWidget*> _mapOfEditionItem;
      std::set<YACS::HMI::Subject*> _setOfModifiedSubjects;

      static std::set<QtGuiContext*> _setOfContext;
      static counters* _counters;
      static bool _delayCalc;

    protected:
      static QtGuiContext* _QtCurrent;
      YACS::HMI::GenericGui* _gmain;
      YACS::HMI::SchemaModel* _schemaModel;
      FormEditTree* _editTree;
      QItemSelectionModel* _selectionModel;
      QGraphicsScene* _scene;
      YACS::HMI::GraphicsView* _view;
      YACS::HMI::ItemEditionRoot* _rootEdit;
      QWidget* _window;
      QStackedWidget * _stackedWidget;
      QString _fileName;
      YACS::HMI::GuiExecutor *_guiExecutor;
      YACS::HMI::Subject* _selectedSubject;
      bool _isEdition;
      bool _isLoadingPresentation;
      int _studyId;
      SuitWrapper* _wrapper;
    };

  }
}
#endif
