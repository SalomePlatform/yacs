
#include "FormComponent.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <QList>

using namespace std;

ComboBox::ComboBox(QWidget *parent)
  : QComboBox(parent)
{
}

ComboBox::~ComboBox()
{
}
    
void ComboBox::mousePressEvent(QMouseEvent *event)
{
  emit mousePressed();
  QComboBox::mousePressEvent(event);
}


FormComponent::FormComponent(QWidget *parent)
{
  setupUi(this);

  le_name->setReadOnly(true);

  gridLayout3->removeWidget(cb_instance);
  delete cb_instance;
  cb_instance = new ComboBox(fr_component);
  gridLayout3->addWidget(cb_instance, 1, 1, 1, 1);

  gridLayout3->removeWidget(cb_container);
  delete cb_container;
  cb_container = new ComboBox(fr_component);
  gridLayout3->addWidget(cb_container, 2, 1, 1, 1);

  _checked = false;
  QIcon icon;
  icon.addFile("icons:icon_down.png");
  icon.addFile("icons:icon_up.png",
                QSize(), QIcon::Normal, QIcon::On);
  tb_component->setIcon(icon);
  on_tb_component_toggled(false);
//   connect(tb_component, SIGNAL(toggled(bool)),
//           this, SLOT(on_tb_component_toggled(bool)));
}

FormComponent::~FormComponent()
{
}

void FormComponent::on_tb_component_toggled(bool checked)
{
  //DEBTRACE("FormComponent::on_tb_component_toggled " << checked);
  _checked = checked;
  if (_checked) fr_component->show();
  else fr_component->hide();
}
