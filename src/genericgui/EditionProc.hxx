
#ifndef _EDITIONPROC_HXX_
#define _EDITIONPROC_HXX_

#include "EditionBloc.hxx"

#include <QTextEdit>
#include <string>

namespace YACS
{
  namespace HMI
  {

    class EditionProc: public EditionBloc
    {
    public:
      EditionProc(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~EditionProc();
      virtual void update(GuiEvent event, int type, Subject* son);

      // liste de nodes fils

    protected:
      QTextEdit *_statusLog;
      std::string _errorLog;
      std::string _modifLog;
    };
  }
}
#endif
