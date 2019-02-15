#ifndef ALGLIN_I_
#define ALGLIN_I_

#include <alglin.hh>
#include "alglin.hxx"

class AlgLin_i : public POA_Distant::AlgLin,
		 public PortableServer::RefCountServantBase {

private:

  alglin A_interne;

public:

  AlgLin_i();
  virtual ~AlgLin_i();

  void addvec(Distant::vecteur_out C, 
	      const Distant::vecteur& A, 
	      const Distant::vecteur& B);

  CORBA::Double prdscl(const Distant::vecteur& A, 
		       const Distant::vecteur& B);

};

#endif
