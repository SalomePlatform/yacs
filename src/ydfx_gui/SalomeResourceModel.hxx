#ifndef SALOMERESOURCEMODEL_HXX
#define SALOMERESOURCEMODEL_HXX

#include "AbstractResourceModel.hxx"


class YDFXWIDGETS_EXPORT SalomeResourceModel : public AbstractResourceModel
{
public:
  SalomeResourceModel();
  ~SalomeResourceModel();
  virtual bool isMachineInteractive(const std::string& machine)const;
  virtual std::vector< std::string > getFittingMachines()const;
  virtual void setWantedMachine(const std::string& v);
  virtual std::string getWantedMachine()const;
  virtual void setNbprocs(unsigned int v);
  virtual unsigned int getNbprocs()const;
  virtual void setMaxDurationHours(int v);
  virtual int getMaxDurationHours()const;
  virtual void setMaxDurationMinutes(int v);
  virtual int getMaxDurationMinutes()const;
  virtual void setWckey(const std::string& v);
  virtual std::string getWckey()const;
  virtual void setLocalDir(const std::string& v);
  virtual std::string getLocalDir()const;
  virtual void setRemoteDir(const std::string& v);
  virtual std::string getRemoteDir()const;
  virtual std::string getDefaultRemoteDir(std::string machine)const;
  virtual void setParallelizeStatus(bool v);
  virtual bool getParallelizeStatus()const;
  virtual const std::list<std::string>& getInFiles()const;
  virtual std::list<std::string>& getInFiles();

private:
  std::string _wantedMachine;
  unsigned int _nbProcs;
  int _hours;
  int _minutes;
  std::string _wcKey;
  std::string _localDirectory;
  std::string _remoteDirectory;
  bool _parallelizeStatus;
  std::list<std::string> _in_files;
};

#endif // SALOMERESOURCEMODEL_HXX
