
#ifndef _EDITIONSCRIPT_HXX_
#define _EDITIONSCRIPT_HXX_

#include "yacsconfig.h"
#include "EditionElementaryNode.hxx"

#ifdef HAS_QSCI4
class QsciScintilla;
#endif

namespace YACS
{
  namespace HMI
  {
    class EditionScript: public EditionElementaryNode
    {
      Q_OBJECT

    public slots:
      virtual void onApply();
      virtual void onCancel();
      virtual void onScriptModified();

    public:
      EditionScript(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionScript();

    protected:
      SubjectInlineNode *_subInlineNode;

#ifdef HAS_QSCI4
      QsciScintilla* _sci;
#else
      QTextEdit* _sci;
#endif

    };
  }
}
#endif
