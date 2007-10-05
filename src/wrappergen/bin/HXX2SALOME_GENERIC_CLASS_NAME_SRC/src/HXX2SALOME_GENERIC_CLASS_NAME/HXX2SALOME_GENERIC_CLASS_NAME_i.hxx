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
