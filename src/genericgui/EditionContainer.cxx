
#include "EditionContainer.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include "FormContainer.hxx"
#include "QtGuiContext.hxx"
#include "Proc.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;
using namespace YACS::ENGINE;

EditionContainer::EditionContainer(Subject* subject,
                                   QWidget* parent,
                                   const char* name)
  : ItemEdition(subject, parent, name)
{
  _wContainer = new FormContainer(this);
  _wid->gridLayout1->addWidget(_wContainer);
  connect(_wContainer->cb_host, SIGNAL(mousePressed()),
          this, SLOT(fillContainerPanel()));
  connect(_wContainer->tb_container, SIGNAL(toggled(bool)),
          this, SLOT(fillContainerPanel())); // --- to update display of current selection
  _wContainer->tb_container->toggle();
}

EditionContainer::~EditionContainer()
{
}

void EditionContainer::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("EditionContainer::update");
  ItemEdition::update(event, type, son);
  switch (event)
    {
    case RENAME:
      _wContainer->le_name->setText((son->getName()).c_str());
      fillContainerPanel();
      break;
    default:
      ;
    }
}

void EditionContainer::fillContainerPanel()
{
  DEBTRACE("EditionContainer::fillContainerPanel");

  _wContainer->le_name->setText(_subject->getName().c_str());
  _wContainer->le_instance->setReadOnly(true);

  _wContainer->cb_host->clear();
  _wContainer->cb_host->addItem(""); // --- when no host selected

  list<string> machines = QtGuiContext::getQtCurrent()->getGMain()->getMachineList();
  list<string>::iterator itm = machines.begin();
  for( ; itm != machines.end(); ++itm)
    {
      _wContainer->cb_host->addItem(QString((*itm).c_str()));
    }
}
