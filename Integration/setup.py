from setuptools import setup, Extension
import os
from glob import glob

cppFiles = glob(os.path.join(os.path.abspath('../'), 'Implementation', '*.cpp') )
# Compile *mysum.cpp* into a shared library 
module = Extension(name = 'implementation', 
                sources = cppFiles
                )
setup(
    #...
    ext_modules=[module],
)
# setup(
#     #...
#     ext_modules=[Extension('fill_initial', ['/Users/abdokaseb/Desktop/Term/MI/FlaminGo/Implementation/main.cpp'],),
#                  Extension('set_color', ['/Users/abdokaseb/Desktop/Term/MI/FlaminGo/Implementation/main.cpp'],), 
#                  Extension('reach_initial', ['/Users/abdokaseb/Desktop/Term/MI/FlaminGo/Implementation/main.cpp'],), 
#                  Extension('get_best_move', ['/Users/abdokaseb/Desktop/Term/MI/FlaminGo/Implementation/main.cpp'],), 
#                  Extension('make_move', ['/Users/abdokaseb/Desktop/Term/MI/FlaminGo/Implementation/main.cpp'],)
#                  ],
# )
