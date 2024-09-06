#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2024  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

import unittest
import os
import salome
import Engines
import pylauncher
import SALOME_PyNode
import KernelBasis
import SALOME

import tempfile
import glob
import pickle
import tempfile
import logging
from datetime import datetime
import subprocess as sp
from pathlib import Path

killMeCode = """
import os
import sys
import signal
j = 7 * i
sys.stdout.write(str(j)) ; sys.stdout.flush() # the aime of test in replay mode to be sure that case is runnable
os.kill( os.getpid() , signal.SIGKILL)# the aim of test is here
"""

normalCode = """
j = 8 * i
my_log_4_this_session.addFreestyleAndFlush( ("a",777) ) # to check that hidden var is still accessible
"""

heatCPUCode = """import KernelBasis
nbcore = 3
cst = KernelBasis.GetTimeAdjustmentCst()
KernelBasis.HeatMarcel(5 * nbcore * cst,nbcore)
j = 8*i"""

killMeAtTheEnd = """import atexit
import KernelServices

def ErrorAtexit():
    KernelServices.GenerateViolentMemoryFaultForTestPurpose()

atexit.register(ErrorAtexit)

print("OKKKKKK")
j = 9 * i * a
print("OKKKKKK3333")
"""

killMeAtTheEnd2 = """import atexit
import KernelServices
import os

def ErrorAtexit():
    KernelServices.GenerateViolentMemoryFaultForTestPurpose()

atexit.register(ErrorAtexit)

print("OKKKKKK")
j = 9 * i * a
k = os.getcwd()
print("OKKKKKK3333")
"""

FunnyCase = """import os
import time
if not os.path.exists( {!r} ):
    time.sleep( 20 ) # first exec
else:
    time.sleep( 2 )  # second exec after retry
j = 44
"""

