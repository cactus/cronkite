from distutils.core import setup, Extension
cronkite = Extension(
    "cronkite",
    libraries = ['cronkite'],
    sources = ['pycronkite.c'])

setup(
    name = 'Cronkite',
    version = '1.0',
    description = 'python module for libcronkite',
    url = 'http://github.com/cactus/cronkite',
    ext_modules = [cronkite])

