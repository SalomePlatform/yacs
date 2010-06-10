#include "alglin_i.hxx"
#include <iostream>


AlgLin_i:: AlgLin_i(CORBA::ORB_ptr orb,
		    PortableServer::POA_ptr poa,
		    PortableServer::ObjectId * contId, 
		    const char *instanceName, 
		    const char *interfaceName) :
  Engines_Component_i(orb, poa, contId, instanceName, interfaceName, 1)
{
  MESSAGE("activate object");
  _thisObj = this ;
  _id = _poa->activate_object(_thisObj);
}


AlgLin_i::~AlgLin_i()
{
}

void AlgLin_i::addvec(Engines::vecteur_out C,
		      const Engines::vecteur& A, 
		      const Engines::vecteur& B)
{
  beginService("addvec");

  long i, n = A.length();

  if (n != B.length())
    sendMessage("warning", "vecteurs de longueur differente");

  vecteur A_(n);
  vecteur B_(n);
  vecteur C_(n);

  double *xA = A_.x();
  double *xB = B_.x();
  double *xC = C_.x();

  for (i=0; i<n; i++) {
    xA[i] = A[i];
    xB[i] = B[i];
  }

  A_interne.addvec(&C_, &A_, &B_);

  C = new Engines::vecteur;
  C->length(n);
  for (i=0; i<n; i++) {
    (*C)[i] = xC[i];
  }

  endService("addvec");
}

CORBA::Double AlgLin_i::prdscl(const Engines::vecteur& A, 
			       const Engines::vecteur& B)
{
  beginService("prdscl");

  long i, n = A.length();

  if (n != B.length())
    sendMessage("warning", "vecteurs de longueur differente");

  vecteur A_(n);
  vecteur B_(n);

  double *xA = A_.x();
  double *xB = B_.x();

  for (i=0; i<n; i++) {
    xA[i] = A[i];
    xB[i] = B[i];
  }

  CORBA::Double d =  A_interne.prdscl(&A_, &B_); 

  endService("prdscl");
  return d;
}

extern "C"
{
  PortableServer::ObjectId * AlgLinEngine_factory(
                               CORBA::ORB_ptr orb,
                               PortableServer::POA_ptr poa,
                               PortableServer::ObjectId * contId,
                               const char *instanceName,
                               const char *interfaceName)
  {
    MESSAGE("PortableServer::ObjectId * AlgLinEngine_factory()");
    AlgLin_i * O
      = new AlgLin_i(orb, poa, contId, instanceName, interfaceName);
    return O->getId() ;
  }
}

