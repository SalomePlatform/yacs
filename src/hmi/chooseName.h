#ifndef __CHOOSENAME_H_
#define __CHOOSENAME_H_

#include "dichoosename.h"

namespace YACS
{
  namespace HMI
  {

    class ChooseName: public diChooseName
    {
      Q_OBJECT

    public slots:
      void accept();
      void reject();
    public:
      ChooseName( std::string parentName,
                  std::string typeName,
                  std::string defaultName,
                  QWidget* parent = 0,
                  const char* name = 0,
                  bool modal = FALSE,
                  WFlags fl = 0 );
      ~ChooseName();
      std::string getChoosenName() { return _choosenName; };
      bool isOk() { return _isOk; };
    protected:
      std::string _choosenName;
      bool _isOk;
    };

  }
}
#endif
