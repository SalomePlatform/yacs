
#include "FormEachLoop.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

FormEachLoop::FormEachLoop(QWidget *parent)
{
  DEBTRACE("FormEachLoop::FormEachLoop");
  setupUi(this);
}

FormEachLoop::~FormEachLoop()
{
  DEBTRACE("FormEachLoop::~FormEachLoop");
}
