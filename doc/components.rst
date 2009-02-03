
:tocdepth: 3

.. _components:

Integration de composants dans l'environnement Salome
=============================================================

Ce document décrit les opérations à effectuer pour intégrer des codes (de calcul
par exemple) dans l'architecture Salomé.     

On peut envisager plusieurs scénarios d'intégration:

* Le plus simple est le pilotage local du code depuis un interpréteur   python.
  Le code ne fonctionnera que sur une machine.

* Le stade suivant consiste à pouvoir piloter le code localement ou   à
  distance.    L'architecture retenue consiste à exposer le code comme un serveur
  CORBA [CORBA]_, capable de recevoir et d'exécuter des requêtes de clients CORBA.
  Salomé offre un certain nombre de services permettant de simplifier
  l'utilisation de CORBA.

* Enfin, l'intégration proprement dite dans Salomé consiste à fournir à Salomé
  toutes les informations nécessaires au pilotage de code, par exemple depuis
  l'interface homme-machine ou via la supervision par exemple.

On envisagera plusieurs situations types, sachant qu'il y aura toujours  des cas
particuliers qui devront être traités en adaptant les procédures décrites dans
ce document.    Les situations types envisagées sont considérées suivant la
forme du code disponible:

* exécutable binaire,

* librairie binaire,

* code source f77/C/C++,

* code source python.

Dans la mesure du possible, on donnera des exemples des différentes situations
rencontrées.

.. toctree::
   :maxdepth: 2

   etapes.rst
   compInterne.rst
   accesLocal.rst
   accesCorba.rst
   salome.rst
   annexe_common.rst
   references.rst

