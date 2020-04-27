# Containers {#containers}

## General description
There are three base kinds of containers:

1. Par(ameter) container
2. Cal(ibration) container
3. Lookup table

Par container provides access to parameters which are related to the task execution, for example cluster finder, digitiser, etc.

Cal container and lookup table are related more to the individual channels of the detectors, e.g. mapping of the channels or calibration of the amplitude for each individual channel.

Containers are managed by SParManager class, which is responsible for reading and storing the containers. Parameter containers are stored in text files (param files). Each fine can store multiple containers, each with unique names.

Container in the text is defined in following way:
* pair of brackets `[]` with the name of container,
* optional body of the container following the name (container can be empty),
* optional comments starting with `#`.

Example:

    [Container1]
    # body here with multiple lines
    ...
    ...
    [Container2]
    # another body
    ...

While reading, each container from the file is stored in a SContainer class, which holds name and each single line related to the container.

To access the container, the container must be registered in the manager. For each kind of container there is respective function:

1. SParManager::addParameterContainer()
2. SParManager::addCalibrationContainer()
3. SParManager::addLookupContainer()

Each of the functions accepts container name and respective container object. Multiple containers can utilise the same class type.

The parameter file must be parsed with SParManager::parseSource() and the names and content of the containers must be stored in the manager before any container is registered. If the container name is not present in the manager during
registration the program fill abort. This is assure that all required parameters are present.

During registration, the respective container is populated with values.

## Par container

The par container is represented by a base pure virtual class SPar, which provides interface to access the data. The derived class must provide
* SPar::getParams(),
* SPar::putParams() and
* SPar::clear()

functions, class members storing the values, and their access functions.

The two first functions accept SParContainer object which is handled by SParManager, and the functions called by SParManager only.

Each par container contains set of parameters defined by:
* name,
* type: `Int_t`, `Float_t` or `Double_t`,
* and value or array of values.

Each parameter has form of:

    name: type value(s)

and can be broken in multiple lines by a `\` delimeter at the end of the line, however the `name` and the `type` must be in a single line. An example:

    par1: Float_t 10
    par2: Int_t 10 20 30
    par3: Double_t \
      1.1 1.2 1.3
    par4: Int_t 1 2 3 \
      4 5 6

SParManager during parsing the par container creates object of SParContainer which holds objects containing name, type and values. Access to these values are provided by member functions SParContainer::fill() (obtain the parameter value) and SParContainer::add() (add or overwrite parameter with value). Each of these member functions is overloaded for one of the three types and arrays of these types.

Finally, the SParManager calls SPar::getParams() or SPar::putParams() and passes an object of SParContainer in order to initialize SPar with the values or retrieve the values, respectively.

A such architecture is required due to fact that container stores linear arrays or single values, and SPar may provide multidimensional arrays. The SPar::getParams() and SPar::putParams() provide interpretation of the data. They are also responsible for checking validity of the data, e.g. whether the array in the param file has the same length like expected by par container, and if not, then report problem.

See SFibersStackGeomPar for typical implementaion of par container.

## Lookup tables

## Calibration containers
