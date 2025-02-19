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

// --- include all SALOMEDS Test from basics until the present directory
//
#include "SALOMELocalTraceTest.hxx"
#include "SALOMEDSImplTest.hxx"
#include "SALOMEDSTest.hxx"

// --- Registers the fixture into the 'registry'

CPPUNIT_TEST_SUITE_REGISTRATION( SALOMEDSTest );
CPPUNIT_TEST_SUITE_REGISTRATION( SALOMEDSTest_Embedded );

#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestRunner.h>
#include <stdexcept>

#include <iostream>
#include <fstream>

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOMEDS)
#include "utilities.h"
#include "ArgvKeeper.hxx"
#include "OpUtil.hxx"
#include "Basics_Utils.hxx"
#include "SALOME_NamingService.hxx"
#include "NamingService_WaitForServerReadiness.hxx"
#include "SALOMEDS_Study_i.hxx"

#ifdef WIN32
#include <winsock2.h>
#define setenv Kernel_Utils::setenv
#endif

// ============================================================================
/*!
 *  Main program source for Unit Tests with cppunit package does not depend
 *  on actual tests, so we use the same for all partial unit tests.
 */
// ============================================================================

int main(int argc, char* argv[])
{
  SetArgcArgv(argc, argv);
  CORBA::ORB_var orb = KERNEL::GetRefToORB();
  std::string host; // = Kernel_Utils::GetHostname();
  char* wait_Superv = getenv("SALOMEDS_UNITTESTS_WAIT_SUPERVISOR");
  if(wait_Superv) host = Kernel_Utils::GetHostname();

  SALOME_NamingService NS(orb);
  if(host.empty())
    NamingService_WaitForServerReadiness(&NS, "/Study");
  else {
    std::string serverName = "/Containers/"+host+"/SuperVisionContainer";
    NamingService_WaitForServerReadiness(&NS, serverName);
  }

  CORBA::Object_var obj = NS.Resolve( "/Study" );
  if(CORBA::is_nil(obj)) {
     system("killSalome.py");
     return 1;
  }

  //Set up the environment for Embedded case
  std::string kernel_root = getenv("KERNEL_ROOT_DIR");
  CPPUNIT_ASSERT(!kernel_root.empty());
  kernel_root+="/share/salome/resources/kernel";

  CORBA::Object_var poaObj = orb->resolve_initial_references("RootPOA");
  if(!CORBA::is_nil(poaObj)) {
                PortableServer::POA_var poa = PortableServer::POA::_narrow(poaObj);

    SALOMEDS_Study_i* aStudy_i = new SALOMEDS_Study_i(orb);
    // Activate the objects.  This tells the POA that the objects are ready to accept requests.
    PortableServer::ObjectId_var aStudy_iid =  poa->activate_object(aStudy_i);
    SALOMEDS::Study_var Study = aStudy_i->_this();
    NS.Register(Study.in(), "/Study_embedded");

    // Obtain a POAManager, and tell the POA to start accepting
    // requests on its objects.
    PortableServer::POAManager_var pman = poa->the_POAManager();
    pman->activate();
  }


  // --- Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // ---  Add a listener that colllects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );

  // ---  Add a listener that print dots as test run.
#ifdef WIN32
  CPPUNIT_NS::TextTestProgressListener progress;
#else
  CPPUNIT_NS::BriefTestProgressListener progress;
#endif
  controller.addListener( &progress );

  // ---  Get the top level suite from the registry

  CPPUNIT_NS::Test *suite =
    CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();

  // ---  Adds the test to the list of test to run

  CPPUNIT_NS::TestRunner runner;
  runner.addTest( suite );
  runner.run( controller);

  // ---  Print test in a compiler compatible format.

  std::ofstream testFile;
  testFile.open("UnitTestsResult", std::ios::out |  std::ios::trunc);
  //CPPUNIT_NS::CompilerOutputter outputter( &result, std::cerr );
  CPPUNIT_NS::CompilerOutputter outputter( &result, testFile );
  outputter.write();

  // ---  Run the tests.

  bool wasSucessful = result.wasSuccessful();
  testFile.close();

  // ---  Return error code 1 if the one of test failed.

  return wasSucessful ? 0 : 1;
}
