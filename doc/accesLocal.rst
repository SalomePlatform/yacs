
.. _secacceslocal:

Composant à accès local
=======================


Principe
--------

Comme indiqué au chapitre :ref:`secetapes`,
l'objet interne construit au chapitre :ref:`seccompinterne` 
peut être manipulé   depuis un interpréteur python
local, suivant le schéma ci-après.

.. _figacceslocal2:


.. image:: images/accesLocal.png
   :width: 46ex
   :align: center

.. centered::   Accès depuis un interpréteur python local

Dans le cas d'un objet interne C++, il faudra écrire une interface python-C++
pour obtenir un composant local.  L'écriture de cette interface fera l'objet de
la section suivante.    Dans le cas d'un objet interne python, il n'y a rien à
faire : l'objet interne  python peut être utilisé tel quel comme composant
local.


A partir d'un objet interne python
----------------------------------

Si l'objet interne est implémenté comme objet python, il n'est pas nécessaire
d'introduire une interface supplémentaire.


A partir d'un objet interne C++
-------------------------------

Pour pouvoir utiliser un objet C++ à partir d'un interpréteur python, il faut
passer par une interface python/C++. Cette interface peut être codée par
l'intégrateur ou être générée (semi-) automatiquement à l'aide d'outils  tels
que swig [SWIG]_ ou boost [BOOST]_.    On envisagera ici la génération de
l'interface à l'aide de swig, à l'aide  d'un exemple simple. Pour le traitement
de cas particuliers, on renvoit  à la documentation de swig, voire à la
documentation de python.


Fichier d'interface swig
^^^^^^^^^^^^^^^^^^^^^^^^

La procédure standard pour utiliser swig est d'écrire un **fichier d'interface**
(se terminant par ``.i``).  Ce fichier d'interface ressemble fortement à une
fichier d'interface C++  (voir par exemple ``vecteur.hxx`` ou
``FreeFem.hxx``).    Il contient toutes les déclarations C++
(structures, fonctions, classes,   constantes, ...) que l'intégrateur veut
"exporter" au niveau python.   Dans le cas des classes  C++, seule la partie
publique des classes peut être indiquée dans  le fichier d'interface.    Des
exemples seront donnés plus loin.


Processus de génération du code de l'interface C++/python
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Règle**
  Les extensions au langage python, écrites en langage C/C++/f77 (langages
  compilés autres que python), doivent être compilées sous forme de librairies
  dynamiques (``.so`` sous unix, ``.dll`` sous windows).  Depuis l'interpréteur
  python, on chargera ces extensions au moyen de la   commande ``import``.    

Tous les composants à intégrer seront donc compilés sous forme de librairie
dynamique, ce qui impliquera une procédure particulière pour l'utilisation  des
outils de débogages (voir plus loin).    Les différentes opérations à effectuer
et les fichiers intervenant   dans le processus sont schématisés par la figure
suivante.

.. _processusswig:


.. image:: images/accesLocalCpp.png
   :width: 45ex
   :align: center

.. centered::   Interface via swig


Exemple 5 (première version)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Si on veut accéder depuis un interpréteur python local à la classe  ``alglin``,
on écrira un fichier d'interface du type :

.. _alglin.i.v1:


``alglin.i``

.. include:: ./exemples/exemple5/v1/alglin.i
   :literal:

Les différentes lignes signifient :     ::

   %module AlgLinModule

Définit le nom du module python.   Pour importer les définitions du composant
depuis un interpréteur python, on écrira : ``import AlgLinModule``.     ::

   %{
   #include "alglin.hxx"
   %}

Entre les lignes ``%{`` et ``%}``, on indique les déclarations  C++ dont le code
de l'interface C++/python aura besoin (sinon le  fichier C++ généré par swig ne
compilera pas).  Typiquement, on inclut ici le fichier d'interface de l'objet
interne C++  construit au chapitre précédent.     ::

   class alglin {
   public:
     alglin ();
     ~alglin ();
     void      addvec (vecteur *C, vecteur *A, vecteur *B);
     double    prdscl (vecteur *A, vecteur *B);
     vecteur * create_vector (long n);
     void      destroy_vector (vecteur *V);
   
   };

