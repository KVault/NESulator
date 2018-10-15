from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

examples_extension = Extension(
    name="guicython",
    sources=["guicython.pyx"],
    libraries=["libGUI_Interface"]
)
setup(
    name="guicython",
    ext_modules=cythonize([examples_extension])
)