
#include "FormLoop.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

FormLoop::FormLoop(QWidget *parent)
{
  DEBTRACE("FormLoop::FormLoop");
  setupUi(this);
}

FormLoop::~FormLoop()
{
  DEBTRACE("FormLoop::~FormLoop");
}
