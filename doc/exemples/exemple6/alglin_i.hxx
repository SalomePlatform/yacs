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
