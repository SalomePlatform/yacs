
Annexe : traitement des commons fortran et variables globales C/C++
===================================================================


Common fortran
--------------

Cette section résulte de réflexions menées par Marc Boucker, Alexandre Douce,
Céline Béchaud et l'auteur (pour plus de détails, voir [COMMON]_). On ne prétend
pas ici présenter un état complet  des situations possibles.    Les codes
fortran 77 que l'on veut commander depuis l'interpréteur python   ou depuis
Corba/Salomé, définissent souvent des zones mémoires partagées  entre les
différentes fonctions fortran, appelées "common".    Exemple :    Les fonctions
``f1`` et ``f2`` utilisent la même zone mémoire   ``a`` dans le common ``C``.

.. _f1:


``f1.f``

.. include:: ./exemples/exemple11/v1/f1.f
   :literal:

.. _f2:


``f2.f``

.. include:: ./exemples/exemple11/v1/f2.f
   :literal:

Si les deux fonctions sont contenues dans le même composant et le  common ``C``
n'est pas utilisé par des fonctions d'autres composants,  le common n'est pas
visible de l'extérieur du composant et "tout se  passe bien" (voir figure
:ref:`Utilisation d'un common dans un composant <figcommon0>`).    Si le concepteur du composant veut permettre la lecture
et/ou l'écriture  du common depuis la couche python et/ou CORBA, il pourra
écrire facilement  des fonctions d'accès (par exemple fonctions ``setCommon`` et
``getCommon``  dans l'exemple ci-après).


``common.f``

.. include:: ./exemples/exemple11/v1/common.f
   :literal:

.. _figcommon0:


.. image:: images/common0.png
   :width: 34ex
   :align: center

.. centered::
   Utilisation d'un common dans un composant

On donne ici un exemple d'encapsulation dans C++, puis dans python (via swig) :


``f.hxx``

.. include:: ./exemples/exemple11/v1/f.hxx
   :literal:


``modf.i``

.. include:: ./exemples/exemple11/v1/modf.i
   :literal:

Un exemple d'utilisation :


.. include:: ./exemples/exemple11/v1/resultats.txt
   :literal:

Si le common ``C`` est utilisé dans plusieurs librairies dynamiques, la gestion
est plus délicate. De façon générale, on ne peut pas supposer que le common
utilisé par chacune des librairies est situé à la méme adresse mémoire.    Il y
a deux situations types que l'on peut rencontrer :

#. Les deux composants sont montés localement depuis un même interpréteur
   python :

     .. _figcommon1:


     .. image:: images/common1.png
        :width: 42ex
        :align: center

     .. centered::
        Utilisation d'un common partagé entre deux librairies - version locale

#. Les deux composants sont montés dans des espaces mémoire  différents (sur la
   même machine ou des machines différentes)  via le mécanisme Salomé (conteneurs)
   :

     .. _figcommon2:


     .. image:: images/common2.png
        :width: 47ex
        :align: center

     .. centered::
        Utilisation d'un common partagé entre deux librairies - version distribuée

Dans les deux cas, il faut prévoir des fonctions de synchronisation  (par
exemple en utilisant les fonctions de lecture/écriture des commons  depuis la
couche de commande python et/ou Salomé).    L'adaptation au cas des deux
composants locaux chargés depuis un interpréteur  python s'écrit :

#. Pour le premier composant :


   ``f1.hxx``

   .. include:: ./exemples/exemple11/v2/f1.hxx
      :literal:


   ``modf1.i``

   .. include:: ./exemples/exemple11/v2/modf1.i
      :literal:

#. Pour le second composant :


   ``f2.hxx``

   .. include:: ./exemples/exemple11/v2/f2.hxx
      :literal:


   ``modf2.i``

   .. include:: ./exemples/exemple11/v2/modf2.i
      :literal:

#. Les fonctions de lecture d'écriture du common seront incorporées  dans chaque
   composant.

Un exemple d'utilisation


.. include:: ./exemples/exemple11/v2/resultats.txt
   :literal:

En résumé,     si un code existant comportant des commons doit être découpé en
plusieurs  composants, on peut soit :

* modifier le code en enlevant les commons et en faisant transiter  les
  informations via les listes de paramètres des fonctions;

* écrire des fonctions d'accès en lecture/écriture aux commons  et utiliser ces
  fonctions de lecture/écriture depuis les couches  supérieures aux composant de
  façon à synchroniser les état internes des  différents composants.

La première solution demande d'intervenir en profondeur dans le code  fortran,
la seconde exige que l'utilisateur synchronise explicitement  les commons dans
les différents composants.    En ce qui concerne les nouveaux codes fortran, on
déconseille vivement  l'utilisation des commons.


Variables globales C/C++
------------------------

La situation est analogue au cas des commons : chaque composant  aura son propre
jeu de variables globales. Il faudra d'une façon  ou d'une autre assurer la
cohérence de ces différents jeux  de variables.

