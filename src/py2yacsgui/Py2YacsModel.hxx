// Copyright (C) 2016-2019  CEA/DEN, EDF R&D
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
#ifndef PY2YACSMODEL_HXX
#define PY2YACSMODEL_HXX

#include <string>
#include "config_py2yacsgui.hxx"
#include "py2yacs.hxx"
#include <list>
#include <QObject>

namespace YACS
{
  namespace ENGINE
  {
    class Proc;
  };
};

class PY2YACSGUILIB_EXPORT Py2YacsModel : public QObject
{
  Q_OBJECT
  public:
  Py2YacsModel();
  virtual ~Py2YacsModel();
  void loadFile(const std::string& path);
  void save();
  void saveAs(const std::string& path);
  void setScript(const std::string& texte);
  const std::string& getScript();
  const std::string& getLastError();
  bool exportToXml(const std::string& path);
  std::list<std::string> getValidFunctionNames();
  void setFunctionName(const std::string& functionName);
  bool savePossible();
  bool schemaAvailable();
  YACS::ENGINE::Proc* getProc();
  
  public slots:
    void setFunctionName(const QString& functionName);
  
  signals:
    void scriptChanged(const QString& script);
    void errorChanged(const QString& script);
    void functionsChanged(std::list<std::string> validFunctionNames);
  private:
    Py2yacs _data;
    std::string _loadedFile;
    std::string _script;
    std::string _lastError;
    std::string _functionName;
};

#endif // PY2YACSMODEL_HXX
