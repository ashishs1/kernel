#!/usr/bin/env python3
# Copyright (C) 2015-2025  CEA, EDF, OPEN CASCADE
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

"""
Usage: salome_test_driver.py <timeout_delay> <test command> [test command arguments]
"""

import sys
import os
import subprocess
import signal
from contextlib import suppress

# Run test
def runTest(command):
  print("Running:", " ".join(command))
  p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  out, err = p.communicate()
  res = p.returncode
  # About res value:
  # A negative value -N indicates that the child was terminated by signal N (Unix only).
  # On Unix, the value 11 generally corresponds to a segmentation fault.
  return res, out, err
#

# Display output and errors
def processResult(res, out, err):
  # Decode output
  out = out.decode('utf_8', errors='ignore') if out else ''
  err = err.decode('utf_8', errors='ignore') if err else ''

  # Execute hook if it is installed
  if getattr(processResult, '__hook', None):
    res, out, err = getattr(processResult, '__hook')(res, out, err)

  # Print standard output
  if out:
    print(out)

  # Print standard error
  if err:
    print("    ** Detected error **")
    print("Error code: ", res)
    print(err, end=' \n')
    print("    ** end of message **")

  return res
#

def installHook(hook=None):
  """Install custome hook to process test result."""
  with suppress(AttributeError, IndexError):
    hook = hook.split(',')
    hook_path = hook.pop(0)
    hook_func = hook.pop(0)
    if os.path.exists(hook_path) and hook_func:
      with open(hook_path, 'rb') as hook_script:
        dic = {}
        exec(hook_script.read(), dic)
        processResult.__hook = dic.get(hook_func)
        print("Custom hook has been installed")
#

# Timeout management
class TimeoutException(Exception):
  """Exception raised when test timeout is reached."""
#
def timeoutHandler(signum, frame):
  raise TimeoutException()
#

if __name__ == "__main__":
  timeout_delay = sys.argv[1]
  args = sys.argv[2:]

  # Install hook if supplied via command line
  with suppress(IndexError, ValueError):
    index = args.index('--hook')
    args.pop(index)
    hook = args.pop(index)
    installHook(hook)

  # Add explicit call to python executable if a Python script is passed as
  # first argument
  if not args:
    print("Invalid arguments for salome_test_driver.py. No command defined.")
    sys.exit(1)
  _, ext = os.path.splitext(args[0])
  if ext == ".py":
    test_and_args = [sys.executable] + args
  else:
    test_and_args = args

  # Ensure OMNIORB_USER_PATH is set
  from salomeContextUtils import setOmniOrbUserPath
  setOmniOrbUserPath()

  # Set timeout handler
  print("Test timeout explicitly set to: %s seconds"%timeout_delay)
  timeout_sec = abs(int(timeout_delay)-10)
  if sys.platform == 'win32':
    from threading import Timer
    timer = Timer(timeout_sec, timeoutHandler)
    timer.start()
  else:
    signal.alarm(timeout_sec)
    signal.signal(signal.SIGALRM, timeoutHandler)

  # Run test in a new SALOME instance
  from salome_instance import SalomeInstance
  res = 1
  try:
    salome_instance = SalomeInstance.start(shutdown_servers=True)
    port = salome_instance.get_port()
    res, out, err = runTest(test_and_args)
    res = processResult(res, out, err)
  except TimeoutException:
    print("FAILED : timeout(%s) is reached"%timeout_delay)
  except Exception:
    import traceback
    traceback.print_exc()
    pass
  try:
    salome_instance.stop()
    os.kill(pid, signal.SIGTERM)
  except Exception:
    pass
  if sys.platform == 'win32':
    timer.cancel()
  print("Exit test with status code:", res)
  sys.exit(res)
#
