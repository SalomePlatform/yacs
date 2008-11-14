
#include "FormContainer.hxx"
#include "FormComponent.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <QList>

using namespace std;

FormContainer::FormContainer(QWidget *parent)
{
  setupUi(this);
  _checked = false;
  _advanced = false;

  gridLayout2->removeWidget(cb_host);
  delete cb_host;
  cb_host = new ComboBox(gb_basic);
  gridLayout2->addWidget(cb_host, 2, 1, 1, 1);

  QIcon icon;
  icon.addFile("icons:icon_down.png");
  icon.addFile("icons:icon_up.png",
                QSize(), QIcon::Normal, QIcon::On);
  tb_container->setIcon(icon);
  on_tb_container_toggled(false);
  on_ch_advance_stateChanged(0);
}

FormContainer::~FormContainer()
{
}

void FormContainer::on_tb_container_toggled(bool checked)
{
  //DEBTRACE("FormContainer::on_tb_container_toggled " << checked);
  _checked = checked;
  if (_checked) fr_container->show();
  else fr_container->hide();
}

void FormContainer::on_ch_advance_stateChanged(int state)
{
  //DEBTRACE("FormContainer::on_ch_advance_stateChanged " << state);
  if (state) gb_advance->show();
  else gb_advance->hide();
}
