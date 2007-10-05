#ifndef __YACSGUI_XML_DRIVER_H__
#define __YACSGUI_XML_DRIVER_H__

#include <ComposedNode.hxx>
#include <Proc.hxx>
#include <VisitorSaveSchema.hxx>
#include <parsers.hxx>

#include <qpoint.h>

class YACSGui_Module;

/*! 
 * Class that extends engine XML wirter capabilities by
 * adding information about node presentations to the output file.
 */
class YACSGui_VisitorSaveSchema : public YACS::ENGINE::VisitorSaveSchema
{
public:
  YACSGui_VisitorSaveSchema(YACSGui_Module* module, YACS::ENGINE::ComposedNode *root);
  virtual ~YACSGui_VisitorSaveSchema();

  virtual void    visitProc(YACS::ENGINE::Proc *node);

private:
  void            writePresentation(YACS::ENGINE::Proc *proc);
  void            writeLinks(YACS::ENGINE::Proc *proc);

private:
  YACSGui_Module* myModule;
};

/*! 
 * Class that extends engine XML loader. It can process the presentation data
 * not hanled by the base class.
 */
class YACSGui_Loader : public YACS::YACSLoader
{
public:
  struct PrsData
  {
    float x, y, z;
    float width, height;

    PrsData() { x = y = z = 0.; width = height = 1.; }
    PrsData(float theX, float theY, float theZ, 
	    float theWidth, float theHeight)
      { 
	x = theX;
        y = theY;
	z = theZ;
	width = theWidth;
	height = theHeight;
      }
  };

  typedef std::map<YACS::ENGINE::Node*, PrsData> PrsDataMap;

  struct LinkData
  {
    std::list<QPoint> points;

    LinkData() { points = std::list<QPoint>(0); }
    void appendPoint(QPoint thePoint)
      {
	points.push_back( thePoint );
      }
    void fillPoints(std::list<QPoint> theList)
      {
	points.clear();
	for (std::list<QPoint>::iterator it = theList.begin(); it != theList.end(); ++it)
	  points.push_back( *it );
      }
  };

  struct PortLinkData : LinkData
  {
    std::string fromnode, fromport, tonode, toport;

    PortLinkData():LinkData() { fromnode = fromport = tonode = toport = ""; }
    PortLinkData(std::string theFromnode, std::string theFromport, 
		 std::string theTonode,   std::string theToport):LinkData()
      {
	fromnode = theFromnode;
	fromport = theFromport;
	tonode = theTonode;
	toport = theToport;
      }
  };

  struct LabelLinkData : LinkData
  {
    std::string slavenode;

    LabelLinkData():LinkData() { slavenode = ""; }
    LabelLinkData(std::string theSlavenode):LinkData()
      {
	slavenode = theSlavenode;
      }
  };


  typedef std::map< std::pair<YACS::ENGINE::Port*, YACS::ENGINE::Port*>, std::list<QPoint> > PortLinkDataMap;
  typedef std::map< YACS::ENGINE::Node*, std::list<QPoint> > LabelLinkDataMap;

public:
  YACSGui_Loader();
  virtual ~YACSGui_Loader();

  const PrsDataMap&       getPrsData(YACS::ENGINE::Proc* proc, int&, int&);
  const PortLinkDataMap&  getPortLinkData(YACS::ENGINE::Proc* proc);
  const LabelLinkDataMap& getLabelLinkData(YACS::ENGINE::Proc* proc);

  void                    process(std::string theElement, bool theNewLink = false);
 
private:
  typedef std::map<std::string, PrsData> InputMap;

  typedef std::list<PortLinkData> InputPLList;
  typedef std::list<LabelLinkData> InputLLList;

  InputMap          myInputMap;
  PrsDataMap        myPrsMap; 

  InputPLList       myInputPLList;
  PortLinkDataMap   myPortLinkMap;

  InputLLList       myInputLLList;
  LabelLinkDataMap  myLabelLinkMap;
};

/*! 
 * Struct for parse <canvas .../> element from XML file.
 */
struct canvastype_parser: parser
{
  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::cerr << "canvastype_parser::onStart: " << el << std::endl;
      std::string element(el);
      parser* pp=&main_parser;
      SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }

  virtual void buildAttr(const XML_Char** attr)
    {
      required("width",attr);
      required("height",attr);
      for (int i = 0; attr[i]; i += 2) 
      {
	std::cerr << attr[i] << "=" << attr[i + 1] << std::endl;
	if(std::string(attr[i]) == "width")  width(attr[i+1]);
	if(std::string(attr[i]) == "height") height(attr[i+1]);
      }
    }
  
  virtual void pre ()
    {
      width_ = height_ = 1;
    }
  
  virtual void width(const std::string& width)
    {
      width_ = QString( width ).toInt();
    }
  
  virtual void height(const std::string& height)
    {
      height_ = QString( height ).toInt();
    }

  int width_, height_;
};

/*! 
 * Struct for parse <presentation .../> element from XML file.
 */
