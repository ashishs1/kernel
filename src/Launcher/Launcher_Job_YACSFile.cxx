// Copyright (C) 2009-2025  CEA, EDF, OPEN CASCADE
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

// Author: André RIBES - EDF R&D
//
#include "Launcher_Job_YACSFile.hxx"
#include <sstream>
#include <sys/stat.h>

const char Launcher::Job_YACSFile::TYPE_NAME[] = "yacs_file";

Launcher::Job_YACSFile::Job_YACSFile()
:Launcher::Job_SALOME(false)
{
  _job_type = Launcher::Job_YACSFile::TYPE_NAME;
  _dumpState = -1;
  _yacsDriverOptions = "";
}

Launcher::Job_YACSFile::~Job_YACSFile() {}

void
Launcher::Job_YACSFile::setJobFile(const std::string & job_file)
{
  Launcher::Job::setJobFile(job_file);
}

void
Launcher::Job_YACSFile::addJobTypeSpecificScript(std::ofstream & launch_script_stream)
{
  struct stat statbuf;
  if(stat(getenv("APPLI"), &statbuf) ==0 &&  S_ISREG(statbuf.st_mode))
      // case of a salome launcher file
      launch_script_stream << _resource_definition.AppliPath << " shell -- driver " << _job_file_name_complete;
  else
      launch_script_stream << _resource_definition.AppliPath << "/salome shell -- driver " << _job_file_name_complete;
  if ( !_verbose_py_log_level.empty() )
    launch_script_stream << " --verbose" << " --verbose-level=" << _verbose_py_log_level;
  if (_dumpState > 0)
    launch_script_stream << " --dump=" << _dumpState;
  if(! _yacsDriverOptions.empty())
    launch_script_stream << " " << _yacsDriverOptions ;
  launch_script_stream << " > logs/yacs_" << _launch_date << ".log 2>&1" << std::endl;
}

void
Launcher::Job_YACSFile::checkSpecificParameters()
{
  // Specific parameters
  std::map<std::string, std::string>::iterator it = _specific_parameters.find("EnableDumpYACS");
  if (it != _specific_parameters.end())
  {
    // Decode info
    std::string user_value = it->second;
    std::istringstream iss(user_value);
    if (!(iss >> _dumpState))
      throw LauncherException("Specific parameter EnableDumpYACS is not correctly defined: it should be an integer. Value given is " + user_value);
  }
  
  it = _specific_parameters.find("YACSDriverOptions");
  if (it != _specific_parameters.end())
  {
    // Decode info
    _yacsDriverOptions = it->second;
  }
}
