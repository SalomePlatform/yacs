#include <YACSGui_XMLDriver.h>
#include <YACSGui_Graph.h>
#include <YACSGui_Module.h>
#include <YACSPrs_ElementaryNode.h>
#include <YACSPrs_Link.h>

#include <QxGraph_Canvas.h>
#include <QxGraph_CanvasView.h>
#include <QxGraph_ViewWindow.h>

#include <Node.hxx>
#include <InputPort.hxx>
#include <OutputPort.hxx>
#include <InputDataStreamPort.hxx>
#include <OutputDataStreamPort.hxx>

#include <qstring.h>
#include <qptrlist.h>

#include <set>

using namespace YACS::ENGINE;
using namespace std;

static canvastype_parser canvas_parser;
static presentationtype_parser presentation_parser;
static pointtype_parser point_parser;
static prslinktype_parser prslink_parser;

void prslinktype_parser::onStart(const XML_Char* el, const XML_Char** attr)
{
  std::cerr << "prslinktype_parser::onStart: " << el << std::endl;
  std::string element(el);
  parser* pp=&main_parser;
  if(element == "point") pp = &point_parser;
  SetUserDataAndPush(pp);
  pp->init();
  pp->pre();
  pp->buildAttr(attr);
}

YACSGui_VisitorSaveSchema::YACSGui_VisitorSaveSchema(YACSGui_Module* module,
						     ComposedNode *root)
  : VisitorSaveSalomeSchema(root), myModule(module)
{
}

YACSGui_VisitorSaveSchema::~YACSGui_VisitorSaveSchema()
{
}

void YACSGui_VisitorSaveSchema::visitProc(Proc *node)
{
  this->VisitorSaveSchema::visitProc(node);
  writePresentation(node);
  writeLinks(node);
}

void YACSGui_VisitorSaveSchema::writePresentation(Proc *proc)
{
  YACSGui_Graph* aGraph = myModule->getGraph(proc);
  if ( !aGraph || !aGraph->getCanvas() )
    return;

  QxGraph_CanvasView* aCV = myModule->getViewWindow(proc)->getViewModel()->getCurrentView();
  if ( !aCV ) return;

  int depth = 1;
  
  _out << indent(depth) << "<canvas";
  _out                  << " width=\""  << aGraph->getCanvas()->width() << "\"";
  _out                  << " height=\"" << aGraph->getCanvas()->height() << "\"";
  _out                  << " left=\""   << aCV->contentsX() << "\"";
  _out                  << " top=\""    << aCV->contentsY() << "\"";
  _out                  << " xscale=\"" << aCV->worldMatrix().m11() << "\"";
  _out                  << " yscale=\"" << aCV->worldMatrix().m22() << "\"";
  _out                  << "/>" << endl;

  set<Node*> nodeSet = getAllNodes(proc);

  for (set<Node*>::iterator iter = nodeSet.begin(); iter != nodeSet.end(); ++iter)
  {
    Node* aNode = *iter;
    YACSPrs_ElementaryNode* aPrs = aGraph->getItem( aNode );
    if ( !aPrs )
      continue;

    _out << indent(depth) << "<presentation";
    _out                  << " name=\""  << proc->getChildName( aNode ) << "\"";
    _out                  << " x=\""     << aPrs->x()                   << "\"";
    _out                  << " y=\""     << aPrs->y()                   << "\"";
    _out                  << " z=\""     << aPrs->z()                   << "\"";
    _out                  << " width=\"" << aPrs->width()               << "\"";
    _out                  << " height=\""<< aPrs->height()              << "\"";
    _out                  << "/>" << endl;
  }
}

void YACSGui_VisitorSaveSchema::writeLinks(YACS::ENGINE::Proc *proc)
{
    YACSGui_Graph* aGraph = myModule->getGraph(proc);
  if ( !aGraph || !aGraph->getCanvas() )
    return;

  int depth = 1;
  
  set<Node*> nodeSet = getAllNodes(proc);

  for (set<Node*>::iterator iter = nodeSet.begin(); iter != nodeSet.end(); ++iter)
  {
    Node* aNode = *iter;
    YACSPrs_ElementaryNode* aPrs = aGraph->getItem( aNode );
    if ( !aPrs )
      continue;

    QPtrList<YACSPrs_Port> aPorts = aPrs->getPortList();
    for (YACSPrs_Port* aPort = aPorts.first(); aPort; aPort = aPorts.next())
    {
      YACSPrs_InOutPort* anIOPort;
      if ( ( anIOPort = dynamic_cast<YACSPrs_InOutPort*>( aPort ) ) && !anIOPort->isInput()
	   ||
	   dynamic_cast<YACSPrs_LabelPort*>( aPort ) )
      { // this port is an output port => iterates on its links
	list<YACSPrs_Link*> aLinks = aPort->getLinks();
	for(list<YACSPrs_Link*>::iterator itL = aLinks.begin(); itL != aLinks.end(); itL++)
	{
	  _out << indent(depth) << "<prslink";
	  
	  if ( YACSPrs_PortLink* aPL = dynamic_cast<YACSPrs_PortLink*>( *itL ) ) {
	    _out << " fromnode=\"" << proc->getChildName( aNode )                                       << "\"";
	    _out << " fromport=\"" << aPort->getName()                                                  << "\"";
	    _out << " tonode=\""   << proc->getChildName( aPL->getInputPort()->getEngine()->getNode() ) << "\"";
	    _out << " toport=\""   << aPL->getInputPort()->getName()                                    << "\"";
	  }
	  else if ( YACSPrs_LabelLink* aLL = dynamic_cast<YACSPrs_LabelLink*>( *itL ) ) {
	    _out << " tonode=\""   << proc->getChildName( aLL->getSlaveNode()->getEngine() )            << "\"";
	  }
	  
	  _out << ">" << endl;
	  
	  list<QPoint> aPoints = (*itL)->getPoints();
	  for(list<QPoint>::iterator itP = aPoints.begin(); itP != aPoints.end(); itP++) {
	    _out << indent(depth+1) << "<point";
	    _out                    << " x=\"" << (*itP).x() << "\"";
	    _out                    << " y=\"" << (*itP).y() << "\"";
	    _out                    << "/>" << endl;
	  }
	  
	  _out << indent(depth) << "</prslink>" << endl;
	}
      }
    }
  }
}

