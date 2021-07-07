#!/usr/bin/python3

from distutils.core import setup
from distutils.extension import Extension
import numpy as np

setup(
    name = 'vncmodule2',
    version = '1.0.0',
    description = "VNC on roids",    
    author = "Razvan Teslaru", 
    author_email = "razvan.teslaru9@gmail.com", 
    include_dirs = [np.get_include()],
    ext_modules = 
        [Extension('vncmodule', ['vncmodule.cpp'], include_dirs=['headers'])]
    )


