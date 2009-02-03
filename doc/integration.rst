.. _integration:

**********************************************************
Intégration de composants dans SALOME pour YACS
**********************************************************

Cette partie de la documentation donne les éléments pour créer des composants SALOME couplables avec YACS 
soit en partant de zéro soit en intégrant un code existant.

Les deux premiers chapitres sont des tutoriaux pour créer des modules SALOME avec un composant simple de type
"Hello World" implémentés en Python (:ref:`pysalome`) ou en C++ (:ref:`cppsalome`).

Le chapitre suivant (:ref:`components`) décrit les opérations à effectuer pour intégrer des codes de calcul
dans l'architecture SALOME.
Le quatrième chapitre (:ref:`calculator`) montre comment créer un composant SALOME qui utilise des objets MED.

Le chapitre suivant présente l'outil :ref:`hxx2salome` qui permet d'automatiser, dans une grande mesure,
l'intégration d'un code de calcul en C++ à condition de n'utiliser que des ports dataflow.

Le sixième chapitre (:ref:`progdsc`) est réservé à ceux qui veulent développer de nouveaux ports datastream
sur la base du modèle de programmation DSC.

Le chapitre suivant est un :ref:`calcium`. Les ports CALCIUM sont des ports datastream prédéfinis dans SALOME
qui permettent de mettre en oeuvre simplement des couplages datastream dans YACS.

Enfin le dernier chapitre est un guide d'utilisation de l'outil :ref:`yacsgen` qui permet d'automatiser
l'intégration de codes de calcul en Fortran, C, Python qui utilisent des ports datastream CALCIUM ce que ne permet
pas l'outil :ref:`hxx2salome`.

.. toctree::
   :maxdepth: 2

   pysalome
   cppsalome
   components
   calculator
   hxx2salome
   progdsc
   calcium
   yacsgen