struct presentationtype_parser: parser
{
  presentationtype_parser():parser()
    {
      collector_ = 0;
    }

  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::cerr << "presentationtype_parser::onStart: " << el << std::endl;
      std::string element(el);
      parser* pp=&main_parser;
      SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }

  virtual void buildAttr(const XML_Char** attr)
    {
      required("name",attr);
      required("x",attr);
      required("y",attr);
      required("z",attr);
      required("width",attr);
      required("height",attr);
      for (int i = 0; attr[i]; i += 2) 
      {
	std::cerr << attr[i] << "=" << attr[i + 1] << std::endl;
	if(std::string(attr[i]) == "name")   name(attr[i+1]);
	if(std::string(attr[i]) == "x")      x(attr[i+1]);
	if(std::string(attr[i]) == "y")      y(attr[i+1]);
	if(std::string(attr[i]) == "z")      z(attr[i+1]);
	if(std::string(attr[i]) == "width")  width(attr[i+1]);
	if(std::string(attr[i]) == "height") height(attr[i+1]);
      }
      
      if ( collector_ )
	collector_->process("presentation");
    }

  virtual void pre ()
    {
      name_ = "";
      x_ = y_ = z_ = 0.;
      width_ = height_ = 1;
    }

  virtual void name(const std::string& name)
    {
      name_ = name;
    }

  virtual void x(const std::string& x)
    {
      x_ = QString( x ).toFloat();
    }

  virtual void y(const std::string& y)
    {
      y_ = QString( y ).toFloat();
    }

  virtual void z(const std::string& z)
    {
      z_ = QString( z ).toFloat();
    }

  virtual void width(const std::string& width)
    {
      width_ = QString( width ).toFloat();
    }

  virtual void height(const std::string& height)
    {
      height_ = QString( height ).toFloat();
    }

  std::string name_;
  float x_, y_, z_;
  float width_, height_;

  YACSGui_Loader* collector_;
};

/*! 
 * Struct for parse <point .../> element from XML file.
 */
struct pointtype_parser: parser
{
  virtual void onStart(const XML_Char* el, const XML_Char** attr)
    {
      std::cerr << "pointtype_parser::onStart: " << el << std::endl;
      std::string element(el);
      parser* pp=&main_parser;
      SetUserDataAndPush(pp);
      pp->init();
      pp->pre();
      pp->buildAttr(attr);
    }

  virtual void buildAttr(const XML_Char** attr)
    {
      required("x",attr);
      required("y",attr);
      for (int i = 0; attr[i]; i += 2) 
      {
	std::cerr << attr[i] << "=" << attr[i + 1] << std::endl;
	if(std::string(attr[i]) == "x") x(attr[i+1]);
	if(std::string(attr[i]) == "y") y(attr[i+1]);
      }
    }
  
  virtual QPoint post()
    {
      QPoint aP(x_,y_);
      return aP;
    }

  virtual void pre ()
    {
      x_ = y_ = 0;
    }
  
  virtual void x(const std::string& x)
    {
      x_ = QString( x ).toInt();
    }
  
  virtual void y(const std::string& y)
    {
      y_ = QString( y ).toInt();
    }

  int x_, y_;
};

/*! 
 * Struct for parse <prslink .../> element from XML file.
 */
struct prslinktype_parser: parser
{
   prslinktype_parser():parser()
    {
      collector_ = 0;
    }

  virtual void onStart(const XML_Char* el, const XML_Char** attr);
    
  virtual void onEnd(const char *el,parser* child)
    {
      std::cerr << "prslinktype_parser::onEnd: " << el << std::endl;
      std::string element(el);
      if(element == "point") point(((pointtype_parser*)child)->post());
    }

  virtual void buildAttr(const XML_Char** attr)
    {
      required("tonode",attr);
      for (int i = 0; attr[i]; i += 2) 
      {
	std::cerr << attr[i] << "=" << attr[i + 1] << std::endl;
	if(std::string(attr[i]) == "fromnode") fromnode(attr[i+1]);
	if(std::string(attr[i]) == "fromport") fromport(attr[i+1]);
	if(std::string(attr[i]) == "tonode")   tonode(attr[i+1]);
	if(std::string(attr[i]) == "toport")   toport(attr[i+1]);
      }
      
      if ( collector_ )
	collector_->process("prslink", true);
    }

  virtual void point(const QPoint& thePoint)
    {
      points_.push_back(thePoint);
      if ( collector_ )
	collector_->process("prslink");
    }

  virtual void pre ()
    {
      fromnode_ = fromport_ = tonode_ = toport_ = "";
    }

  virtual void fromnode(const std::string& fromnode)
    {
      fromnode_ = fromnode;
    }

  virtual void fromport(const std::string& fromport)
    {
      fromport_ = fromport;
    }
  
  virtual void tonode(const std::string& tonode)
    {
      tonode_ = tonode;
    }

  virtual void toport(const std::string& toport)
    {
      toport_ = toport;
    }

  std::string type()
    {
      std::string aType = "";
      if ( fromnode_ != "" && fromport_ != "" && tonode_ != "" && toport_ != "" )
	aType = "portlink";
      else if ( fromnode_ == "" && fromport_ == "" && tonode_ != "" && toport_ == "" )
	aType = "labellink";
      return aType;
    }

  std::string fromnode_, fromport_, tonode_, toport_;
  std::list<QPoint> points_;

  YACSGui_Loader* collector_;
};

#endif
