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

#ifndef _FACTORY_HXX_
#define _FACTORY_HXX_

#include <string>
#include <vector>
#include <map>

struct mytype{
    std::string _name;
    std::string _kind;
};

struct mycontrol{
    void fromnode(const std::string& fromnode) { _fromnode = fromnode; }
    void tonode(const std::string& tonode) { _tonode = tonode; }
    std::string fromnode()const { return _fromnode ; }
    std::string tonode()const { return _tonode ; }
    std::map<std::string,std::string> _props;
    void clear()
    {
      _props.clear();
    }
    void setProperty(const std::string& name, const std::string& value)
    {
      _props[name]=value;
    }
private:
    std::string _fromnode;
    std::string _tonode;
};

struct mylink:mycontrol{
    void fromport(const std::string& fromport) { _fromport = fromport; }
    void toport(const std::string& toport) { _toport = toport; }
    std::string fromport()const { return _fromport ; }
    std::string toport()const { return _toport ; }
    bool withControl() const { return _withControl ;}

    bool _withControl;
private:
    std::string _fromport;
    std::string _toport;
public:
    void clear()
    {
      _props.clear();
      _withControl=true;
    }
};

struct mystream:mylink{
};

struct myparam{
    std::string _tonode;
    std::string _toport;
    std::string _value;
};

struct myinport{
    std::string _name;
    std::string _type;
    std::map<std::string,std::string> _props;
    void clear()
    {
      _props.clear();
    }
    void setProperty(const std::string& name, const std::string& value)
    {
      _props[name]=value;
    }
};

struct myoutport:myinport{};

struct myprop{
    std::string _name;
    std::string _value;
};

typedef std::vector<myprop> myprops;

struct myfunc{
    std::string _name;
    std::string _code;
};

struct machine
{
    std::string _name;
};
typedef std::vector<machine> machines;

struct mycontainer
{
  std::string _name;
  machines _machs;
  std::map<std::string,std::string> _props;
};

struct loadon
{
    std::string _container;
};

struct mycomponentinstance
{
  std::string _name;
  std::string _kind;
  std::string _component;
  std::string _container;
  std::map<std::string,std::string> _props;
};

#endif
