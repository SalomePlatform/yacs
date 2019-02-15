#ifndef ALGLIN_I_
#define ALGLIN_I_

#include "alglin.hxx"
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(alglin)
#include "SALOME_Component_i.hxx"

class AlgLin_i : public POA_Engines::AlgLin,
		 public Engines_Component_i {

private:

  alglin A_interne;

public:

  AlgLin_i(CORBA::ORB_ptr orb,
	   PortableServer::POA_ptr poa,
	   PortableServer::ObjectId * contId, 
	   const char *instanceName,
	   const char *interfaceName);

  virtual ~AlgLin_i();

  void addvec(Engines::vecteur_out C, 
	      const Engines::vecteur& A, 
	      const Engines::vecteur& B);

  CORBA::Double prdscl(const Engines::vecteur& A, 
		       const Engines::vecteur& B);

  Engines::vecteur* create_vector(CORBA::Long n);
  void destroy_vector(const Engines::vecteur& V);
  
};


extern "C"
PortableServer::ObjectId * 
     AlgLinEngine_factory(CORBA::ORB_ptr orb ,
			  PortableServer::POA_ptr poa , 
			  PortableServer::ObjectId * contId ,
			  const char *instanceName ,
			  const char *interfaceName );

#endif
