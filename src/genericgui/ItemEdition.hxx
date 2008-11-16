#ifndef _ITEMEDITION_HXX_
#define _ITEMEDITION_HXX_

#include <QTextEdit>

#include "guiObservers.hxx"

#include "FormEditItem.hxx"

namespace YACS
{
  namespace HMI
  {

    class TablePortsEdition;

    class ItemEditionBase: public GuiObserver
    {
    public:
      ItemEditionBase(Subject* subject);
      virtual ~ItemEditionBase();
      virtual void select(bool isSelected);
      virtual void synchronize();
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual Subject* getSubject();
    protected:
      Subject* _subject;
      int _stackId;
      std::string _name;
      std::string _type;
      std::string _category;
    };

    class ItemEdition: public FormEditItem, public ItemEditionBase
    {
      Q_OBJECT

    public slots:
      virtual void onApply();
      virtual void onCancel();
      virtual void onModifyName(const QString &text);

    public:
      ItemEdition(Subject* subject,
                  QWidget* parent = 0,
                  const char* name = 0);
      virtual ~ItemEdition();
      virtual void synchronize();
      virtual void select(bool isSelected);
      virtual void setName(std::string name);
      virtual void update(GuiEvent event, int type, Subject* son);

    protected:
      virtual void setEdited(bool isEdited);
      bool _isEdited;
      bool _haveScript;
    };

    class ItemEditionRoot: public ItemEdition
    {
    public:
      ItemEditionRoot(Subject* subject,
                      QWidget* parent = 0,
                      const char* name = 0);
      virtual ~ItemEditionRoot();
    };

  }
}



#endif
