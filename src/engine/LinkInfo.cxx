#include "LinkInfo.hxx"
#include "ComposedNode.hxx"

#include <sstream>

using namespace std;
using namespace YACS::ENGINE;

static const char GLOBAL_MESSAGE1[]="Global report  : \n";

static const char LINK_REPR[]="link";

LinkInfo::LinkInfo(unsigned char level):_levelOfInfo(level),_level(0)
{
}

void LinkInfo::clearAll()
{
  _level=0;
  _unsetInPort.clear();
  _onlyBackDefined.clear();
  _uselessLinks.clear();
  _infos.clear();
  _collapse.clear();
  _errors.clear();
}

void LinkInfo::startCollapseTransac()
{
  _level++;
}

void LinkInfo::endCollapseTransac() throw(Exception)
{
  if(--_level==0)
    {
      if(_levelOfInfo==ALL_STOP_ASAP or _levelOfInfo==ERR_ONLY_DONT_STOP)
        throw Exception(getErrRepr());
      if(_levelOfInfo==ALL_STOP_ASAP)
        throw Exception(getWarnRepr());
    }
}

void LinkInfo::setPointOfView(ComposedNode *pov)
{
  _pov=pov;
}

void LinkInfo::pushInfoLink(OutPort *semStart, InPort *end, InfoReason reason)
{
  _infos[reason].push_back(pair<OutPort *, InPort *>(semStart,end));
}

void LinkInfo::pushWarnLink(OutPort *semStart, InPort *end, WarnReason reason)
{
  if(_collapse[reason].empty())
    _collapse[reason].push_back(vector< pair<OutPort *,InPort *> >());
  else
    if(_collapse[reason].back()[0].second!=end)
      _collapse[reason].push_back(vector< pair<OutPort *,InPort *> >());
  _collapse[reason].back().push_back(pair<OutPort *,InPort *>(semStart,end));
}

void LinkInfo::pushErrLink(OutPort *semStart, InPort *end, ErrReason reason) throw(Exception)
{
  if(reason==E_NEVER_SET_INPUTPORT)
    _unsetInPort.push_back(end);
  else if(reason==E_ONLY_BACKWARD_DEFINED)
    _onlyBackDefined.push_back(end);
  else
    _errors[reason].push_back(pair<OutPort *, InPort *>(semStart,end));
  if(_level==0)
    if(_levelOfInfo==ALL_STOP_ASAP or _levelOfInfo==ERR_ONLY_DONT_STOP)
      throw Exception(getErrRepr());
}

void LinkInfo::pushUselessCFLink(Node *start, Node *end)
{
  _uselessLinks.insert(pair<Node *,Node *>(start,end));
}

void LinkInfo::takeDecision() const throw(Exception)
{
  if(!_errors.empty())
    throw Exception(getErrRepr());
}

std::string LinkInfo::getGlobalRepr() const
{
  ostringstream retS; retS << GLOBAL_MESSAGE1;
  retS << printThereIsAre(getNumberOfErrLinks(E_ALL),"error") << ".\n";
  retS << printThereIsAre(getNumberOfWarnLinksGrp(W_ALL),"warning") << ".\n";
  retS << printThereIsAre(getNumberOfInfoLinks(I_ALL),"info") << ".\n";
  if(getNumberOfErrLinks(E_ALL)>0)
    {
      retS << "****** ERRORS ******" << endl;
      retS << getErrRepr() << endl;
    }
  if(getNumberOfWarnLinksGrp(W_ALL)>0)
    {
      retS << "****** WARNINGS ******" << endl;
      retS << getWarnRepr() << endl;
    }
  if(getNumberOfInfoLinks(I_ALL)>0)
    {
      retS << "****** INFO ******" << endl;
      retS << getInfoRepr() << endl;
    }
  return retS.str();
}

std::string LinkInfo::getInfoRepr() const
{
  map<InfoReason, vector< pair<OutPort *,InPort *> > >::const_iterator iter;
  ostringstream stream;
  for(iter=_infos.begin();iter!=_infos.end();iter++)
    {
      for(vector< pair<OutPort *,InPort *> >::const_iterator iter2=(*iter).second.begin();iter2!=(*iter).second.end();iter2++)
        {
          stream << getStringReprOfI((*iter).first) << " between \"" << _pov->getOutPortName((*iter2).first);
          stream << "\" and \"" << _pov->getInPortName((*iter2).second) << "\"." << endl;
        }
    }
  set< pair<Node *, Node *> >::const_iterator iter3;
  for(iter3=_uselessLinks.begin();iter3!=_uselessLinks.end();iter3++)
    {
      stream << getStringReprOfI(I_USELESS) << " between \"" << _pov->getChildName((*iter3).first);
      stream << "\" and \"" << _pov->getChildName((*iter3).second) << "\"." << endl;
    }
  return stream.str();
}

