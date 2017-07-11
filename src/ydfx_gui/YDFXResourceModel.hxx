#ifndef YDFXRESOURCEMODEL_H
#define YDFXRESOURCEMODEL_H

#include "AbstractResourceModel.hxx"

class YACSEvalYFX;
class YACSEvalListOfResources;
class YACSEvalParamsForCluster;

class YDFXWIDGETS_EXPORT YDFXResourceModel : public AbstractResourceModel
{
public:
    
  YDFXResourceModel(YACSEvalYFX* eval);
  virtual ~YDFXResourceModel();

  virtual bool getParallelizeStatus()const;
  virtual void setParallelizeStatus(bool v);

  virtual std::string getRemoteDir()const;
  virtual std::string getDefaultRemoteDir(std::string machine)const;
  virtual void setRemoteDir(const std::string& v);

  virtual std::string getLocalDir()const;
  virtual void setLocalDir(const std::string& v);

  virtual std::string getWckey()const;
  virtual void setWckey(const std::string& v);

  virtual int getMaxDurationMinutes()const;
  virtual void setMaxDurationMinutes(int v);

  virtual int getMaxDurationHours()const;
  virtual void setMaxDurationHours(int v);

  virtual unsigned int getNbprocs()const;
  virtual void setNbprocs(unsigned int v);

  virtual std::string getWantedMachine()const;
  virtual void setWantedMachine(const std::string& v);
  virtual std::vector<std::string> getFittingMachines()const;

  virtual const std::list<std::string>& getInFiles()const;
  virtual std::list<std::string>& getInFiles();

  virtual bool isMachineInteractive(const std::string& machine)const;

  void reset(YACSEvalYFX* eval);

private:

  YACSEvalListOfResources * resources();
  const YACSEvalListOfResources * resources()const;
  YACSEvalParamsForCluster& getClusterParams();
  const YACSEvalParamsForCluster& getClusterParams()const;
  void getMaxDuration(int& hours, int& minutes)const;
  void setMaxDuration(int hours, int minutes);

private:
  YACSEvalYFX* _eval;
};

#endif // YDFXRESOURCEMODEL_H
