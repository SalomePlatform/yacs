
#ifndef _YACSGUILOADER_HXX_
#define _YACSGUILOADER_HXX_

#include <parsers.hxx>

#include <map>
#include <string>
#include <QString>

namespace YACS
{
  namespace ENGINE
  {
    class Node;
    class Proc;
  }

  namespace HMI
  {

    struct PrsData
    {
      float _x, _y;
      float _width, _height;
      
      PrsData()
      {
        _x = _y = 0;
        _width = _height = 1.;
      }
      
      PrsData(float x,
              float y, 
              float width,
              float height)
      { 
        _x = x;
        _y = y;
        _width = width;
        _height = height;
      }
    };
    
    /*! 
     * Class that extends engine XML loader. It can process the presentation data
     * not hanled by the base class.
     */
    class YACSGuiLoader : public YACS::YACSLoader
    {
    public:
      YACSGuiLoader();
      virtual ~YACSGuiLoader();

      virtual YACS::ENGINE::Proc* load (const char *filename);
      std::map<YACS::ENGINE::Node*, PrsData> getPrsData(YACS::ENGINE::Proc* proc);
      
      void process(std::string element, bool newLink = false);
      
    private:
      std::map<YACS::ENGINE::Node*, PrsData> _prsMap;
      std::map<std::string, PrsData> _inputMap;
    };

    /*! 
     * Struct for parse <presentation .../> element from XML file.
     */
    struct presentationtype_parser: parser
    {   
      std::string name_;
      float x_, y_;
      float width_, height_;
      
      YACSGuiLoader* collector_;

      presentationtype_parser(): parser()
      {
        collector_ = 0;
      }
      
      virtual void onStart(const XML_Char* el, const XML_Char** attr)
      {
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
        required("width",attr);
        required("height",attr);
        for (int i = 0; attr[i]; i += 2) 
          {
            if(std::string(attr[i]) == "name")   name(attr[i+1]);
            if(std::string(attr[i]) == "x")      x(attr[i+1]);
            if(std::string(attr[i]) == "y")      y(attr[i+1]);
            if(std::string(attr[i]) == "width")  width(attr[i+1]);
            if(std::string(attr[i]) == "height") height(attr[i+1]);
          }
        
        if ( collector_ )
          collector_->process("presentation");
      }
      
      virtual void pre ()
      {
        name_ = "";
        x_ = y_ = 0.;
        width_ = height_ = 1;
      }
      
      virtual void name(const std::string& name)
      {
        name_ = name;
      }
      
      virtual void x(const std::string& x)
      {
        x_ = QString(x.c_str()).toFloat();
      }
      
      virtual void y(const std::string& y)
      {
        y_ = QString(y.c_str()).toFloat();
      }
      
      virtual void width(const std::string& width)
      {
        width_ = QString(width.c_str()).toFloat();
      }
      
      virtual void height(const std::string& height)
      {
        height_ = QString(height.c_str()).toFloat();
      }
    };


  }
}

#endif