class testPerfLogManager1(unittest.TestCase):
    def test0(self):
        """
        EDF29852 : Kill container with OutOfProcessNoReplay mode and see if container still responds.
        """
        with tempfile.TemporaryDirectory() as tmpdirname:
            salome.salome_init()
            assert(isinstance(KernelBasis.GetAllPyExecutionModes(),tuple))
            KernelBasis.SetPyExecutionMode("OutOfProcessNoReplay") # the aim of test is here
            hostname = "localhost"
            cp = pylauncher.GetRequestForGiveContainer(hostname,"container_crash_test")
            salome.cm.SetBigObjOnDiskThreshold(1000)
            salome.cm.SetOverrideEnvForContainersSimple(env = [])
            salome.cm.SetDirectoryForReplayFiles( str( tmpdirname ) )
            cont = salome.cm.GiveContainer(cp)
            poa = salome.orb.resolve_initial_references("RootPOA")
            obj = SALOME_PyNode.SenderByte_i(poa,pickle.dumps( (["i"],{"i": 3} ) )) ; id_o = poa.activate_object(obj) ; refPtr = poa.id_to_reference(id_o)
            pyscript2 = cont.createPyScriptNode("testScript2",killMeCode)
            pyscript2.executeFirst(refPtr)
            self.assertRaises(SALOME.SALOME_Exception,pyscript2.executeSecond,["j"]) # an agressive SIGKILL has been received and container is still alive :) - it throws an exception :)
            pyscript2.UnRegister()
            pyscript3 = cont.createPyScriptNode("testScript3",normalCode)
            obj = SALOME_PyNode.SenderByte_i(poa,pickle.dumps( (["i"],{"i": 3} ) )) ; id_o = poa.activate_object(obj) ; refPtr = poa.id_to_reference(id_o)
            pyscript3.executeFirst(refPtr)
            ret = pyscript3.executeSecond(["j"])
            ret = pickle.loads( SALOME_PyNode.SeqByteReceiver(ret[0]).data() )
            self.assertEqual(ret,24) # container has received a SIGKILL but it kindly continue to respond :)
            a = salome.logm.NaiveFetch()
            dicToTest = {k:v for k,v in a[0][2][0].get().freestyle}
            self.assertEqual(dicToTest['a'],777)
            cont.Shutdown()

    def test1(self):
        """
        EDF29852 : Same than test0 Kill container with OutOfProcessWithReplay mode and see if container still responds. But in addition we test if the python script is runnable !
        """
        with tempfile.TemporaryDirectory() as tmpdirname:
            salome.salome_init()
            assert(isinstance(KernelBasis.GetAllPyExecutionModes(),tuple))
            KernelBasis.SetPyExecutionMode("OutOfProcessWithReplay") # the aim of test is here
            hostname = "localhost"
            cp = pylauncher.GetRequestForGiveContainer(hostname,"container_crash_test")
            salome.cm.SetBigObjOnDiskThreshold(1000)
            salome.cm.SetOverrideEnvForContainersSimple(env = [])
            salome.cm.SetDirectoryForReplayFiles(str( tmpdirname ))
            cont = salome.cm.GiveContainer(cp)
            poa = salome.orb.resolve_initial_references("RootPOA")
            obj = SALOME_PyNode.SenderByte_i(poa,pickle.dumps( (["i"],{"i": 3} ) )) ; id_o = poa.activate_object(obj) ; refPtr = poa.id_to_reference(id_o)
            pyscript2 = cont.createPyScriptNode("testScript2",killMeCode)
            pyscript2.executeFirst(refPtr)
            self.assertRaises(SALOME.SALOME_Exception,pyscript2.executeSecond,["j"]) # an agressive SIGKILL has been received and container is still alive :) - it throws an exception :)
            pyscript2.UnRegister()
            pyscript3 = cont.createPyScriptNode("testScript3",normalCode)
            obj = SALOME_PyNode.SenderByte_i(poa,pickle.dumps( (["i"],{"i": 3} ) )) ; id_o = poa.activate_object(obj) ; refPtr = poa.id_to_reference(id_o)
            pyscript3.executeFirst(refPtr)
            ret = pyscript3.executeSecond(["j"])
            ret = pickle.loads( SALOME_PyNode.SeqByteReceiver(ret[0]).data() )
            self.assertEqual(ret,24) # container has received a SIGKILL but it kindly continue to respond :)
            a = salome.logm.NaiveFetch()
            dicToTest = {k:v for k,v in a[0][2][0].get().freestyle}
            self.assertEqual(dicToTest['a'],777)
            # EDF30875 : check in verbose mode of presence of entries to debug
            self.assertEqual( set(dicToTest), {'strtdumpout', 'b4loadctx', 'afterloadctx', 'afterdump', 'bforeexec', 'a', 'afterexec'} )
            grpsOfLogToKill = cont.getAllLogFileNameGroups()
            self.assertEqual(1,len(grpsOfLogToKill))
            replayInput = grpsOfLogToKill[0]
            # now try to replay the failing case
            p = sp.Popen(["python3",os.path.basename(replayInput[0])],cwd = os.path.dirname(replayInput[0]),stdout=sp.PIPE,stderr=sp.PIPE)
            out,err = p.communicate()
            self.assertNotEqual(p.returncode,0) # very important ! The failing case must continue to fail :)
            self.assertEqual("21".encode(),out) # very important to check that the reported case is standalone enough to be replayable poste mortem
            # cleanup
            dn = os.path.dirname(replayInput[0])
            for elt in replayInput:
                zeFile = os.path.join( dn, os.path.basename(elt) )
                if os.path.exists( zeFile ):
                    os.unlink( zeFile )
            cont.Shutdown()

    def test2(self):
        """
        Aim of test is to check that CPU/mem retrieved in log is OK even in OutOfProcessNoReplay mode.
        """
        with tempfile.TemporaryDirectory() as tmpdirname:
            salome.salome_init()
            salome.logm.clear()
            assert(isinstance(KernelBasis.GetAllPyExecutionModes(),tuple))
            KernelBasis.SetPyExecutionMode("OutOfProcessNoReplay") # the aim of test is here
            hostname = "localhost"
            PROXY_THRES = -1
            #
            salome.cm.SetBigObjOnDiskThreshold(PROXY_THRES)
            salome.cm.SetOverrideEnvForContainersSimple(env = [])
            salome.cm.SetDirectoryForReplayFiles(str( tmpdirname ))
            salome.cm.SetDeltaTimeBetweenCPUMemMeasureInMilliSecond( 250 )
            cp = pylauncher.GetRequestForGiveContainer(hostname,"container_cpu_mem_out_process_test")
            cont = salome.cm.GiveContainer(cp)
            poa = salome.orb.resolve_initial_references("RootPOA")
            obj = SALOME_PyNode.SenderByte_i(poa,pickle.dumps( (["i"],{"i": 3} ) )) ; id_o = poa.activate_object(obj) ; refPtr = poa.id_to_reference(id_o)
            pyscript2 = cont.createPyScriptNode("testScript3",heatCPUCode)
            pyscript2.executeFirst(refPtr)
            ret = pyscript2.executeSecond(["j"])
            ret = pickle.loads( SALOME_PyNode.SeqByteReceiver(ret[0]).data() )
            self.assertEqual(ret,24)
            pyscript2.UnRegister()
            a = salome.logm.NaiveFetch()
            cpu_mem_to_test = a[0][1][0].get()._cpu_mem_during_exec.data # normally even in OutOfProcessNoReplay mode the CPU must report somehing greater than 100
            self.assertGreater(len(cpu_mem_to_test),10) # 5 second of run 250 ms of interval between measures -> 20. In case of problem in HeatMarcel -> 10
            logging.debug("CPU mem measured (even in OutOfProcessNoReplay) : {}".format(cpu_mem_to_test))
            greater_than_100 = [a for a,b in cpu_mem_to_test if a > 100]
            self.assertGreater(len(greater_than_100),1) # At minimum one measure must report CPU load > 100%
            cont.Shutdown()

    def test3(self):
        """
        [EDF29150] : test that we can resist to a crash at exit
        """
        with tempfile.TemporaryDirectory() as tmpdirname:
            salome.salome_init()
            KernelBasis.SetPyExecutionMode("OutOfProcessWithReplayFT")
            hostname = "localhost"
            cp = pylauncher.GetRequestForGiveContainer(hostname,"container_crash_test")
            salome.cm.SetNumberOfRetry( 3 )
            salome.cm.SetCodeOnContainerStartUp("""a = 2""")
            salome.cm.SetBigObjOnDiskThreshold(1000)
            salome.cm.SetOverrideEnvForContainersSimple(env = [])
            salome.cm.SetDirectoryForReplayFiles(str( tmpdirname ))
            cont = salome.cm.GiveContainer(cp)
            poa = salome.orb.resolve_initial_references("RootPOA")
            obj = SALOME_PyNode.SenderByte_i(poa,pickle.dumps( (["i"],{"i": 3} ) )) ; id_o = poa.activate_object(obj) ; refPtr = poa.id_to_reference(id_o)
            pyscript = cont.createPyScriptNode("testScript4",killMeAtTheEnd)
            pyscript.executeFirst(refPtr)
            ret = pyscript.executeSecond(["j"])
            ret = pickle.loads( SALOME_PyNode.SeqByteReceiver(ret[0]).data() )
            self.assertEqual(ret,54)
            with open(cont.locallogfilename) as f:
                logCont = f.read( )
                self.assertTrue( "WARNING : Retry #" in logCont)
                self.assertTrue( "WARNING : Following code has generated non zero return code" in logCont )# should report something into the container
            cont.Shutdown()

    def test4(self):
        """
        EDF30399 : Check current directory
        """
        with tempfile.TemporaryDirectory() as tmpdirname:
            os.chdir( tmpdirname )
            KernelBasis.SetForwardCurrentDirectoryStatus( True ) # key point
            salome.salome_init()
            assert(isinstance(KernelBasis.GetAllPyExecutionModes(),tuple))
            KernelBasis.SetPyExecutionMode("OutOfProcessNoReplayFT") # the aim of test is here
            hostname = "localhost"
            cp = pylauncher.GetRequestForGiveContainer(hostname,"container_crash_test_2")
            salome.cm.SetCodeOnContainerStartUp("""a = 2""")
            salome.cm.SetBigObjOnDiskThreshold(1000)
            salome.cm.SetOverrideEnvForContainersSimple(env = [])
            salome.cm.SetDirectoryForReplayFiles( str( tmpdirname ) )
            cont = salome.cm.GiveContainer(cp)
            poa = salome.orb.resolve_initial_references("RootPOA")
            obj = SALOME_PyNode.SenderByte_i(poa,pickle.dumps( (["i"],{"i": 3} ) )) ; id_o = poa.activate_object(obj) ; refPtr = poa.id_to_reference(id_o)
            pyscript = cont.createPyScriptNode("testScript4",killMeAtTheEnd2)
            pyscript.executeFirst(refPtr)
            ret = pyscript.executeSecond(["j","k"])
            ret0 = pickle.loads( SALOME_PyNode.SeqByteReceiver(ret[0]).data() )
            self.assertEqual(ret0,54)
            ret2 = pickle.loads( SALOME_PyNode.SeqByteReceiver(ret[1]).data() )
            self.assertEqual( ret2, str(tmpdirname) )
            cont.Shutdown()

    def test5(self):
        """
        EDF30875 : Verbose mode set to ON 
        """
        from threading import Thread
        def func( fname ):
            import time
            time.sleep( 5 )
            with open( fname, "w" ) as f:
                f.write( "go" )
        # file used to pilot the behaviour of process
        fname = "touch.txt"
        KernelBasis.SetNumberOfRetry(2)
        KernelBasis.SetExecutionTimeOut(10) # <= Key Point is here
        KernelBasis.SetPyExecutionMode("OutOfProcessNoReplayFT") # Fail tolerant
        with tempfile.TemporaryDirectory() as tmpdirname:
            os.chdir( tmpdirname )
            fnameFull = Path(tmpdirname) / fname
            hostname = "localhost"
            cp = pylauncher.GetRequestForGiveContainer(hostname,"container_crash_test")
            salome.cm.SetDirectoryForReplayFiles( str( tmpdirname ) )
            with salome.ContainerLauncherCM(cp,True) as cont:
                poa = salome.orb.resolve_initial_references("RootPOA")
                obj = SALOME_PyNode.SenderByte_i(poa,pickle.dumps( (["i"],{"i": 3} ) )) ; id_o = poa.activate_object(obj) ; refPtr = poa.id_to_reference(id_o)
                pyscript = cont.createPyScriptNode("testScript5",FunnyCase.format( fnameFull.as_posix() ))
                t = Thread(target = func,args=[fnameFull.as_posix()])
                t.start()
                pyscript.executeFirst(refPtr)
                ret = pyscript.executeSecond(["j"])
                t.join()
                ret0 = pickle.loads( SALOME_PyNode.SeqByteReceiver(ret[0]).data() )
                self.assertEqual(ret0,44)
                a = salome.logm.NaiveFetch()
                #res = a[0][1][0]
                #b = SALOME_ContainerHelper.ScriptExecInfoDeco(res,a[0][1]) print( a[0].log) 
                # print( res.get().freestyle )
                #print( [elt[0] for elt in res.get().freestyle] )
                #self.assertEqual( [elt[0] for elt in res.get().freestyle], ['b4loadctx', 'afterloadctx', 'bforeexec', 'b4loadctx', 'afterloadctx', 'bforeexec', 'afterexec', 'strtdumpout', 'afterdump'] )

if __name__ == '__main__':
    from salome_utils import positionVerbosityOfLoggerRegardingState,setVerboseLevel,setVerbose
    salome.standalone()
    salome.salome_init()
    setVerbose(True)
    setVerboseLevel(logging.DEBUG)
    positionVerbosityOfLoggerRegardingState()
    unittest.main()

