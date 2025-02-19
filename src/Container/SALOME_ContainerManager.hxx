// Copyright (C) 2007-2025  CEA, EDF, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#pragma once

#include "SALOME_Container.hxx"

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_Component)
#include CORBA_CLIENT_HEADER(SALOME_ContainerManager)
#include "SALOME_ResourcesCatalog_Parser.hxx"

#include "Utils_Mutex.hxx"

#include <vector>
#include <string>
#include <utility>
#include <functional>
#include <set>

class SALOME_NamingService_Abstract;
class SALOME_ResourcesManager_Client;

class CONTAINER_EXPORT SALOME_ContainerManager : public POA_Engines::ContainerManager
{

public:
  SALOME_ContainerManager(CORBA::ORB_ptr orb, PortableServer::POA_var poa, SALOME_NamingService_Abstract *ns);
  ~SALOME_ContainerManager();

  // Corba Methods
  Engines::Container_ptr GiveContainer(const Engines::ContainerParameters& params) override;

  void ShutdownContainers() override;

  void ShutdownContainersNow() override;

  void SetOverrideEnvForContainers(const Engines::KeyValDict& env) override;

  Engines::KeyValDict *GetOverrideEnvForContainers() override;

  void SetCodeOnContainerStartUp(const char *code) override;

  char *GetCodeOnContainerStartUp() override;

  // C++ Methods
  void Shutdown();

  void DeclareUsingSalomeSession() { _isSSL = false; }

  CORBA::Long GetTimeOutToLaunchServerInSecond() override;

  void SetTimeOutToLaunchServerInSecond(CORBA::Long timeInSecond) override;

  CORBA::Long GetDeltaTimeBetweenNSLookupAtLaunchTimeInMilliSecond() override;

  void SetDeltaTimeBetweenNSLookupAtLaunchTimeInMilliSecond(CORBA::Long timeInMS) override;

  CORBA::Long GetDeltaTimeBetweenCPUMemMeasureInMilliSecond() override;

  void SetDeltaTimeBetweenCPUMemMeasureInMilliSecond(CORBA::Long timeInMS) override;

  void SetBigObjOnDiskThreshold(CORBA::Long thresholdInByte) override;

  void SetBigObjOnDiskDirectory(const char *directory) override;

  void SetDirectoryForReplayFiles(const char *directory) override;

  void SetNumberOfRetry(CORBA::Long nbRetry) override;

  CORBA::Long GetNumberOfRetry() override;

  static const char *_ContainerManagerNameInNS;

private:

  void ShutdownContainersGeneric(std::function<void(Engines::Container_ptr)> funcToBeCalledOnContainer);

protected:
  // C++ methods
  Engines::Container_ptr
  FindContainer(const Engines::ContainerParameters& params,
                const Engines::ResourceList& possibleResources);

  Engines::Container_ptr
  FindContainer(const Engines::ContainerParameters& params,
                const std::string& resource);

  std::string GetCppBinaryOfKernelSSLContainer() const;

  std::string GetCppBinaryOfKernelContainer() const;
  
  std::string GetRunRemoteExecutableScript() const;

  std::string BuildCommandToLaunchRemoteContainer(const std::string & resource_name,
                                                  const Engines::ContainerParameters& params,
                                                  const std::string& container_exe="SALOME_Container") const;

  std::string BuildCommandToLaunchLocalContainer(const Engines::ContainerParameters& params,
                                                 const std::string& machinesFile,
                                                 const std::string& container_exe,//"SALOME_Container"
                                                 std::string& tmpFileName) const;

  std::string BuildTempFileToLaunchRemoteContainer(const std::string& resource_name,
                                                   const Engines::ContainerParameters& params, std::string& tmpFileName) const;

  static void RmTmpFile(std::string& tmpFile);

  void AddOmninamesParams(std::string& command) const;

  void AddOmninamesParams(std::ostream& fileStream) const;

  static std::string BuildTemporaryFileName();

  std::string GetMPIZeroNode(const std::string machine, const std::string machinesFile) const;

  std::string machinesFile(const int nbproc);
  
