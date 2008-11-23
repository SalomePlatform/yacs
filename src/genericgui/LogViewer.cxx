
#include "LogViewer.hxx"
#include <fstream>
#include <sstream>

using namespace std;

LogViewer::LogViewer(std::string label, QWidget *parent)
{
  setupUi(this);
  _label = label;
  browser->setLineWrapMode(QTextEdit::NoWrap);
  browser->setReadOnly(1);
  connect(pushButton,SIGNAL(clicked()),this, SLOT(close()));
}

LogViewer::~LogViewer()
{
}

void LogViewer::setText(std::string text)
{
  string atext = _label + "\n\n" + text;
  browser->setText(atext.c_str());
}

void LogViewer::readFile(std::string fileName)
{
  std::fstream f(fileName.c_str());
  std::stringstream hfile;
  hfile << f.rdbuf();
  string atext = _label + "\n" + fileName + "\n\n";
  QString qtext = atext.c_str();
  browser->setText(qtext + hfile.str().c_str());
  f.close();
}
