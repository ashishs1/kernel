// Copyright (C) 2007-2023  CEA/DEN, EDF R&D, OPEN CASCADE
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

// ----------------------------------------------------------------------------
//
%module libSALOME_LifeCycleCORBA

%feature("autodoc", "1");

%include <std_except.i>
%include "std_string.i"


// ----------------------------------------------------------------------------

%{
#include "utilities.h"
#include "SALOME_LifeCycleCORBA.hxx"
#include "SALOME_FileTransferCORBA.hxx"
#include "SALOME_NamingService.hxx"
#include "ServiceUnreachable.hxx"
#include "Utils_SALOME_Exception.hxx"
#if PY_VERSION_HEX < 0x02050000 && !defined(PY_SSIZE_T_MIN)
typedef int Py_ssize_t;
#define PY_SSIZE_T_MAX INT_MAX
#define PY_SSIZE_T_MIN INT_MIN
#endif

#include <Python.h>
#include <omniORBpy.h>
omniORBpyAPI* api=0;

%}


// ----------------------------------------------------------------------------


%init
%{
  // init section

  PyObject* omnipy = PyImport_ImportModule((char*)"_omnipy");
  if (!omnipy)
  {
    PyErr_SetString(PyExc_ImportError, (char*)"Cannot import _omnipy");
    return NULL;
  }
  PyObject* pyapi = PyObject_GetAttrString(omnipy, (char*)"API");
  api = (omniORBpyAPI*)PyCapsule_GetPointer(pyapi,"_omnipy.API");
  Py_DECREF(pyapi);
%}



// ----------------------------------------------------------------------------

%typemap(out) Engines::Container_ptr, Engines::EngineComponent_ptr, Engines::fileRef_ptr,
              Engines::ContainerManager_ptr, Engines::ResourcesManager_ptr 
{
  MESSAGE("typemap out on CORBA object ptr");
  SCRUTE($1);
  $result = api->cxxObjRefToPyObjRef($1, 1);
  SCRUTE($result);
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) Engines::fileRef_ptr
{
  $1=PyObject_HasAttrString($input, "__omni_obj");
}

%typemap(in) Engines::fileRef_ptr aFileRef
{
  MESSAGE("typemap in on CORBA object ptr");
  try {
     CORBA::Object_ptr obj = api->pyObjRefToCxxObjRef($input,1);
     $1 = Engines::fileRef::_narrow(obj);
     SCRUTE($1);
  }
  catch (...) {
     PyErr_SetString(PyExc_RuntimeError, "not a valid CORBA object ptr");
  }
}

// We use a dictionary to represent ContainerParameters structure instead of the CORBA
// structure itself because it would require a transformation from the Python
// ContainerParameters structure, generated by omniOrbPy, to a C++ ContainerParameters
// structure, generated by omniOrb, and this would be quite complex.
%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const Engines::ContainerParameters &
{
  $1 = PyDict_Check($input)? 1 : 0;
}

%typemap(in) const Engines::ContainerParameters &
{
  MESSAGE("typemap in on Engines::ContainerParameters");
  if (PyDict_Check($input) == 1)
    {
      Engines::ContainerParameters *param = new Engines::ContainerParameters ;
      SALOME_LifeCycleCORBA::preSet(*param);

      PyObject *key, *value;
      Py_ssize_t pos = 0;
      while (PyDict_Next($input, &pos, &key, &value))
       {
         char* keystr = PyString_AsString(key);
         if (strcmp(keystr,"container_name")==0)
           {
             param->container_name = CORBA::string_dup(PyString_AsString(value));
           }
         else if (strcmp(keystr,"resource_name")==0)
           {
             param->resource_params.name = CORBA::string_dup(PyString_AsString(value));
           }
         else if (strcmp(keystr,"hostname")==0)
           {
             param->resource_params.hostname = CORBA::string_dup(PyString_AsString(value));
           }
         else if (strcmp(keystr,"OS")==0)
           {
             param->resource_params.OS = CORBA::string_dup(PyString_AsString(value));
           }
         else if (strcmp(keystr,"mem_mb")==0)
           {
             param->resource_params.mem_mb = PyLong_AsLong(value);
           }
         else if (strcmp(keystr,"cpu_clock")==0)
           {
             param->resource_params.cpu_clock = PyLong_AsLong(value);
           }
         else if (strcmp(keystr,"nb_proc_per_node")==0)
           {
             param->resource_params.nb_proc_per_node = PyLong_AsLong(value);
           }
         else if (strcmp(keystr,"nb_node")==0)
           {
             param->resource_params.nb_node = PyLong_AsLong(value);
           }
         else if (strcmp(keystr,"isMPI")==0)
           {
             param->isMPI = PyLong_AsLong(value);
           }
         else if (strcmp(keystr,"workingdir")==0)
             param->workingdir = CORBA::string_dup(PyString_AsString(value));
         else if (strcmp(keystr,"mode")==0)
             param->mode = CORBA::string_dup(PyString_AsString(value));
         else if (strcmp(keystr,"policy")==0)
             param->resource_params.policy = CORBA::string_dup(PyString_AsString(value));
         else if (strcmp(keystr,"parallelLib")==0)
           {
             param->parallelLib = CORBA::string_dup(PyString_AsString(value));
           }
       }
      $1 = param;
    }
  else 
    {
       MESSAGE("Not a ContainerParameters struct");
       PyErr_SetString(PyExc_TypeError,"Must Be a Python ContainerParameters object");
       return NULL;
    }
}


%typemap(freearg) const Engines::ContainerParameters &
{
  MESSAGE("delete $1");
  delete $1;
}

// ----------------------------------------------------------------------------

%include <Utils_SALOME_Exception.hxx>

%exception {
    Py_BEGIN_ALLOW_THREADS
    try {
      $action
    }
    catch (ServiceUnreachable&) {
       Py_BLOCK_THREADS
       PyErr_SetString(PyExc_RuntimeError,"Naming Service Unreacheable");
       return NULL;
    }
    catch (SALOME_Exception &e) {
       Py_BLOCK_THREADS
       PyErr_SetString(PyExc_RuntimeError,e.what());
       return NULL;
    }
    catch (SALOME::SALOME_Exception &e) {
       Py_BLOCK_THREADS
       PyErr_SetString(PyExc_RuntimeError,e.details.text);
       return NULL;
    }
    catch (...) {
       Py_BLOCK_THREADS
       PyErr_SetString(PyExc_RuntimeError, "unknown exception");
       return NULL;
    }
    Py_END_ALLOW_THREADS
}


%include <SALOME_LifeCycleCORBA.hxx>
%include <SALOME_FileTransferCORBA.hxx>
