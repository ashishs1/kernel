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

//  File   : SALOMEDSImpl_AttributeIOR.hxx
//  Author : Sergey RUIN
//  Module : SALOME
//
#ifndef _SALOMEDSImpl_AttributeIOR_HeaderFile
#define _SALOMEDSImpl_AttributeIOR_HeaderFile

#include "SALOMEDSImpl_Defines.hxx"
#include "DF_Attribute.hxx"
#include "DF_Label.hxx"       
#include "SALOMEDSImpl_GenericAttribute.hxx"

#include <string>

class SALOMEDSIMPL_EXPORT SALOMEDSImpl_AttributeIOR :
  public SALOMEDSImpl_GenericAttribute 
{
private:

  std::string myString;

public:

  static const std::string& GetID() ;
  static SALOMEDSImpl_AttributeIOR* Set(const DF_Label& label,
    const std::string& IOR);
  SALOMEDSImpl_AttributeIOR();
  const std::string& ID() const;
  void SetValue(const std::string& theValue);
  std::string Value() const ;
  void Restore(DF_Attribute* with);
  DF_Attribute* NewEmpty() const;
  void Paste(DF_Attribute* into);
  ~SALOMEDSImpl_AttributeIOR() ;
};

#endif
