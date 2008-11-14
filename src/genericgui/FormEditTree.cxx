
#include "FormEditTree.hxx"
#include "TreeView.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

FormEditTree::FormEditTree(QWidget *parent)
{
  setupUi(this);
  gridLayout->removeWidget(tv_schema);
  delete tv_schema;
  tv_schema = new TreeView(this);
  tv_schema->setObjectName(QString::fromUtf8("tv_schema"));
  gridLayout->addWidget(tv_schema, 0, 0, 1, 1);
}

FormEditTree::~FormEditTree()
{
}

