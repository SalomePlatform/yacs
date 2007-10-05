/* --- Ansi C --- */
/* --- coding: latin_1 ---

    File
      creation : 2007-06-27.09.30.21
      revision : $Id$

    Copyright © 2007 Commissariat à l'Energie Atomique
      par Gilles ARNAUD (DM2S/SFME/LETR)
        C.E. Saclay; Bat 454; 91191 GIF/YVETTE CEDEX; France
        Tel: 01 69 08 38 86; Fax : 33 1 69 08 85 68 
        Gilles.Arnaud@cea.fr
 
    Object
      paramétrage utilisateur
 
___________________________________________________________________*/

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

