//  Copyright (C) 2006-2010  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
