/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer4/trunk/Modules/OpenIGTLinkIF/vtkSlicerOpenIGTLinkIFLogic.h $
  Date:      $Date: 2010-06-10 11:05:22 -0400 (Thu, 10 Jun 2010) $
  Version:   $Revision: 13728 $

==========================================================================*/

/// This class manages the logic associated with tracking device for IGT.

#ifndef __vtkIGTLIOLogic_h
#define __vtkIGTLIOLogic_h

// OpenIGTLink includes
#include <igtlImageMessage.h>
#include <igtlTransformMessage.h>

//#include "vtkSlicerBaseLogic.h"
//#include "vtkSlicerModuleLogic.h"
#include "vtkCallbackCommand.h"

//// MRML includes
//#include <vtkMRMLTransformNode.h>
//#include <vtkMRMLFiducialListNode.h>

// VTK includes
#include <vtkMultiThreader.h>

// STD includes
#include <vector>

// IGTLIO includes
#include "igtlioLogicExport.h"
#include "igtlioDevice.h"
#include "IGTLIOUtilities.h"

namespace igtlio
{

typedef vtkSmartPointer<class vtkIGTLIOLogic> vtkIGTLIOLogicPointer;

typedef vtkSmartPointer<class vtkIGTLIOConnector> vtkIGTLIOConnectorPointer;
typedef vtkSmartPointer<class vtkIGTLIOSession> vtkIGTLIOSessionPointer;


/// Logic is the manager for the IGTLIO module.
///
/// The module contains a number of Connectors, each with a specific
/// TCP/IP channel and a set of Devices that communicates through
/// their Connector.
///
///
/// Contents:
/// - list of connectors
/// - device factory usable by connectors
/// - timer that trigger main thread actions in the connectors.
///
/// Notifications:
/// - new/create/modified connectors and devices
///
/// Requirements:
///  - Call the PeriodicProcess() method every N ms in order to do the
///    main thread processing. This should be handled externally by a timer
///    or similar.
///
class OPENIGTLINKIO_LOGIC_EXPORT vtkIGTLIOLogic : public vtkObject
{
public:
  enum {
    //TODO: harmonize event handling for Logic, Connector, Device.
    ConnectionAddedEvent        = 118960,
    ConnectionAboutToBeRemovedEvent      = 118961,

    NewDeviceEvent        = 118949,
//    DeviceModifiedEvent   = 118950, // must listen to each specific device in order to get this one.
    RemovedDeviceEvent    = 118951,
    CommandQueryReceivedEvent = Device::CommandQueryReceivedEvent, // one of the connected COMMAND devices got a query
    CommandResponseReceivedEvent = Device::CommandResponseReceivedEvent // one of the connected COMMAND devices got a response
  };

 static vtkIGTLIOLogic *New();
 vtkTypeMacro(vtkIGTLIOLogic, vtkObject);
 void PrintSelf(ostream&, vtkIndent);

 vtkIGTLIOConnectorPointer CreateConnector();
 int RemoveConnector(unsigned int index);
 int GetNumberOfConnectors() const;
 vtkIGTLIOConnectorPointer GetConnector(unsigned int index);

 /// Start a server and return a Session representing the connection.
 /// If sync is BLOCKING, the call blocks until at client has connected to the server.
 vtkIGTLIOSessionPointer StartServer(int serverPort=-1, igtlio::SYNCHRONIZATION_TYPE sync=igtlio::BLOCKING, double timeout_s=5);

 /// Connect to the given server and return Session representing the connection.
 /// if sync is BLOCKING, the call blocks until the server responds or until timeout.
 vtkIGTLIOSessionPointer ConnectToServer(std::string serverHost, int serverPort=-1, igtlio::SYNCHRONIZATION_TYPE sync=igtlio::BLOCKING, double timeout_s=5);

 /// Call timer-driven routines for each connector
 void PeriodicProcess();

 //TODO: interface for accessing Devices
 unsigned int GetNumberOfDevices() const;
 void RemoveDevice(unsigned int index);
 DevicePointer GetDevice(unsigned int index);


protected:
 vtkIGTLIOLogic();
 virtual ~vtkIGTLIOLogic();

private:
 std::vector<vtkIGTLIOConnectorPointer> Connectors;

private:
  vtkIGTLIOLogic(const vtkIGTLIOLogic&); // Not implemented
  void operator=(const vtkIGTLIOLogic&); // Not implemented

  int CreateUniqueConnectorID() const;
  std::vector<DevicePointer> CreateDeviceList() const;

  vtkSmartPointer<class vtkCallbackCommand> NewDeviceCallback;
  vtkSmartPointer<class vtkCallbackCommand> RemovedDeviceCallback;

public:
  vtkSmartPointer<class vtkCallbackCommand> DeviceEventCallback;

};
} // namespace igtlio

#endif // __vtkIGTLIOLogic_h
