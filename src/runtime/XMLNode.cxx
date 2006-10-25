
#include "XMLNode.hxx"
#include "XMLPorts.hxx"

#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

XmlNode::XmlNode(const string& name)
  : ElementaryNode(name)
{
  _implementation="XML";
  cerr << "XMLNode::XMLNode " << name << endl;
}

void XmlNode::set_script(const string& script)
{
  _script=script;
}

void XmlNode::execute()
{
  cerr << "XmlNode::run" << endl;
  cerr << "---------------XmlNode::inputs---------------" << endl;
  set<InputPort *>::iterator iter;
  for(iter = _setOfInputPort.begin(); iter != _setOfInputPort.end(); iter++)
    {
      InputXmlPort *p=(InputXmlPort *)*iter;
      cerr << "port name: " << p->getName() << endl;
      cerr << "port kind: " << p->type()->kind() << endl;
      const char* ob=p->getXml();
      cerr << "Xml: " << ob << endl;
      getOutputPort(p->getName())->put(ob); // obligation meme ports en entree et sortie
    }
  cerr << "--------------XmlNode::calculation-----------" << _script << endl;
}


