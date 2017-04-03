OpenIGTLinkIO
=============

OpenIGTLink utility library.

Originally forked from OpenIGTLinkIF.

OpenIGTLinkIO contains several wrapper layers on top of OpenIGTLink. The code originates from OpenIGTLink/OpenIGTLinkIF. The main intent of the library is to share igtl code between Slicer, CustusX, IBIS, MITK and other systems.

**Warning: This is a work in progress. The code is highly unstable!**

The library contains the following modules, ordered from few to lots of dependencies:
* **Converter Module** VTK<->OpenIGTLink: Standalone utility classes for converting between VTK and igtl formats.
* **Devices Module**: A library of device classes, one for each igtl message, VTK-dependent.
* **Logic Module**: An igtl client that used Devices to send/receive igtl messages over one or more Connections, VTK-dependent.
* **GUI Module**: User interface classes and widgets for the Logic Module. Qt and CTK-dependent.

An example application for running a standalone client is also available.

Build instructions
------------

Build with cmake:         
```
cmake -G"generator_name" \
      -DOpenIGTLink_DIR:PATH=path/to/igtl/build \
      -DVTK_DIR:PATH=path/to/vtk/build \
      -DCTK_DIR:PATH=path/to/ctk/build \
      path/to/source
```
Make sure Qt 4 or 5 is in your path.

CTK must be build with VTK support: -DCTK_LIB_Visualization/VTK/Core:BOOL=ON. Set -DVTK_DIR:PATH=path/to/vtk/build as well.

The library can be included into other systems through the usual cmake framework. *Install not tested.*

The following is an example of how to build the library with all prerequisites:
```
# prerequisites:
#
# cmake is in path
# qmake is in path

mkdir ~/dev
cd ~/dev

git clone git@github.com:Kitware/VTK.git
mkdir -p VTK_build
cd VTK_build
cmake ../VTK -DVTK_QT_VERSION:STRING=5 -DModule_vtkGUISupportQt:BOOL=ON
make -j6
cd ..

git clone git@github.com:commontk/CTK.git
mkdir -p CTK_build
cd CTK_build
cmake ../CTK -DCTK_QT_VERSION:STRING=5 -DCTK_LIB_Visualization/VTK/Core:BOOL=ON -DVTK_DIR:PATH=~/dev/VTK_build -DBUILD_TESTING:BOOL=OFF
make -j6
cd ..

git clone git@github.com:IGSIO/OpenIGTLink.git
mkdir -p OpenIGTLink_build
cd OpenIGTLink_build
cmake ../OpenIGTLink -DBUILD_TESTING:BOOL=OFF
make -j6


git clone git@github.com:IGSIO/OpenIGTLinkIO.git
mkdir -p OpenIGTLinkIO_build
cd OpenIGTLinkIO_build
cmake ../OpenIGTLinkIO -DOpenIGTLink_DIR:PATH=~/dev/OpenIGTLink_build -DVTK_DIR:PATH=~/dev/VTK_build -DCTK_DIR:PATH=~/dev/CTK_build 
make -j6
```


