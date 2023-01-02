![SiFi-CC: Analysis fremwork](https://img.shields.io/badge/SiFi--CC-Analysis%20framework-orange)
![Coveralls](https://img.shields.io/coveralls/github/SiFi-CC/sifi-framework)
[![Coverage Status](https://coveralls.io/repos/github/SiFi-CC/sifi-framework/badge.svg?branch=master)](https://coveralls.io/github/SiFi-CC/sifi-framework?branch=refactor_parameters)
![Travis (.com)](https://img.shields.io/travis/com/SiFi-CC/sifi-framework)
![GitHub Workflow Status (branch)](https://img.shields.io/github/workflow/status/SiFI-CC/sifi-framework/CMake%20Build%20Matrix/master)
![GitHub](https://img.shields.io/github/license/SIFi-CC/sifi-framework)
![GitHub release (latest by date)](https://img.shields.io/github/v/release/SiFi-cc/sifi-framework)
![GitHub tag (latest by date)](https://img.shields.io/github/v/tag/SiFi-CC/sifi-framework)


# SiFi Framework Documentation

The SiFi Framework contains all the libraries and applications,
which build the basis for the data analysis of the SiFi-CC detector.
This includes the simulation, the event display, the geometry representation of
the detector and the data structure.

## Preparing the framework

### Requirements

* c++17 comaptible compiler (gcc-9 or higher)
* root-6 (tested with 6.22.06 and higher, compiled with c++17 support)
* geant4 (for running simulations, optional)

### Dependencies

The framework is always tested on the Latest Ubuntu. The bare system requires a few packages to be installed:

* cmake (the build system)
* libgtest-dev (for the tests)
* lcov (for code coverage)
* rapidjson-dev (MySQL interface)
* libcurl4-openssl-dev (for cpr)

The default root in Ubuntu is compiled against c++14. We provide our custom toot compilation available from `http://gccb.if.uj.edu.pl/code/root-cpp17_6.24.06_amd64.deb` which has following dependencies:

* python3-numpy
* libtbb-dev
* libxxhash0

An example how to install dependencies and prepare environment can be found in github workflow in `.github/workflows/configurations.yml` or below:

    bash
    sudo apt update
    sudo apt install cmake libgtest-dev lcov rapidjson-dev libcurl4-openssl-dev -y
    sudo apt install python3-numpy libtbb-dev libxxhash0 -y
    wget http://gccb.if.uj.edu.pl/code/root-cpp17_6.24.06_amd64.deb
    sudo dpkg -i root-cpp17_6.24.06_amd64.deb
    . /etc/profile

For other Linux distributions, you need to handle dependencies manually.

### Getting source

Get the source from the project git page:

    git clone https://github.com/SiFi-CC/sifi-framework.git
    cd sifi-framework

### Compilation and installation

We assume the working directory is `sifi-framework` of the git repository.

    mkdir build
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=__some_location__
    make
    make install

To disable tests, add `-DENABLE_TESTING=OFF` to the cmake arguments.

## The framework

The framework is divided into three main components:

1. Processing tasks
2. Data category
3. Parameter containers

### Processing tasks

They read data from one category, process, and store in the other. One task can read more categories, and write to multiple categories. The tasks are executed in predefined order.

### Data category

It store data on each stage of the analysis. They are stored in the root tree as branches. Category contain header information and the data object storing actual data. All values are browseable in the tree.

### Parameter containers

There are three types of the parameter container

1. Par(ameter) container
2. Cal(ibration) container
3. Lookup container

Par container store general parameters, like parameters for digitizer, or track finder, which are independent from input data.

Cal containers store calibration parameters, individual for input data, e.g. for channels.

Lookup tables are similar to the cal containers, are individual for each channel, however use a slightly different interface than cal containers.
