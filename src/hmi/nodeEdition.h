#ifndef _NODEEDITION_H_
#define _NODEEDITION_H_

#include <qtextedit.h>
#include "guiObservers.hxx"

#include "winodeedition.h"

#ifdef HAVE_QEXTSCINTILLA_H
class QextScintilla;
#endif

namespace YACS
{
  namespace HMI
  {

    class itemEdition: public GuiObserver
    {
    public:
      itemEdition(Subject* subject);
      virtual ~itemEdition();
      virtual void select(bool isSelected);
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual Subject* getSubject();
    protected:
      Subject* _subject;
      int _stackId;
      std::string _name;
      std::string _type;
      std::string _category;
    };

    class NodeEdition: public wiNodeEdition, public itemEdition
    {
      Q_OBJECT

    public slots:
      virtual void onApply();
      virtual void onCancel();
      virtual void onModify(const QString &text);
      virtual void onFuncNameModified(const QString &text);
      virtual void onScriptModified();
    public:
      NodeEdition(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0,
                  WFlags fl = 0 );
      virtual ~NodeEdition();
      virtual void setName(std::string name);
      virtual void update(GuiEvent event, int type, Subject* son);

    protected:
      virtual void setEdited(bool isEdited);
      bool _isEdited;
      bool _haveScript;
#ifdef HAVE_QEXTSCINTILLA_H
      QextScintilla* _sci;
#else
      QTextEdit* _sci;
#endif
      SubjectInlineNode *_subInlineNode;
      SubjectPyFuncNode* _subFuncNode;
      std::string _funcName;
      QLineEdit* _liFuncName;
    };

  }
}



#endif
