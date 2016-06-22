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

//// OpenIGTLinkIF MRML includes
//#include "vtkIGTLToMRMLLinearTransform.h"
//#include "vtkIGTLToMRMLPosition.h"
//#include "vtkIGTLToMRMLImageMetaList.h"
//#include "vtkIGTLToMRMLLabelMetaList.h"
//#include "vtkIGTLToMRMLPoints.h"
//#include "vtkIGTLToMRMLPolyData.h"
//#include "vtkIGTLToMRMLTrackingData.h"
//#include "vtkIGTLToMRMLStatus.h"
//#include "vtkIGTLToMRMLSensor.h"
//#include "vtkIGTLToMRMLString.h"
//#include "vtkIGTLToMRMLTrajectory.h"

//#include "vtkSlicerOpenIGTLinkIFModuleLogicExport.h"

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
#include "vtkIGTLIODevice.h"

typedef vtkSmartPointer<class vtkIGTLIOLogic> vtkIGTLIOLogicPointer;
typedef vtkSmartPointer<class vtkIGTLIOConnector> vtkIGTLIOConnectorPointer;
typedef vtkSmartPointer<class vtkIGTLIODevice> vtkIGTLIODevicePointer;
typedef vtkSmartPointer<class vtkIGTLIOLogic> vtkIGTLIOLogicPointer;


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
  };

 static vtkIGTLIOLogic *New();
 vtkTypeMacro(vtkIGTLIOLogic, vtkObject);
 void PrintSelf(ostream&, vtkIndent);

 vtkIGTLIOConnectorPointer CreateConnector();
 int RemoveConnector(int index);
 int GetNumberOfConnectors() const;
 vtkIGTLIOConnectorPointer GetConnector(int index);

 /// Call timer-driven routines for each connector
 void PeriodicProcess();

 //TODO: interface for accessing Devices
 int GetNumberOfDevices() const;
 void RemoveDevice(int index);
 vtkIGTLIODevicePointer GetDevice(int index);


protected:
 vtkIGTLIOLogic();
 virtual ~vtkIGTLIOLogic();

private:
 std::vector<vtkIGTLIOConnectorPointer> Connectors;

private:
  vtkIGTLIOLogic(const vtkIGTLIOLogic&); // Not implemented
  void operator=(const vtkIGTLIOLogic&); // Not implemented

  int CreateUniqueConnectorID() const;
  std::vector<vtkIGTLIODevicePointer> CreateDeviceList() const;

  vtkSmartPointer<class vtkCallbackCommand> NewDeviceCallback;
  vtkSmartPointer<class vtkCallbackCommand> RemovedDeviceCallback;

};

#endif // __vtkIGTLIOLogic_h
