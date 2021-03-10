# SiFi Framework Documentation

The SiFi Framework contains all the libraries and applications,
which build the basis for the data analysis of the SiFi-CC detector.
This includes the simulation, the event display, the geometry representation of
the detector and the data structure.

## Preparing the framework

### Requirements

* c++17
* root-6
* geant4

### Getting source

Get the source from the project git page:

    git clone https://github.com/SiFi-CC/sifi-framework.git
    cd sifi-framework
    git submodule init
    git submodule update

### Compilation and installation

We assume the working directory is `sifi-framework` of the git repository.

    mkdir build
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=__some_location__
    make
    make install

## The framework

The framework is divided into three main components:

1. Processing tasks
2. Data category
3. Parameter containers

### Processing tasks

They read data from one category, process, and store in the other. One task can read more categories, and write to multiple categories. The tasks are executed in predefined order.

### Data category

It store data on each stage of the analysis. They are stored in the root tree as branches. Category contain header information and the data object storing actuall data. All values are borwsable in the tree.

### Parameter containers

There are three types of the parameter container

1. Par(ameter) container
2. Cal(ibration) container
3. Lookup container

Par container store general parameters, like parameters for digitizer, or track finder, which are independend from input data.

Cal containers store calibration parameters, individual for input data, e.g. for channels.

Lookup tables are similar to the cal containers, are individual for each channel, however use a slightly different interface than cal containers.
