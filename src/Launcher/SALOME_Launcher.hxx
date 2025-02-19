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

#ifndef __SALOME_LAUNCHER_HXX__
#define __SALOME_LAUNCHER_HXX__

#include "SALOME_Launcher_defs.hxx"

#include "SALOME_NamingService.hxx"
#include "SALOMEconfig.h"
#include CORBA_CLIENT_HEADER(SALOME_Launcher)
#include "Launcher.hxx"

#include <string>
#include <list>

class SALOME_ContainerManager;
class SALOME_ResourcesManager;

class SALOMELAUNCHER_EXPORT SALOME_Launcher:
  public POA_Engines::SalomeLauncher,
  public PortableServer::RefCountServantBase
{

public:
  SALOME_Launcher(CORBA::ORB_ptr orb, PortableServer::POA_var poa);
  SALOME_Launcher(CORBA::ORB_ptr orb, PortableServer::POA_var poa, SALOME_NamingService_Abstract *externalNS);
  virtual ~SALOME_Launcher();
  // Main methods
  CORBA::Long createJob    (const Engines::JobParameters & job_parameters);
  void        launchJob    (CORBA::Long job_id);
  char *      getJobState  (CORBA::Long job_id);
  char *      getAssignedHostnames  (CORBA::Long job_id); // Get names or ids of hosts assigned to the job
  void        exportInputFiles(CORBA::Long job_id);
  void        getJobResults(CORBA::Long job_id, const char * directory);
  void        clearJobWorkingDir(CORBA::Long job_id);
  CORBA::Boolean getJobDumpState(CORBA::Long job_id, const char * directory);
  CORBA::Boolean getJobWorkFile(CORBA::Long job_id, const char * work_file, const char * directory);
  void        stopJob      (CORBA::Long job_id);
  void        removeJob    (CORBA::Long job_id);
  char * dumpJob(CORBA::Long job_id);
  CORBA::Long restoreJob(const char * dumpedJob);

  // Useful methods
  CORBA::Long    createJobWithFile(const char * xmlExecuteFile, const char * clusterName);
  CORBA::Boolean testBatch        (const Engines::ResourceParameters& params);

  // SALOME Kernel service methods
  void Shutdown();
  CORBA::Long getPID();

  // Observer and introspection methods
  virtual void addObserver(Engines::SalomeLauncherObserver_ptr observer);
  virtual void removeObserver(Engines::SalomeLauncherObserver_ptr observer);
  virtual Engines::JobsList * getJobsList();
  virtual Engines::JobParameters * getJobParameters(CORBA::Long job_id);

  // Save and load methods
  virtual void loadJobs(const char* jobs_file);
  virtual void saveJobs(const char* jobs_file);
  
  void DeclareUsingSalomeSession();

  SALOME_ResourcesManager *getResourcesManager() const { return _ResManager; }

  static const char *_LauncherNameInNS;

  static JobParameters_cpp
    JobParameters_CORBA2CPP(const Engines::JobParameters& job_parameters);

  static Engines::JobParameters_var
    JobParameters_CPP2CORBA(const JobParameters_cpp& job_parameters);

protected:
  // Internal methods
  virtual void notifyObservers(const std::string & event_name, const std::string & event_data);
  void init(CORBA::ORB_ptr orb, PortableServer::POA_var poa);
protected:
  CORBA::ORB_var _orb;
  PortableServer::POA_var _poa;
  SALOME_ContainerManager *_ContManager;
  SALOME_ResourcesManager *_ResManager;
  SALOME_NamingService_Abstract *_NS = nullptr;

  std::list<Engines::SalomeLauncherObserver_var> _observers;

  Launcher_cpp _l;
};

/*!
 * Methods to be used in SSL mode to skip NS.
 */
namespace KERNEL
{
  SALOMELAUNCHER_EXPORT SALOME_Launcher *getLauncherSA();
}

#endif