std::string LinkInfo::getWarnRepr() const
{
  map<WarnReason, vector< vector< pair<OutPort *,InPort *> > > >::const_iterator iter;
  ostringstream stream;
  unsigned i=0;
  for(iter=_collapse.begin();iter!=_collapse.end();iter++)
    {
      stream << getStringReprOfW((*iter).first) << " for group containing following group links: ";
      vector< vector< pair<OutPort *,InPort *> > >::const_iterator iter2=(*iter).second.begin();
      for(;iter2!=(*iter).second.end();iter2++)
        {
          stream << "    Group # " << i++ << " : " << endl;
          for(vector< pair<OutPort *,InPort *> >::const_iterator iter3=(*iter2).begin();iter3!=(*iter2).end();iter3++)
            stream << "          \"" << _pov->getOutPortName((*iter3).first) << "\" and \"" << _pov->getInPortName((*iter3).second) << endl;
        }
    }
  return stream.str();
}

std::string LinkInfo::getErrRepr() const
{
  vector<InPort *>::const_iterator iter;
  ostringstream stream;
  for(iter=_unsetInPort.begin();iter!=_unsetInPort.end();iter++)
    stream << getStringReprOfE(E_NEVER_SET_INPUTPORT) << "\"" << _pov->getInPortName(*iter) << "\"." << endl;
  for(iter=_onlyBackDefined.begin();iter!=_onlyBackDefined.end();iter++)
    stream << getStringReprOfE(E_ONLY_BACKWARD_DEFINED) << "\"" << _pov->getInPortName(*iter) << "\"." << endl;
  map<ErrReason, vector< pair<OutPort *,InPort *> > >::const_iterator iter2;
  for(iter2=_errors.begin();iter2!=_errors.end();iter2++)
    for(vector< pair<OutPort *,InPort *> >::const_iterator iter3=(*iter2).second.begin();iter3!=(*iter2).second.end();iter3++)
      stream << getStringReprOfE((*iter2).first) << " between \"" <<_pov->getOutPortName((*iter3).first) << "\" and \"" << _pov->getInPortName((*iter3).second) << endl;
  return stream.str();
}

/*!
 * If 'reason'==I_ALL returns nummmber of types of links info whereas it returns number of info per type.
 */
unsigned LinkInfo::getNumberOfInfoLinks(InfoReason reason) const
{
  if(reason==I_ALL)
    return _infos.size()+_uselessLinks.size();
  if(reason==I_CF_USELESS)
    return _uselessLinks.size();
  else
    {
      map<InfoReason, vector< pair<OutPort *,InPort *> > >::const_iterator iter=_infos.find(reason);
      if(iter!=_infos.end())
        return (*iter).second.size();
      else
        return 0;
    }
}

unsigned LinkInfo::getNumberOfWarnLinksGrp(WarnReason reason) const
{
  unsigned ret=0;
  map<WarnReason, vector< vector< pair<OutPort *,InPort *> > > >::const_iterator iter;
  if(reason==W_ALL)
    {
      for(iter=_collapse.begin();iter!=_collapse.end();iter++)
        ret+=(*iter).second.size();
      return ret;
    }
  map<WarnReason, vector< vector< pair<OutPort *,InPort *> > > >::const_iterator iter2=_collapse.find(reason);
  if(iter2!=_collapse.end())
    return (*iter2).second.size();
  else
    return 0;
}

unsigned LinkInfo::getNumberOfErrLinks(ErrReason reason) const
{
  if(reason==E_ALL)
    return _errors.size()+_onlyBackDefined.size()+_unsetInPort.size();
  else if(reason==E_NEVER_SET_INPUTPORT)
    return _unsetInPort.size();
  else if(reason==E_ONLY_BACKWARD_DEFINED)
    return _onlyBackDefined.size();
  else
    {
      map<ErrReason, vector< pair<OutPort *,InPort *> > >::const_iterator iter=_errors.find(reason);
      if(iter!=_errors.end())
        return (*iter).second.size();
      else
        return 0;
    }
}

