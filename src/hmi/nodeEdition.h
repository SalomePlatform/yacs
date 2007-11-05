#ifndef _NODEEDITION_H_
#define _NODEEDITION_H_

#include "guiObservers.hxx"

#include "winodeedition.h"

namespace YACS
{
  namespace HMI
  {

    class NodeEdition: public wiNodeEdition, public GuiObserver
    {
      Q_OBJECT

    public slots:
      virtual void onApply();
      virtual void onCancel();
      virtual void onModify(const QString &text);
    public:
      NodeEdition(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0,
                  WFlags fl = 0 );
      virtual ~NodeEdition();
      virtual void setName(std::string name);
      virtual void select(bool isSelected);
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual Subject* getSubject();

    protected:
      virtual void setEdited(bool isEdited);
      std::string _type;
      std::string _name;
      bool _isEdited;
      Subject* _subject;
      int _stackId;
    };

  }
}



#endif