On indique dans le reste du fichier ``alglin.i``, les classes et  définitions
que l'on désire exporter vers l'interpréteur python.    Exemple d'usage de
l'interface générée :


.. include:: ./exemples/exemple5/v1/sortie.txt
   :literal:

**Remarques**

  #. Par rapport à la déclaration de la classe C++ (fichier ``alglin.hxx``), on a
     introduit un constructeur (``alglin()``) et un  destructeur (``~alglin()``).
     Dans la classe C++ de l'objet interne, ces constructeur et destructeur  ne sont
     pas nécessaires (l'objet interne n'a pas besoin d'être initialisé  à sa création
     et ne gère pas de mémoire dynamique C++). Le compilateur  fournit un
     constructeur et un destructeur par défaut dans ce cas.    Pour le fichier
     d'interface swig, par contre, **il faut** déclarer  explicitement un
     constructeur et un destructeur pour que  python puisse gérer correctement la
     mémoire C++ (i.e. quand  un objet python est créé/détruit, l'objet C++ interne
     soit aussi  créé/détruit "proprement").

  #. Remarquer que dans le fichier d'interface ``alglin.i``, on n'explicite  pas
     la définition de la structure/classe ``vecteur``.    Les objets de type
     ``vecteur`` seront vus depuis l'interpréteur  python comme des objets "boîte
     noire" (on connait leur type  et leur emplacement mémoire mais pas les
     méthodes/attributs associés).    Si on essaie d'appeler une méthode sur un objet
     vecteur, on obtient  le message d'erreur suivant :
  
.. include:: ./exemples/exemple5/v1/sortie2.txt
   :literal:


.. epigraph::

   La deuxième version de cet exemple (ci-après) corrigera ce problème.

   .. % 


Exemple 5 (deuxième version)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

La première version de l'exemple souffre de deux défauts (parmi d'autres ...),
concernant les objets de type vecteur :

* on n'a pas accès aux méthodes ni aux attributs d'objets de la  classe vecteur
  (cf. la seconde remarque ci-dessus),

* rien n'est prévu pour initialiser/modifier les coefficients  contenus dans un
  objet vecteur.

swig permet d'enrichir le fichier d'interface ``alglin.i`` 
pour ajouter les fonctionnalités manquantes :

.. _alglin.i.v2:


``alglin.i (version 2)``

.. include:: ./exemples/exemple5/v2/alglin.i
   :literal:

**Remarque**
  Par rapport à la version précédente, on a la déclaration de
  la classe vecteur, ce qui donne par exemple accès à la taille  des vecteurs et à
  une "poignée" sur les coefficients (mais pas sur les  coefficients
  individuellement).    La troisième version corrigera ce défaut.    Un exemple
  d'utilisation du composant (et de la limitation sur l'accès aux vecteurs) est :

.. include:: ./exemples/exemple5/v2/sortie.txt
   :literal:


Exemple 5 (troisième version)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

La deuxième version de l'exemple permet de "voir" les objets de type vecteur
mais seulement en "surface". En particulier, on n'a pas accès aux coefficients
individuellement depuis l'interpréteur python.    La troisième version, en
ajoutant dans l'interface ``alglin.i``   des fonctions utilitaires
(``__setitem__`` et ``__getitem__``)   permet de simuler (partiellement) depuis
la couche python de véritables vecteurs de coefficients.    

**Remarque**
  On a aussi ajouté
  une fonction d'affichage\ ``__str__``, qui permet,   quand on exécute ``print
  v`` depuis l'interpréteur, d'afficher la   liste des coefficients de v.

.. _alglin.i.v3:


``alglin.i (version 3)``

.. include:: ./exemples/exemple5/v3/alglin.i
   :literal:

Un exemple d'utilisation du composant (y compris l'accès aux vecteurs) est :


.. include:: ./exemples/exemple5/v3/sortie.txt
   :literal:

