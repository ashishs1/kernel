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

//  SALOME Container : implementation of container and engine for Kernel
//  File   : Container_init_python.hxx
//  Author : Paul RASCLE, EDF
//  Module : KERNEL
//  $Header$
//
#ifndef _CONTAINER_INIT_PYTHON_HXX_
#define _CONTAINER_INIT_PYTHON_HXX_

#include "SALOME_Container.hxx"
#include <SALOMEconfig.h>

#include <pthread.h>  // must be before Python.h !

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif
#include <Python.h>

typedef void ContainerPyOutChanged(void* data,char * c);

typedef struct {
  PyObject_HEAD
  int softspace;
  ContainerPyOutChanged* _cb;
  void* _data;
  bool _iscerr;
} ContainerPyStdOut;

struct CONTAINER_EXPORT KERNEL_PYTHON
{
  static void init_python(int argc, char **argv);
};

#endif
