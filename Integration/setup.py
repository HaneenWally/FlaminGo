from setuptools import setup, Extension
from glob import glob
import os

files = glob(os.path.join(os.path.abspath('../'), 'Implementation', '*.cpp'))
# Compile *mysum.cpp* into a shared library 
setup(
    #...
    ext_modules=[Extension('implementation', files
                )],
)
# setup(
#     #...
#     ext_modules=[Extension('implementation', ['/Users/abdokaseb/Desktop/Term/MI/FlaminGo/Implementation/main.cpp'],)
#                  Extension('set_color', ['/Users/abdokaseb/Desktop/Term/MI/FlaminGo/Implementation/main.cpp'],), 
#                  Extension('reach_initial', ['/Users/abdokaseb/Desktop/Term/MI/FlaminGo/Implementation/main.cpp'],), 
#                  Extension('get_best_move', ['/Users/abdokaseb/Desktop/Term/MI/FlaminGo/Implementation/main.cpp'],), 
#                  Extension('make_move', ['/Users/abdokaseb/Desktop/Term/MI/FlaminGo/Implementation/main.cpp'],)
#                  ],
# )
