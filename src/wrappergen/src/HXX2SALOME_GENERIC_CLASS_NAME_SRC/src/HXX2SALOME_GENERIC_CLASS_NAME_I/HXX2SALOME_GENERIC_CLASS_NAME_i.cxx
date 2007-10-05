#include "HXX2SALOME_GENERIC_CLASS_NAME_i.hxx"
//  HXX2SALOME_CPP_INCLUDE
using namespace std;
#include <string>

#ifdef USE_MED
#include "FIELDClient.hxx"
#include "MESHClient.hxx"
#include "MEDMEM_Support_i.hxx"
#include "MEDMEM_Mesh_i.hxx"
#include "MEDMEM_FieldTemplate_i.hxx"
#endif

#include "SenderFactory.hxx"
#include "MultiCommException.hxx"
#include "ReceiverFactory.hxx"
#include "SALOME_Matrix_i.hxx"
#include "MatrixClient.hxx"

//=============================================================================
/*!
 *  standard constructor
 */
//=============================================================================
HXX2SALOME_GENERIC_CLASS_NAME_i::HXX2SALOME_GENERIC_CLASS_NAME_i(CORBA::ORB_ptr orb,
	PortableServer::POA_ptr poa,
	PortableServer::ObjectId * contId, 
	const char *instanceName, 
	const char *interfaceName) :
  Engines_Component_i(orb, poa, contId, instanceName, interfaceName),cppCompo_(new HXX2SALOME_GENERIC_CLASS_NAME)
{
  MESSAGE("activate object");
  _thisObj = this ;
  _id = _poa->activate_object(_thisObj);
}

HXX2SALOME_GENERIC_CLASS_NAME_i::~HXX2SALOME_GENERIC_CLASS_NAME_i()
{
}

//  HXX2SALOME_CXX_CODE


extern "C"
{
  PortableServer::ObjectId * HXX2SALOME_GENERIC_CLASS_NAMEEngine_factory(
			       CORBA::ORB_ptr orb,
			       PortableServer::POA_ptr poa, 
			       PortableServer::ObjectId * contId,
			       const char *instanceName, 
		       	       const char *interfaceName)
  {
    MESSAGE("PortableServer::ObjectId * HXX2SALOME_GENERIC_CLASS_NAMEEngine_factory()");
    SCRUTE(interfaceName);
    HXX2SALOME_GENERIC_CLASS_NAME_i * myHXX2SALOME_GENERIC_CLASS_NAME 
      = new HXX2SALOME_GENERIC_CLASS_NAME_i(orb, poa, contId, instanceName, interfaceName);
    return myHXX2SALOME_GENERIC_CLASS_NAME->getId() ;
  }
}
