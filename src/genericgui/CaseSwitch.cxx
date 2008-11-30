
#include "CaseSwitch.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

CaseSwitch::CaseSwitch(QWidget *parent)
{
  DEBTRACE("CaseSwitch::CaseSwitch");
  setParent(parent);
  setupUi(this);
  _isDefault = false;
}

CaseSwitch::~CaseSwitch()
{
  DEBTRACE("CaseSwitch::~CaseSwitch");
}

void CaseSwitch::on_cb_default_stateChanged(int state)
{
  DEBTRACE("CaseSwitch::on_cb_default_stateChanged " << state)
  if (state == Qt::Unchecked)
    setDefaultChecked(false);
  else
    setDefaultChecked(true);
}

bool CaseSwitch::isDefaultChecked()
{
  return _isDefault;
}

void CaseSwitch::setDefaultChecked(bool isDefault)
{
  DEBTRACE("CaseSwitch::setDefaultChecked " << isDefault)
  _isDefault = isDefault;
  sb_case->setVisible(!_isDefault);
  cb_default->setChecked(isDefault);
}
