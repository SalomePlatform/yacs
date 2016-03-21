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

#ifndef __HXX2SALOME_GENERIC_CLASS_NAME_HXX_hxx2salome__
#define __HXX2SALOME_GENERIC_CLASS_NAME_HXX_hxx2salome__

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(HXX2SALOME_GENERIC_CLASS_NAME_Gen)
#include CORBA_CLIENT_HEADER(MED)
#include "SALOME_Component_i.hxx"
#include "SALOMEMultiComm.hxx"
class HXX2SALOME_GENERIC_CLASS_NAME;  // forward declaration

class HXX2SALOME_GENERIC_CLASS_NAME_i:
  public POA_HXX2SALOME_GENERIC_CLASS_NAME_ORB::HXX2SALOME_GENERIC_CLASS_NAME_Gen,
  public Engines_Component_i,
  public SALOMEMultiComm
{

public:
    HXX2SALOME_GENERIC_CLASS_NAME_i(CORBA::ORB_ptr orb,
	    PortableServer::POA_ptr poa,
	    PortableServer::ObjectId * contId, 
	    const char *instanceName, 
	    const char *interfaceName);
    virtual ~HXX2SALOME_GENERIC_CLASS_NAME_i();

//  HXX2SALOME_HXX_CODE

private:
    std::auto_ptr<HXX2SALOME_GENERIC_CLASS_NAME> cppCompo_;

};


extern "C"
    PortableServer::ObjectId * HXX2SALOME_GENERIC_CLASS_NAMEEngine_factory(
	    CORBA::ORB_ptr orb,
	    PortableServer::POA_ptr poa,
	    PortableServer::ObjectId * contId,
	    const char *instanceName,
	    const char *interfaceName);


#endif
