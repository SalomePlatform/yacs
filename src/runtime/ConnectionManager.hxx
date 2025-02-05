// Copyright (C) 2007-2025  CEA, EDF
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

#ifndef _CONNECTION_MANAGER_HXX_
#define _CONNECTION_MANAGER_HXX_

#include "YACSRuntimeSALOMEExport.hxx"
#include "DSC_Basic.hxx"

#include <map>
#include <mutex>

#include <string>

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(DSC_Engines)

/*! \class ConnectionManager
 *  \brief This class implements the interface Engines::ConnectionManager.
 * It is used to make connections between CALCIUM ports.
 */
class YACSRUNTIMESALOME_EXPORT ConnectionManager
{
  public :
    ConnectionManager();
    ~ConnectionManager();
    
    typedef short connectionId;

    /*!
     * connect two CALCIUM ports of two components.
     */
    ConnectionManager::connectionId connect(Engines::DSC_ptr uses_component, 
                                            const char* uses_port_name, 
                                            Engines::DSC_ptr provides_component, 
                                            const char* provides_port_name);

    /*!
     * releases a connection performed with ConnectionManager::connect.
     */
    void disconnect(ConnectionManager::connectionId id,
                    Engines::DSC::Message message);

    /*!
       Shutdown the ConnectionManager process.
     */
    void ShutdownWithExit();

  private :

    struct connection_infos {
      Engines::DSC_var uses_component; 
      std::string uses_port_name;
      Engines::DSC_var provides_component;
      std::string provides_port_name;
      Ports::Port_var provides_port;
    };

    typedef std::map<Engines::ConnectionManager::connectionId, 
            connection_infos *> ids_type;
    typedef std::map<Engines::ConnectionManager::connectionId, 
            connection_infos *>::iterator ids_it_type;

    ids_type _ids;
    int _current_id;
    std::mutex _mutex;
};

#endif
