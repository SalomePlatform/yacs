#include "SalomeResourceModel.hxx"
#include "RuntimeSALOME.hxx"
#include "SALOME_ResourcesManager.hxx"
#include "SALOME_LifeCycleCORBA.hxx"
#include <ctime>

SalomeResourceModel::SalomeResourceModel()
: AbstractResourceModel(),
  _wantedMachine("localhost"),
  _nbProcs(1),
  _hours(0),
  _minutes(5),
  _wcKey("P11U50:CARBONES"),
  _localDirectory("/tmp"),
  _remoteDirectory("/tmp"),
  _parallelizeStatus(true),
  _in_files()
{
}

SalomeResourceModel::~SalomeResourceModel()
{
}

bool SalomeResourceModel::isMachineInteractive(const std::string& machine)const
{
  return !machine.compare("localhost");
}

std::vector< std::string > SalomeResourceModel::getFittingMachines()const
{
  std::vector< std::string > resultat;
  YACS::ENGINE::RuntimeSALOME* runTime = YACS::ENGINE::getSALOMERuntime();
  CORBA::ORB_ptr orb = runTime->getOrb();
  if (!orb) return resultat;

  SALOME_NamingService namingService(orb);
  SALOME_LifeCycleCORBA lcc(&namingService);

  CORBA::Object_var obj;
  obj=namingService.Resolve(SALOME_ResourcesManager::_ResourcesManagerNameInNS);
  if (CORBA::is_nil(obj)) return resultat;

  Engines::ResourcesManager_var resManager;
  resManager = Engines::ResourcesManager::_narrow(obj);
  if(!resManager) return resultat;

  Engines::ResourceParameters params;
  lcc.preSet(params);

  Engines::ResourceList* resourceList = resManager->GetFittingResources(params);

  for (int i = 0; i < resourceList->length(); i++)
  {
    const char* aResource = (*resourceList)[i];
    resultat.push_back(aResource);
  }
  return resultat;
}

void SalomeResourceModel::setWantedMachine(const std::string& v)
{
  _wantedMachine = v;
}

std::string SalomeResourceModel::getWantedMachine()const
{
  return _wantedMachine;
}

void SalomeResourceModel::setNbprocs(unsigned int v)
{
  _nbProcs = v;
}

unsigned int SalomeResourceModel::getNbprocs()const
{
  return _nbProcs;
}

void SalomeResourceModel::setMaxDurationHours(int v)
{
  _hours = v;
}

int SalomeResourceModel::getMaxDurationHours()const
{
  return _hours;
}

void SalomeResourceModel::setMaxDurationMinutes(int v)
{
  _minutes = v;
}

int SalomeResourceModel::getMaxDurationMinutes()const
{
  return _minutes;
}

void SalomeResourceModel::setWckey(const std::string& v)
{
  _wcKey = v;
}

std::string SalomeResourceModel::getWckey()const
{
  return _wcKey;
}

void SalomeResourceModel::setLocalDir(const std::string& v)
{
  _localDirectory = v;
}

std::string SalomeResourceModel::getLocalDir()const
{
  return _localDirectory;
}

void SalomeResourceModel::setRemoteDir(const std::string& v)
{
  _remoteDirectory = v;
}

std::string SalomeResourceModel::getRemoteDir()const
{
  return _remoteDirectory;
}

std::string SalomeResourceModel::getDefaultRemoteDir(std::string machine)const
{
  std::string result = "";
  YACS::ENGINE::RuntimeSALOME* runTime = YACS::ENGINE::getSALOMERuntime();
  CORBA::ORB_ptr orb = runTime->getOrb();
  if (!orb) return result;

  SALOME_NamingService namingService(orb);
  SALOME_LifeCycleCORBA lcc(&namingService);

  CORBA::Object_var obj;
  obj=namingService.Resolve(SALOME_ResourcesManager::_ResourcesManagerNameInNS);
  if (CORBA::is_nil(obj)) return result;

  Engines::ResourcesManager_var resManager;
  resManager = Engines::ResourcesManager::_narrow(obj);
  if(!resManager) return result;

  Engines::ResourceParameters params;
  lcc.preSet(params);
  
  try
  {
    Engines::ResourceDefinition * rd;
    rd = resManager->GetResourceDefinition(machine.c_str());
    result = rd->working_directory.in();
    if (result != "")
    {
      time_t rawtime;
      time(&rawtime);
      std::string launch_date = ctime(&rawtime);
      for (int i = 0; i < launch_date.size(); i++)
        if (launch_date[i] == '/' ||
            launch_date[i] == '-' ||
            launch_date[i] == ':' ||
            launch_date[i] == ' ')
          launch_date[i] = '_';
      launch_date.erase(--launch_date.end()); // Last caracter is a \n
      result += "/run_";
      result += launch_date;
    }
  }
  catch (...)
  {
  }

  return result;
}

void SalomeResourceModel::setParallelizeStatus(bool v)
{
  _parallelizeStatus = v;
}

bool SalomeResourceModel::getParallelizeStatus()const
{
  return _parallelizeStatus;
}

const std::list<std::string>& SalomeResourceModel::getInFiles()const
{
  return _in_files;
}

std::list<std::string>& SalomeResourceModel::getInFiles()
{
  return _in_files;
}