#! /usr/bin/env python

# System imports
from distutils.core import *
from distutils      import sysconfig

# Third-party modules - we depend on numpy for everything
import numpy
import os
# Obtain the numpy include directory.  This logic works across numpy versions.
try:
    numpy_include = numpy.get_include()
except AttributeError:
    numpy_include = numpy.get_numpy_include()
# HexitecIO extension module
_AmptekHardwareInterface = Extension("_AmptekHardwareInterface",
                   ["python/AmptekHardwareInterface.i",] + ["src/" + f for f in os.listdir("src") if os.path.isfile("src/" + f)],
                   include_dirs = [numpy_include, "include"],
                   swig_opts=['-c++',"-modern" , "-nomodernargs"],
                   extra_compile_args = ["-std=c++11", "-g"], 
                   libraries=["usb-1.0"]
                   )

# NumyTypemapTests setup
setup(  name        = "AmptekHardwareInterface function",
        description = "Implementation of the Amptek DP5 Protocol",
        scripts     = ["python/AmptekPX5.py"],
        author      = "Christian Koernig",
        version     = "0.1",
        ext_modules = [_AmptekHardwareInterface]
        )