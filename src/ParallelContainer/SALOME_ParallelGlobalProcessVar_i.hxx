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

//  SALOME_ParallelContainer : implementation of container and engine for Parallel Kernel
//  File   : SALOME_ParallelContainer_i.hxx
//  Author : André RIBES, EDF
//
#ifndef _SALOME_PARALLEL_GLOBALPROCESSVAR_I_HXX_
#define _SALOME_PARALLEL_GLOBALPROCESSVAR_I_HXX_

#ifdef WIN32
# ifdef CONTAINER_EXPORTS
#  define CONTAINER_EXPORT __declspec( dllexport )
# else
#  define CONTAINER_EXPORT __declspec( dllimport )
# endif
#else
# define CONTAINER_EXPORT
#endif

#include <SALOMEconfig.h>
#include <omnithread.h>
#include <map>
#include <string>

class CONTAINER_EXPORT ParallelGlobalProcessVar_i
{
public:
  ParallelGlobalProcessVar_i();
  virtual ~ParallelGlobalProcessVar_i();

protected:
  // Voir commentaire dans le fichier .cxx
  static std::map<std::string, int> _cntInstances_map;
  static std::map<std::string, void *> _library_map; // library names, loaded
  static std::map<std::string, void *> _toRemove_map;// library names to remove
  static omni_mutex _numInstanceMutex ; // lib and instance protection
};

#endif