std::set< std::pair<Node *, Node *> > LinkInfo::getInfoUselessLinks() const
{
  return _uselessLinks;
}

std::pair<OutPort *, InPort *> LinkInfo::getInfoLink(unsigned id, InfoReason reason) const
{
  if(reason==I_CF_USELESS)
    return pair<OutPort *, InPort *>();
  map<InfoReason, vector< pair<OutPort *,InPort *> > >::const_iterator iter=_infos.find(reason);
  if(iter!=_infos.end())
    return (*iter).second[id];
  else
    return pair<OutPort *, InPort *>(0,0);
}

std::vector< std::pair<OutPort *, InPort *> > LinkInfo::getWarnLink(unsigned id, WarnReason reason) const
{
  map<WarnReason, vector< vector< pair<OutPort *,InPort *> > > >::const_iterator iter=_collapse.find(reason);
  if(iter!=_collapse.end())
    return (*iter).second[id];
  else
    return vector< pair<OutPort *, InPort *> >();
}

std::pair<OutPort *, InPort *> LinkInfo::getErrLink(unsigned id, ErrReason reason) const
{
  if(reason==E_NEVER_SET_INPUTPORT)
    return pair<OutPort *, InPort *>(0,_unsetInPort[id]);
  else if(reason==E_ONLY_BACKWARD_DEFINED)
    return pair<OutPort *, InPort *>(0,_onlyBackDefined[id]);
  else
    {
      map<ErrReason, vector< pair<OutPort *,InPort *> > >::const_iterator iter=_errors.find(reason);
      if(iter!=_errors.end())
        return (*iter).second[id];
      else
        return pair<OutPort *, InPort *>(0,0);
    }
}

std::string LinkInfo::getStringReprOfI(InfoReason reason)
{
  string ret;
  switch(reason)
    {
    case I_USELESS:
      ret="Useless CF";
      break;
    case I_BACK:
      ret="Back";
      break;
    case I_BACK_USELESS:
      ret="Back and useless";
      break;
    case I_BACK_CRAZY:
      ret+="Crazy back";
      break;
    case I_DFDS:
      ret+="DF/DS";
    }
  ret+=" "; ret+=LINK_REPR;
  return ret;
}

std::string LinkInfo::getStringReprOfW(WarnReason reason)
{
  string ret;
  switch(reason)
    {
    case W_COLLAPSE:
      ret="Collapse";
      break;
    case W_COLLAPSE_AND_USELESS:
      ret="Collapse and useless";
      break;
    case W_COLLAPSE_EL:
      ret="Collapse on ElementaryNode";
      break;
    case W_COLLAPSE_EL_AND_USELESS:
      ret+="Collapse on ElementaryNode and useless";
      break;
    case W_BACK_COLLAPSE:
      ret+="Back collapse";
      break;
    case W_BACK_COLLAPSE_AND_USELESS:
      ret+="Back collapse and useless";
      break;
    case W_BACK_COLLAPSE_EL:
      ret+="Back collapse on ElementaryNode";
      break;
    case W_BACK_COLLAPSE_EL_AND_USELESS:
      ret+="Back collapse and useless on ElementaryNode";
    }
  ret+=" "; ret+=LINK_REPR;
  return ret;
}

std::string LinkInfo::getStringReprOfE(ErrReason reason)
{
  string ret;
  if(reason==E_NEVER_SET_INPUTPORT)
    return "Never set InPort ";
  if(reason==E_ONLY_BACKWARD_DEFINED)
    return "Never set InPort only back defined ";
  switch(reason)
    {
    case E_DS_LINK_UNESTABLISHABLE:
      ret="DS unestablishable";
      break;
    case E_COLLAPSE_DFDS:
      ret="DF/DS collapse";
      break;
    case E_COLLAPSE_DS:
      ret="Inter DS collapse";
      break;
    case E_UNPREDICTABLE_FED:
      ret="Unpredictable fed";
    }
  ret+=" "; ret+=LINK_REPR;
  return ret;
}

std::string LinkInfo::printThereIsAre(unsigned val, const std::string& other)
{
  ostringstream ret;
  ret << "There ";
  if(val==0)
    ret << "are no";
  else if(val==1)
    ret << "is one";
  else
    ret << "are " << val;
  ret << " " << other;
  if(val==0 || val>1)
    ret << "s";
  return ret.str();
}
