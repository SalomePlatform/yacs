// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
      _inputMap[aP->name_] = PrsData(aP->x_, aP->y_, aP->width_, aP->height_, aP->expx_, aP->expy_,
                                     aP->expWidth_, aP->expHeight_, aP->expanded_, aP->shownState_);
      DEBTRACE(aP->name_ << " " << aP->x_ << " " << aP->y_ << " " <<  aP->width_ << " " << aP->height_ << " " 
               << aP->expx_ << " " << aP->expy_ << " " << aP->expWidth_ << " " << aP->expHeight_ << " " << aP->expanded_ << " " << aP->shownState_);
    }
  }
}
