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

/* choix utilisateur */
// nombre de ressources de parrallelisme
/* simplex */
// #define NBNODE 1
// #define NBNODE 4
// #define NBNODE 16
// #define NBNODE 64
// #define NBNODE 256
// #define NBNODE 1024
#define NBNODE 2
/* essaim particulaire */
#define PLAN 36
#define COTE 6
// nombre maximum d'evaluation
#define NBEVAL 100000

// choix de la fonction a evaluer
// #define ROSENBROCK
// #define TRIPOD
// #define GRIEWANK
//

#define ROSENBROCK

/* constante dépendant du choix de l'évaluateur */

// nombre de paramètre
#ifdef GRIEWANK
#define NBGENE   30
#else
#define NBGENE   2
#endif

// domaine de recherche
#ifdef ROSENBROCK
#define BORNEMIN    -2.0
#define BORNEMAX    2.0
#else
#define BORNEMIN    -100.0
#define BORNEMAX    100.0
#endif

