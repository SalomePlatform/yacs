// Copyright (C) 2016-2017  CEA/DEN, EDF R&D
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
#include "Py2YacsModel.hxx"
#include <fstream>
#include <sstream>
#include <cerrno>
#include <algorithm>
#include <cstring>

Py2YacsModel::Py2YacsModel()
: _data(),
  _loadedFile(),
  _script(),
  _lastError()
{
}

Py2YacsModel::~Py2YacsModel()
{
}

void Py2YacsModel::loadFile(const std::string& path)
{
  std::ifstream file_stream(path);
  std::stringstream buffer;
  if(!file_stream)
  {
    buffer << "Error when opening file " << path
             << ": " << strerror(errno)
             << std::endl;
    _lastError = buffer.str();
    emit errorChanged(_lastError.c_str());
  }
  else
  {
    buffer << file_stream.rdbuf();
    _loadedFile = path;
    setScript(buffer.str());
  }
}

void Py2YacsModel::save()
{
  if(savePossible())
  {
    saveAs(_loadedFile);
  }
}

void Py2YacsModel::saveAs(const std::string& path)
{
  std::ofstream file_stream(path);
  file_stream << _script;
  if(!file_stream)
  {
    std::stringstream buffer;
    buffer << "Error when writing to file " << path
           << ": " << strerror(errno)
           << std::endl;
    _lastError = buffer.str();
    emit errorChanged(_lastError.c_str());
  }
  else
    _loadedFile = path;
}

void Py2YacsModel::setScript(const std::string& texte)
{
  std::stringstream buffer;
  _script = texte;
  emit scriptChanged(_script.c_str());
  _functionName = "";
  try
  {
    _data.load(_script);
    
    // get the errors
    buffer.clear();
    const std::list<std::string>& globalErrors = _data.getGlobalErrors();
    if(! globalErrors.empty())
    {
      buffer << "Global errors:" << std::endl;
      std::list<std::string>::const_iterator it;
      for(it=globalErrors.begin(); it!=globalErrors.end(); it++)
      {
        buffer << *it << std::endl;
      }
      buffer << "-----------------------------------------" << std::endl;
    }
    
    std::list<FunctionProperties>::const_iterator it_fp;
    const std::list<FunctionProperties>& functions = _data.getFunctionProperties();
    for(it_fp=functions.begin();it_fp!=functions.end();it_fp++)
    {
      if(! it_fp->_errors.empty())
      {
        buffer << "Function " << it_fp->_name << " has errors:" << std::endl;
        std::list<std::string>::const_iterator it;
        buffer << "Errors :" ;
        for(it=it_fp->_errors.begin();it!=it_fp->_errors.end();it++)
          buffer << *it << std::endl;
        buffer << "-----------------------------------------" << std::endl;
      }
    }
    _lastError = buffer.str();
  }
  catch(Py2yacsException& e)
  {
    _lastError = e.what();
    _lastError += "\n";
  }
  emit errorChanged(_lastError.c_str());
  emit functionsChanged(getValidFunctionNames());
}

const std::string& Py2YacsModel::getScript()
{
  return _script;
}

const std::string& Py2YacsModel::getLastError()
{
  return _lastError;
}

bool Py2YacsModel::exportToXml(const std::string& path)
{
  bool ok = true;
  if(schemaAvailable())
  {
    try
    {
    _data.save(path, _functionName);
    }
    catch(Py2yacsException& e)
    {
      _lastError = e.what();
      _lastError += "\n";
      ok = false;
    }
  }
  emit errorChanged(_lastError.c_str());
  return ok;
}

void Py2YacsModel::setFunctionName(const QString& functionName)
{
  _functionName = functionName.toStdString();
}

void Py2YacsModel::setFunctionName(const std::string& functionName)
{
  std::list<std::string> validFunctionNames = getValidFunctionNames();
  if(std::find(validFunctionNames.begin(),
               validFunctionNames.end(),
               functionName)               != validFunctionNames.end())
  {
    _functionName = functionName;
  }
  else
  {
    _lastError = "Invalid function name!";
    emit errorChanged(_lastError.c_str());
  }
}

std::list<std::string> Py2YacsModel::getValidFunctionNames()
{
  std::list<std::string> result;
  const std::list<std::string>& globalErrors = _data.getGlobalErrors();
  if(globalErrors.empty())
  {
    std::list<FunctionProperties>::const_iterator it_fp;
    const std::list<FunctionProperties>& functions = _data.getFunctionProperties();
    for(it_fp=functions.begin();it_fp!=functions.end();it_fp++)
    {
      if(it_fp->_errors.empty())
      {
        result.push_back(it_fp->_name);
      }
    }
  }
  return result;
}

bool Py2YacsModel::savePossible()
{
  return !_loadedFile.empty();
}

bool Py2YacsModel::schemaAvailable()
{
  return !_functionName.empty();
}

YACS::ENGINE::Proc* Py2YacsModel::getProc()
{
  YACS::ENGINE::Proc* result = NULL;
  if(schemaAvailable())
  {
    try
    {
      result = _data.createProc(_functionName);
    }
    catch(Py2yacsException& e)
    {
      _lastError = e.what();
      _lastError += "\n";
    }
  }
  return result;
}
