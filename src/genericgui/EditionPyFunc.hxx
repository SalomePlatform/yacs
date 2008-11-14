
#ifndef _EDITIONPYFUNC_HXX_
#define _EDITIONPYFUNC_HXX_

#include "EditionScript.hxx"

namespace YACS
{
  namespace HMI
  {
    class EditionPyFunc: public EditionScript
    {
      Q_OBJECT

    public slots:
      virtual void onApply();
      virtual void onCancel();
      virtual void onFuncNameModified(const QString &text);

    public:
      EditionPyFunc(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionPyFunc();

    protected:
      SubjectPyFuncNode* _subFuncNode;
      std::string _funcName;
      QLineEdit* _liFuncName;
    };
  }
}
#endif
