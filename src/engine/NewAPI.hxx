#include <string>

// ça sera peut-être une struct s'il n'y a rien d'autre que les deux paramètres
class Resource
{
public:
    unsigned int nbCores()const;
    const std::string& name()const;
};

// ça sera peut-être une struct s'il n'y a rien d'autre que les deux paramètres
class ContainerType
{
public:
  const std::string& name()const;
  // float pour pouvoir gérer plus tard des containers qui peuvent être lancé
  // à plusieurs sur le même coeur.
  const float parallelism()const;
  // normalement, pas besoin de start. Quand yacs lance un traitement, il
  // commence par faire un "load" qui cherche le container avec les
  // caractéristiques données et il lance un nouveau s'il ne le trouve pas.
  // Donc on peut s'appuyer sur le mécanisme existant et jouer sur les
  // caractéristiques pour identifier le container.
  //virtual void start(const std::string& resourceName, unsigned int index);
};

// probablement une struct
class Container
// Cet objet est créé par le WorkloadManager chaque fois qu'une tâche peut être
// lancée et il contient l'identification du container à utiliser pour faire
// le traitement. voir Task.run
{
public:
  const Resource* resource()const;
  const ContainerType* type()const;
  // indice du container parmi ceux du même type sur la resource:
  unsigned int index()const;
};

class Task
// Cet objet doit être créé par YACS et soumis au WorkloadManager
{
public:
  const ContainerType* type()const;
  // Cette méthode est à implémenter dans YACS. Elle est appelée par le
  // WorkloadManager au moment où une ressource est disponible et en paramétre
  // on trouve l'identification du container à utiliser. YACS permet déjà
  // d'identifier les containers par leur nom. A partir des informations de
  // l'objet en paramètre on peut construire un nom de container qui sera unique.
  // Avec ce nom, on peut utiliser les méthodes existantes pour charger le
  // container salome / kernel et lancer le traitement à faire dans le noeud
  // YACS.
  // Les traitements à faire dans cette fonction sont: trouver le container
  // kernel et si besoin l'initialiser, lancer le traitement du noeud et
  // attendre la fin du calcul.
  virtual void run(const Container& container)=0;
};

class WorkloadManager
// Cet objet doit être créé par YACS à chaque exécution d'un schéma.
{
public:
  // Fonction à appeler par YACS avant le début de l'exécution du schéma pour
  // renseigner les ressources disponibles. A mon avis, on peut se limiter dans
  // un premier temps de prendre toutes les ressources du catalogue qui peuvent
  // lancer des containers. Cela suffit pour le lancement sur cluster. En local
  // on pourrait avoir envie de ne pas utiliser tous ce qu'il y a dans le
  // catalogue, mais on n'a qu'à modifier le catalogue à la main si on veut
  // faire le malin.
  void addResource(Resource* r);
  
  // Fonction à appeler par YACS au moment où un noeud de traitement est prêt à
  // être lancé. Cet appel ajoute la tâche dans la queue d'attente et rend tout
  // de suite la main. La fonction 'run' de la tâche sera appelée plus tard, au
  // moment où une ressource est effectivement disponible.
  void addTask(Task* t);
  
  // Fonction à appeler par YACS pour démarrer le moteur qui distribue les tâches.
  // Il y aura au moins deux fils d'exécution: un qui gère les arrivées dans la
  // queue et un qui gère les fins de tâche. Ce sont ces deux fils qui démarrent
  // ici. Après, chaque exécution de tâche aura son propre fil.
  void start();
  
  // Fonction à appeler par YACS pour indiquer qu'il n'y aura plus de tâches à
  // soumettre. Le moteur pourra s'arrêter à la fin des tâches en cours.
  void stop();
};
