OpenIGTLinkIFIO
=============

OpenIGTLink utility library.

Originally forked from OpenIGTLinkIF.

OpenIGTLinkIFIO contains several wrapper layers on top of OpenIGTLink. The code originates from OpenIGTLink/OpenIGTLinkIF. The main intent of the library is to share igtl code between Slicer, CustusX, IBIS, MITK and other systems.

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
Make sure Qt5 is in your path. *TBD: Qt4-support*

CTK must be build with VTK support: -DCTK_LIB_Visualization/VTK/Core:BOOL=ON. Set -DVTK_DIR:PATH=path/to/vtk/build as well.

The library can be included into other systems through the usual cmake framework. *Install not tested.*
