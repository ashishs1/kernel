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

//  SALOME NamingService : wrapping NamingService services
//  File   : SALOME_NamingService.hxx
//  Author : Estelle Deville
//  Module : SALOME
//  $Header$
//
#ifndef SALOME_NAMINGSERVICE_H
#define SALOME_NAMINGSERVICE_H

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_ContainerManager)
#include CORBA_CLIENT_HEADER(SALOME_Component)

#include <vector>
#include <string>
#include "Utils_Mutex.hxx"
#include "ServiceUnreachable.hxx"

#include "SALOME_NamingService_Abstract.hxx"

#ifdef WIN32
//#pragma warning(disable:4290) // Warning Exception ...
#endif

class NAMINGSERVICE_EXPORT SALOME_NamingService : public SALOME_NamingService_Abstract
{
public:
  SALOME_NamingService();
  SALOME_NamingService(CORBA::ORB_ptr orb);

  virtual ~SALOME_NamingService();
  bool IsTrueNS() const override { return true; }
  std::vector< std::string > repr() override;
  void init_orb(CORBA::ORB_ptr orb=0) override;
  void Register(CORBA::Object_ptr ObjRef, const char* Path) override;
  CORBA::Object_ptr Resolve(const char* Path) override; 
  CORBA::Object_ptr ResolveFirst(const char* Path) ; 
  CORBA::Object_ptr ResolveComponent(const char* hostname,
                                     const char* containerName,
                                     const char* componentName,
                                     const int nbproc=0) override;

  int Find(const char* name) ;
  bool Create_Directory(const char* Path) ;
  bool Change_Directory(const char* Path) override;
  char* Current_Directory() ;
  void list() ;
  std::vector<std::string> list_directory() override;
  std::vector<std::string> list_subdirs() override;
  std::vector<std::string> list_directory_recurs() override ;
  void Destroy_Name(const char* Path) override;
  void Destroy_Directory(const char* Path) override;
  void Destroy_FullDirectory(const char* Path) override;
  char *getIORaddr();
  CORBA::ORB_ptr orb();
  SALOME_NamingService_Abstract *clone() override;

protected:
  Utils_Mutex _myMutex;
  CORBA::ORB_var _orb;
  CosNaming::NamingContext_var _root_context, _current_context;

  void _initialize_root_context();
  int _createContextNameDir(std::string path,
                            CosNaming::Name& context_name,
                            std::vector<std::string>& splitPath,
                            bool onlyDir);
  void _Find(const char* name, CORBA::Long& occurence_number);
  void _current_directory(std::vector<std::string>& splitPath,
                          int& lengthResult,
                          CosNaming::NamingContext_var contextToFind,
                          bool& notFound);
  void _list_directory_recurs(std::vector<std::string>& myList,
                              std::string relativeSubDir,
                              std::string absCurDirectory);

};

#endif // SALOME_NAMINGSERVICE_H