YACSGui_Loader::YACSGui_Loader()
  : YACSLoader()
{
  presentation_parser.collector_ = this;
  prslink_parser.collector_ = this;

  canvas_parser.pre();
  _defaultParsersMap.insert(make_pair("canvas",&canvas_parser));
  _defaultParsersMap.insert(make_pair("presentation",&presentation_parser));
  _defaultParsersMap.insert(make_pair("point",&point_parser));
  _defaultParsersMap.insert(make_pair("prslink",&prslink_parser));
}

YACSGui_Loader::~YACSGui_Loader()
{
}

const YACSGui_Loader::PrsDataMap& YACSGui_Loader::getPrsData(Proc* proc,
							     int& width, int& height,
							     int& left, int& top,
							     double& xscale, double& yscale)
{
  myPrsMap.clear();

  if ( _defaultParsersMap.empty()
       ||
       !_defaultParsersMap["canvas"] ) return myPrsMap;

  // get information from canvastype_parser
  width  = ((canvastype_parser*)_defaultParsersMap["canvas"])->width_;
  height = ((canvastype_parser*)_defaultParsersMap["canvas"])->height_;
  left = ((canvastype_parser*)_defaultParsersMap["canvas"])->left_;
  top = ((canvastype_parser*)_defaultParsersMap["canvas"])->top_;
  xscale = ((canvastype_parser*)_defaultParsersMap["canvas"])->xscale_;
  yscale = ((canvastype_parser*)_defaultParsersMap["canvas"])->yscale_;

  for ( InputMap::iterator it = myInputMap.begin(); it != myInputMap.end(); it++ )
  {
    Node* aNode = proc->getChildByName( (*it).first );
    myPrsMap[aNode] = (*it).second;
  }

  return myPrsMap;
}

const YACSGui_Loader::PortLinkDataMap& YACSGui_Loader::getPortLinkData(YACS::ENGINE::Proc* proc)
{
  myPortLinkMap.clear();

  for ( InputPLList::iterator it = myInputPLList.begin(); it != myInputPLList.end(); it++ )
  {
    Node* aFromNode = proc->getChildByName((*it).fromnode);
    Port* aFromPort = 0;
    if ( (*it).fromport == "Gate" )
      aFromPort = aFromNode->getOutGate();
    else
      aFromPort = (Port*)(aFromNode->getOutPort((*it).fromport));
    
    Node* aToNode = proc->getChildByName((*it).tonode);
    Port* aToPort;
    if ( (*it).toport == "Gate" )
      aToPort = aToNode->getInGate();
    else
      aToPort = (Port*)(aToNode->getInPort((*it).toport));

    std::pair<Port*,Port*> aPPair(aFromPort,aToPort);
    myPortLinkMap[aPPair] = (*it).points;
  }

  return myPortLinkMap;
}

const YACSGui_Loader::LabelLinkDataMap& YACSGui_Loader::getLabelLinkData(YACS::ENGINE::Proc* proc)
{
    myLabelLinkMap.clear();

  for ( InputLLList::iterator it = myInputLLList.begin(); it != myInputLLList.end(); it++ )
  {
    Node* aSlaveNode = proc->getChildByName((*it).slavenode);
    myLabelLinkMap[aSlaveNode] = (*it).points;
  }

  return myLabelLinkMap;
}

void YACSGui_Loader::process(std::string theElement, bool theNewLink)
{
  if(theElement == "presentation")
  {
    if ( _defaultParsersMap["presentation"] )
    {
      presentationtype_parser* aP = (presentationtype_parser*)_defaultParsersMap["presentation"];
      myInputMap[aP->name_] = PrsData(aP->x_,aP->y_,aP->z_,aP->width_,aP->height_);
    }
  }
  else if(theElement == "prslink")
  {
    if ( _defaultParsersMap["prslink"] )
    {
      prslinktype_parser* aP = (prslinktype_parser*)_defaultParsersMap["prslink"];
      if ( aP->type() == "portlink" )
      {
	if ( theNewLink ) {
	  PortLinkData aPLData(aP->fromnode_,aP->fromport_,aP->tonode_,aP->toport_);
	  //aPLData.fillPoints(aP->points_);
	  myInputPLList.push_back(aPLData);
	}
	else if ( !myInputPLList.empty() )
	  myInputPLList.back().appendPoint(aP->points_.back());
      }
      else if ( aP->type() == "labellink" )
      {
	if ( theNewLink ) {
	  LabelLinkData aLLData(aP->tonode_);
	  //aLLData.fillPoints(aP->points_);
	  myInputLLList.push_back(aLLData);
	}
	else if ( !myInputLLList.empty() )
	  myInputLLList.back().appendPoint(aP->points_.back());
      }
    }
  }
}
