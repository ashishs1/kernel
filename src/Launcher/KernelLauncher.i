// Copyright (C) 2021-2025  CEA, EDF
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

%module KernelLauncher

%include "std_string.i"

%{
#include "KernelLauncher.hxx"
%}

%inline
{
  std::string RetrieveInternalInstanceOfLocalCppResourcesManager();
  std::string GetContainerManagerInstance();
  std::string GetResourcesManagerInstance();
  std::string GetExternalServerInstance();
  std::string GetLogManagerInstance();
  std::string GetLockMasterEntryInNS();
  std::string GetLockMasterInstance();
}

%pythoncode %{
def myContainerManager():
  import Engines
  import CORBA
  orb=CORBA.ORB_init([''])
  return orb.string_to_object(GetContainerManagerInstance())

def myResourcesManager():
  import Engines
  import CORBA
  orb=CORBA.ORB_init([''])
  return orb.string_to_object(GetResourcesManagerInstance())

def GetExternalServer():
  import SALOME
  import CORBA
  orb=CORBA.ORB_init([''])
  return orb.string_to_object(GetExternalServerInstance())

def myLogManager():
  import Engines
  import CORBA
  orb=CORBA.ORB_init([''])
  return orb.string_to_object(GetLogManagerInstance())

def myLockMaster():
  import Engines
  import CORBA
  orb=CORBA.ORB_init([''])
  return orb.string_to_object(GetLockMasterInstance())
%}