  std::string getCommandToRunRemoteProcessCommon(const std::string& templateName, const std::string& remoteScript, 
                                          AccessProtocolType protocol, 
                                          const std::string & hostname, 
                                          const std::string & username,
                                          const std::string & applipath,
                                          const std::string & workdir) const;

  std::string getCommandToRunRemoteProcess(AccessProtocolType protocol, 
                                          const std::string & hostname, 
                                          const std::string & username,
                                          const std::string & applipath,
                                          const std::string & workdir="") const;
                                          
  std::string getCommandToRunRemoteProcessNoAppli(AccessProtocolType protocol, 
                                          const std::string & hostname, 
                                          const std::string & username,
                                          const std::string & applipath,
                                          const std::string & workdir="") const;

  Engines::Container_ptr
  LaunchContainer(const Engines::ContainerParameters& params,
                  const std::string & resource_selected,
                  const std::string & hostname,
                  const std::string & machFile,
                  const std::string & containerNameInNS);

  CORBA::ORB_var _orb;
  PortableServer::POA_var _poa;

  SALOME_ResourcesManager_Client *_resManager;
  SALOME_NamingService_Abstract *_NS;

  //! different behaviour if $APPLI exists (SALOME Application)
  bool _isAppliSalomeDefined;

  //! attribute that contains the number of processes used in batch mode by MPI containers
  int _nbprocUsed;

  //! attribute that specifies the launch mode.
  bool _isSSL = true;

  static omni_mutex _numInstanceMutex ; // lib and instance protection

  //! attributes to allow concurrency for // GiveContainer
  Utils_Mutex _giveContainerMutex1;

  pid_t _pid_mpiServer;

  // Begin of PacO++ Parallel extension
  typedef std::vector<std::string> actual_launch_machine_t;

  bool checkPaCOParameters(Engines::ContainerParameters & params, std::string resource_selected);

  Engines::Container_ptr
  StartPaCOPPContainer(const Engines::ContainerParameters& params, std::string resource_selected);



  std::string BuildCommandToLaunchPaCOProxyContainer(const Engines::ContainerParameters& params,
                                                     std::string machine_file_name,
                                                     std::string & proxy_hostname);

  std::string BuildCommandToLaunchPaCONodeContainer(const Engines::ContainerParameters& params,
                                                    const std::string & machine_file_name,
                                                    SALOME_ContainerManager::actual_launch_machine_t & vect_machine,
                                                    const std::string & proxy_hostname);

  void LogConfiguration(const std::string & log_type,
                        const std::string & exe_type,
                        const std::string & container_name,
                        const std::string & hostname,
                        std::string & begin,
                        std::string & end);

  CORBA::Object_ptr
  LaunchPaCOProxyContainer(const std::string& command,
                           const Engines::ContainerParameters& params,
                           const std::string& hostname);

  bool
  LaunchPaCONodeContainer(const std::string& command,
                          const Engines::ContainerParameters& params,
                          const std::string& name,
                          SALOME_ContainerManager::actual_launch_machine_t & vect_machine);
  // End of PaCO++ Parallel extension
public:
  static char *GetenvThreadSafe(const char *name);
  static std::string GetenvThreadSafeAsString(const char *name);
  static int SystemThreadSafe(const char *command);
  static long SystemWithPIDThreadSafe(const std::vector<std::string>& command);
  static void AddOmninamesParams(std::ostream& fileStream, SALOME_NamingService_Abstract *ns);
  static void MakeTheCommandToBeLaunchedASync(std::string& command);
  static int GetTimeOutToLoaunchServer();
  static void SleepInSecond(int ellapseTimeInSecond);
 private:
  static const int TIME_OUT_TO_LAUNCH_CONT;
  static const int DFT_DELTA_TIME_NS_LOOKUP_IN_MS;
  static Utils_Mutex _getenvMutex;
  static Utils_Mutex _systemMutex;
private:
  std::vector< std::pair<std::string, std::string> > _override_env;
  int _time_out_in_second;
  int _delta_time_ns_lookup_in_ms;
  int _delta_time_measure_in_ms;
  std::string _code_to_exe_on_startup;
};
