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
      float _expx, _expy;
      float _expWidth, _expHeight;
      bool _expanded;
      int _shownState;
      
      PrsData()
      {
        _x = _y = 0;
        _width = _height = 1.;
        _expx = _expy = 0;
        _expWidth = _expHeight = 0;
        _expanded = true;
        _shownState = 0;
      }
      
      PrsData(float x,
              float y, 
              float width,
              float height,
              float expx,
              float expy,
              float expWidth,
              float expHeight,
              bool expanded,
              int shownState)
      { 
        _x = x;
        _y = y;
        _width = width;
        _height = height;
        _expx = expx;
        _expy = expy;
        _expWidth = expWidth;
        _expHeight = expHeight;
        _expanded = expanded;
        _shownState = shownState;
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
      virtual void reset();

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
      float expx_, expy_;
      float expWidth_, expHeight_;
      bool expanded_;
      int shownState_;
      
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
        //required("expanded", attr);
        for (int i = 0; attr[i]; i += 2) 
          {
            if(std::string(attr[i]) == "name")       name(attr[i+1]);
            if(std::string(attr[i]) == "x")          x(attr[i+1]);
            if(std::string(attr[i]) == "y")          y(attr[i+1]);
            if(std::string(attr[i]) == "width")      width(attr[i+1]);
            if(std::string(attr[i]) == "height")     height(attr[i+1]);
            if(std::string(attr[i]) == "expx")       expx(attr[i+1]);
            if(std::string(attr[i]) == "expy")       expy(attr[i+1]);
            if(std::string(attr[i]) == "expWidth")   expWidth(attr[i+1]);
            if(std::string(attr[i]) == "expHeight")  expHeight(attr[i+1]);
            if(std::string(attr[i]) == "expanded")   expanded(attr[i+1]);
            if(std::string(attr[i]) == "shownState") shownState(attr[i+1]);
          }
        
        if ( collector_ )
          collector_->process("presentation");
      }
      
      virtual void pre ()
      {
        name_ = "";
        x_ = y_ = 0.;
        width_ = height_ = 1;
        expx_ = expy_ = 0.;
        expanded_ = true;
        shownState_ = 0;
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
       
      virtual void expx(const std::string& x)
      {
        expx_ = QString(x.c_str()).toFloat();
      }
      
      virtual void expy(const std::string& y)
      {
        expy_ = QString(y.c_str()).toFloat();
      }
      
      virtual void expWidth(const std::string& x)
      {
        expWidth_ = QString(x.c_str()).toFloat();
      }
      
      virtual void expHeight(const std::string& y)
      {
        expHeight_ = QString(y.c_str()).toFloat();
      }
      
      virtual void expanded(const std::string& expanded)
      {
        expanded_ = QString(expanded.c_str()).toInt();
      }
      
      virtual void shownState(const std::string& shownState)
      {
        shownState_ = QString(shownState.c_str()).toInt();
      }
   };


  }
}

#endif
