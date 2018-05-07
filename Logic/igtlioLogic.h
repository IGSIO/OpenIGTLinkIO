/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer4/trunk/Modules/OpenIGTLinkIF/vtkSlicerOpenIGTLinkIFLogic.h $
  Date:      $Date: 2010-06-10 11:05:22 -0400 (Thu, 10 Jun 2010) $
  Version:   $Revision: 13728 $

==========================================================================*/
#ifndef IGTLIOLOGIC_H
#define IGTLIOLOGIC_H

#include <vector>
#include <vtkCallbackCommand.h>
#include <vtkMultiThreader.h>
#include <igtlImageMessage.h>
#include <igtlTransformMessage.h>
#include "igtlioLogicExport.h"
#include "igtlioDevice.h"
#include "igtlioConnector.h"
#include "igtlioUtilities.h"

typedef vtkSmartPointer<class igtlioLogic> igtlioLogicPointer;
typedef vtkSmartPointer<class igtlioConnector> igtlioConnectorPointer;
typedef vtkSmartPointer<class igtlioSession> igtlioSessionPointer;


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
class OPENIGTLINKIO_LOGIC_EXPORT igtlioLogic : public vtkObject
{
public:
  enum {
    //TODO: harmonize event handling for Logic, Connector, Device.
    ConnectionAddedEvent        = 118970,
    ConnectionAboutToBeRemovedEvent      = 118971,

    NewDeviceEvent        = 118949,
    DeviceModifiedEvent   = igtlioConnector::DeviceContentModifiedEvent, // listen to the custom event invoked from connector.
    RemovedDeviceEvent    = 118951,
  };

 static igtlioLogic *New();
 vtkTypeMacro(igtlioLogic, vtkObject);
 void PrintSelf(ostream&, vtkIndent) VTK_OVERRIDE;

 igtlioConnectorPointer CreateConnector();
 int RemoveConnector(unsigned int index);
 int RemoveConnector(igtlioConnectorPointer connector);
 int GetNumberOfConnectors() const;
 igtlioConnectorPointer GetConnector(unsigned int index);

 /// Start a server and return a Session representing the connection.
 /// If sync is BLOCKING, the call blocks until at client has connected to the server.
 igtlioSessionPointer StartServer(int serverPort=-1, IGTLIO_SYNCHRONIZATION_TYPE sync=IGTLIO_BLOCKING, double timeout_s=5);

 /// Connect to the given server and return Session representing the connection.
 /// if sync is BLOCKING, the call blocks until the server responds or until timeout.
 igtlioSessionPointer ConnectToServer(std::string serverHost, int serverPort=-1, IGTLIO_SYNCHRONIZATION_TYPE sync=IGTLIO_BLOCKING, double timeout_s=5);

 /// Call timer-driven routines for each connector
 void PeriodicProcess();

 //TODO: interface for accessing Devices
 unsigned int GetNumberOfDevices() const;
 void RemoveDevice(unsigned int index);
 igtlioDevicePointer GetDevice(unsigned int index);
 int ConnectorIndexFromDevice( igtlioDevicePointer d );


protected:
  igtlioLogic();
 virtual ~igtlioLogic();

private:
 std::vector<igtlioConnectorPointer> Connectors;

private:
  igtlioLogic(const igtlioLogic&); // Not implemented
  void operator=(const igtlioLogic&); // Not implemented

  int CreateUniqueConnectorID() const;
  std::vector<igtlioDevicePointer> CreateDeviceList() const;
  int RemoveConnector(std::vector<igtlioConnectorPointer>::iterator toRemove);

  vtkSmartPointer<class vtkCallbackCommand> NewDeviceCallback;
  vtkSmartPointer<class vtkCallbackCommand> RemovedDeviceCallback;
  vtkSmartPointer<class vtkCallbackCommand> CommandEventCallback;

public:
  vtkSmartPointer<class vtkCallbackCommand> DeviceEventCallback;

};

#endif // IGTLIOLOGIC_H
