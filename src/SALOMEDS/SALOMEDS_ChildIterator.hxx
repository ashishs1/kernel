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

//  File   : SALOMEDS_ChildIterator.hxx
//  Author : Sergey RUIN
//  Module : SALOME
//
#ifndef __SALOMEDS_CHILDITERATOR_H__
#define __SALOMEDS_CHILDITERATOR_H__

#include "SALOMEDSClient.hxx"
#include "SALOMEDSImpl_ChildIterator.hxx"

// IDL headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOMEDS)

class SALOMEDS_ChildIterator: public SALOMEDSClient_ChildIterator
{
private:
  bool                               _isLocal;
  SALOMEDSImpl_ChildIterator*        _local_impl;
  SALOMEDS::ChildIterator_var        _corba_impl;

public:

  SALOMEDS_ChildIterator(const SALOMEDSImpl_ChildIterator& theIterator);
  SALOMEDS_ChildIterator(SALOMEDS::ChildIterator_ptr theIterator);
  ~SALOMEDS_ChildIterator();

  virtual void Init();
  virtual void InitEx(bool theAllLevels);
  virtual bool More();
  virtual void Next();
  virtual _PTR(SObject) Value();
};
#endif
