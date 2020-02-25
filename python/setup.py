from distutils.core import setup, Extension
from Cython.Build import cythonize

modules = [
    Extension(
        "mgeanttrack",
        sources=[
            "/home/dziadu/projects/sifi-framework/python/mgeanttrack.pyx",
        ],
        language="c++",              # generate C++ code
        extra_compile_args=["-std=c++14"],
        include_dirs=["/usr/lib/root/6.18/include"],
        library_dirs=["/home/dziadu/projects/sifi-framework/build/lib"],
        libraries=["GeantSim", "MAPT"],
    ),
    Extension(
        "mgeantfibersraw",
        sources=[
            "/home/dziadu/projects/sifi-framework/python/mgeantfibersraw.pyx",
        ],
        language="c++",              # generate C++ code
        extra_compile_args=["-std=c++14"],
        include_dirs=["/usr/lib/root/6.18/include"],
        library_dirs=["/home/dziadu/projects/sifi-framework/build/lib"],
        libraries=["GeantSim", "MAPT"],
    ),
    Extension(
        "mfibersstackcalsim",
        sources=[
            "/home/dziadu/projects/sifi-framework/python/mfibersstackcalsim.pyx",
        ],
        language="c++",              # generate C++ code
        extra_compile_args=["-std=c++14"],
        include_dirs=["/usr/lib/root/6.18/include"],
        library_dirs=["/home/dziadu/projects/sifi-framework/build/lib"],
        libraries=["FibersStack", "MAPT"],
    ),
    Extension(
        "mfibersstackcal",
        sources=[
            "/home/dziadu/projects/sifi-framework/python/mfibersstackcal.pyx",
        ],
        language="c++",              # generate C++ code
        extra_compile_args=["-std=c++14"],
        include_dirs=["/usr/lib/root/6.18/include"],
        library_dirs=["/home/dziadu/projects/sifi-framework/build/lib"],
        libraries=["FibersStack", "MAPT"],
    ),
    Extension(
        "mapt_python",
        sources=[
            "/home/dziadu/projects/sifi-framework/python/mapt_python.pyx",
            "/home/dziadu/projects/sifi-framework/python/MAPT.cpp",
        ],
        language="c++",              # generate C++ code
        extra_compile_args=["-std=c++14"],
        include_dirs=[
            "/home/dziadu/projects/sifi-framework/lib/base",
            "/home/dziadu/projects/sifi-framework/lib/geant",
            "/home/dziadu/projects/sifi-framework/lib/fibers_stack",
            "/usr/lib/root/6.18/include"
        ],
        library_dirs=["/home/dziadu/projects/sifi-framework/build/lib"],
        libraries=["FibersStack", "GeantSim", "MAPT"],
    ),
]

setup(
    name='pyMAPT',
    package_dir={'' : '/home/dziadu/projects/sifi-framework/python' },
    ext_modules=cythonize(modules, build_dir="/home/dziadu/projects/sifi-framework/build/python")
)
