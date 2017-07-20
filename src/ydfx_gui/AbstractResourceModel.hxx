#ifndef ABSTRACTRESOURCEMODEL_HXX
#define ABSTRACTRESOURCEMODEL_HXX

#include <string>
#include <vector>
#include <list>
#include "ydfxwidgetsExport.hxx"

class YDFXWIDGETS_EXPORT AbstractResourceModel
{
public:

  AbstractResourceModel();
  virtual ~AbstractResourceModel();

  virtual bool getParallelizeStatus()const =0;
  virtual void setParallelizeStatus(bool v)=0;

  virtual std::string getRemoteDir()const =0;
  virtual std::string getDefaultRemoteDir(std::string machine)const =0;
  virtual void setRemoteDir(const std::string& v)=0;

  virtual std::string getLocalDir()const =0;
  virtual void setLocalDir(const std::string& v)=0;

  virtual std::string getWckey()const =0;
  virtual void setWckey(const std::string& v)=0;

  virtual int getMaxDurationMinutes()const =0;
  virtual void setMaxDurationMinutes(int v)=0;

  virtual int getMaxDurationHours()const =0;
  virtual void setMaxDurationHours(int v)=0;
  std::string getMaxDuration()const ;

  virtual unsigned int getNbprocs()const =0;
  virtual void setNbprocs(unsigned int v)=0;

  virtual std::string getWantedMachine()const =0;
  virtual void setWantedMachine(const std::string& v)=0;
  virtual std::vector<std::string> getFittingMachines()const =0;

  virtual const std::list<std::string>& getInFiles()const =0;
  virtual std::list<std::string>& getInFiles() =0;

  virtual bool isMachineInteractive(const std::string& machine)const =0;
};

#endif // ABSTRACTRESOURCEMODEL_HXX
