// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#include "ConnectionManager.hxx"
#include <iostream>

ConnectionManager::ConnectionManager()
: _ids()
, _current_id(0)
, _mutex()
{
}

ConnectionManager::~ConnectionManager()
{
}

ConnectionManager::connectionId
ConnectionManager::connect(Engines::DSC_ptr uses_component, 
                             const char* uses_port_name, 
                             Engines::DSC_ptr provides_component, 
                             const char* provides_port_name) 
{
  // We use a mutex for multithreaded applications.
  std::unique_lock<std::mutex> lock(_mutex);
  Ports::Port_var p_port = provides_component->get_provides_port(provides_port_name, false);
  uses_component->connect_uses_port(uses_port_name, p_port);
  provides_component->connect_provides_port(provides_port_name);

  // Creating a new structure containing connection's infos.
  connection_infos * infos = new connection_infos();
  infos->uses_component = Engines::DSC::_duplicate(uses_component);
  infos->uses_port_name = uses_port_name;
  infos->provides_component = Engines::DSC::_duplicate(provides_component);
  infos->provides_port_name = provides_port_name;
  infos->provides_port = Ports::Port::_duplicate(p_port);

  // Creating a new connection id.
  ConnectionManager::connectionId rtn_id = _current_id;
  _current_id += 1;
  // Adding the new connection into the map.
  _ids[rtn_id] = infos;

  return rtn_id;
}

void
ConnectionManager::disconnect(ConnectionManager::connectionId id,
                              Engines::DSC::Message message)
{
  std::unique_lock<std::mutex> lock(_mutex);
  int err=0;
  // Connection id exist ?
  ids_it_type ids_it = _ids.find(id);
  if (ids_it == _ids.end())
    return;

  // TODO
  // We need to catch exceptions if one of these disconnect operation fails.
  // connection_infos * infos = ids[id];
  connection_infos * infos = ids_it->second;
  try
    {
      infos->provides_component->disconnect_provides_port(infos->provides_port_name.c_str(), message);
    }
  catch(CORBA::SystemException& ex)
    {
      std::cerr << "Problem in disconnect(CORBA::SystemException) provides port: " << infos->provides_port_name.c_str() << std::endl;
      err=1;
    }
  try
    {
      infos->uses_component->disconnect_uses_port(infos->uses_port_name.c_str(),
                                                  infos->provides_port, message);
    }
  catch(CORBA::SystemException& ex)
    {
      std::cerr << "Problem in disconnect(CORBA::SystemException) uses port: " << infos->uses_port_name.c_str() << std::endl;
      err=1;
    }
  delete infos;
  _ids.erase(id);

  if(err)
    throw Engines::DSC::BadPortReference();
}

void
ConnectionManager::ShutdownWithExit()
{
  ids_it_type ids_it = _ids.begin();
  while(ids_it != _ids.end())
    {
      disconnect(ids_it->first, Engines::DSC::RemovingConnection);
      ids_it = _ids.begin();
    }
}
