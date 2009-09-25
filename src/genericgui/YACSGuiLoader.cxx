
#include "YACSGuiLoader.hxx"
#include "Proc.hxx"
#include "Node.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;
using namespace YACS::ENGINE;

static presentationtype_parser presentation_parser;

YACSGuiLoader::YACSGuiLoader()
  : YACSLoader()
{
  presentation_parser.collector_ = this;
  _defaultParsersMap.insert(make_pair("presentation", &presentation_parser));
  _inputMap.clear();
  _prsMap.clear();
}

YACSGuiLoader::~YACSGuiLoader()
{
}

void YACSGuiLoader::reset()
{
  _inputMap.clear();
}

YACS::ENGINE::Proc* YACSGuiLoader::load(const char *filename)
{
  _inputMap.clear();
  return YACS::YACSLoader::load(filename);
}

std::map<YACS::ENGINE::Node*, PrsData> YACSGuiLoader::getPrsData(YACS::ENGINE::Proc* proc)
{
  _prsMap.clear();

  if (_defaultParsersMap.empty()) return _prsMap;

  for (map<string, PrsData>::iterator it = _inputMap.begin(); it != _inputMap.end(); ++it)
  {
    Node* node = 0;
    string name = (*it).first;

    if (name == "__ROOT__")
      node = proc;
    else
      {
        try
          {
            node = proc->getChildByName(name);
          }
        catch(Exception& ex)
          {
            continue;
          }
      }
    _prsMap[node] = (*it).second;
  }
  return _prsMap;
}

void YACSGuiLoader::process(std::string theElement, bool theNewLink)
{
  if(theElement == "presentation")
  {
    if ( _defaultParsersMap["presentation"] )
    {
      presentationtype_parser* aP = (presentationtype_parser*)_defaultParsersMap["presentation"];
      _inputMap[aP->name_] = PrsData(aP->x_, aP->y_, aP->width_, aP->height_);
      DEBTRACE(aP->name_ << " " << aP->x_ << " " << aP->y_ << " " <<  aP->width_ << " " << aP->height_);
    }
  }
}
