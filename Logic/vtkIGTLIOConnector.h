/*=auto=========================================================================

  Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLCurveAnalysisNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
#ifndef __vtkIGTLIOConnector_h
#define __vtkIGTLIOConnector_h

//// OpenIGTLinkIF MRML includes
//#include "vtkIGTLToMRMLBase.h"
//#include "vtkMRMLIGTLQueryNode.h"
//#include "vtkSlicerOpenIGTLinkIFModuleMRMLExport.h"

// OpenIGTLink includes
#include <igtlServerSocket.h>
#include <igtlClientSocket.h>

// IGTLIO includes
#include "igtlioLogicExport.h"
#include "vtkIGTLIODevice.h"
#include "vtkIGTLIODeviceFactory.h"
#include "vtkIGTLIOObject.h"

//// MRML includes
//#include <vtkMRML.h>
//#include <vtkMRMLNode.h>
//#include <vtkMRMLStorageNode.h>

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>
#include <vtkWeakPointer.h>

// STD includes
#include <string>
#include <map>
#include <vector>
#include <set>

typedef vtkSmartPointer<class vtkIGTLIOConnector> vtkIGTLIOConnectorPointer;
typedef vtkSmartPointer<class vtkIGTLIODevice> vtkIGTLIODevicePointer;
typedef vtkSmartPointer<class vtkIGTLIOCircularBuffer> vtkIGTLIOCircularBufferPointer;

typedef vtkSmartPointer<class vtkMutexLock> vtkMutexLockPointer;
typedef vtkSmartPointer<class vtkMultiThreader> vtkMultiThreaderPointer;

/// An OpenIGTLink connection over one TCP/IP port.
///
/// Features:
///  - Manages a list of devices that communicates over the port.
///  - Incoming igtl Messages are sent to the corresponding device,
///    and a notification is emitted in the form of a New/Modified Device.
///  - New devices can be added and modified through the interface
///  - Actions (send, get, start, stop) can be requested for each Device.
///
/// Communication:
///   All data is received from igtl in a separate thread, then stored in a receive buffer.
///   The main thread
///     - Processes all send requests, i.e. converts them to messages and
///       sends them over igtl.
///     - Interprets messages from the receive buffer, i.e. converts messages
///       to Device content or handles query responses.
///
/// Requirements:
///  - Call the Start() method in order to start the communication thread.
///  - Call the PeriodicProcess() method every N ms in order to do the
///    main thread processing. This should be handled externally by a timer
///    or similar.
///
class OPENIGTLINKIO_LOGIC_EXPORT vtkIGTLIOConnector : public vtkIGTLIOObject
{
public:
//added methods:
 //TODO: Notifications: New/Remove/Modify Device, message received?

  /// Call periodically to perform processing in the main thread.
  /// Suggested timeout 5ms.
  void PeriodicProcess();

 /// Add a new Device.
 /// If a Device with an identical device_id already exist, the method will fail.
 int AddDevice(vtkIGTLIODevicePointer device); // TODO look at OnNodeReferenceAdded
 int GetNumberOfDevices() const;
 int RemoveDevice(int index); //TODO: look at OnNodeReferenceRemoved
 /// Get the given Device. This can be used to modify the Device contents.
 vtkIGTLIODevicePointer GetDevice(int index);
 vtkIGTLIODevicePointer GetDevice(std::string device_name);

 /// Request the given Device to send a message with the given prefix.
 /// An undefined prefix means sending the normal message.
 int SendMessage(std::string device_id, vtkIGTLIODevice::MESSAGE_PREFIX=vtkIGTLIODevice::MESSAGE_PREFIX_NOT_DEFINED);

 vtkGetMacro( DeviceFactory, vtkIGTLIODeviceFactoryPointer );
 vtkSetMacro( DeviceFactory, vtkIGTLIODeviceFactoryPointer );
 //

 public:

  //----------------------------------------------------------------
  // Constants Definitions
  //----------------------------------------------------------------

  // Events
  enum {
    ConnectedEvent        = 118944,
    DisconnectedEvent     = 118945,
    ActivatedEvent        = 118946,
    DeactivatedEvent      = 118947,
    ReceiveEvent          = 118948,
    NewDeviceEvent        = 118949,
    DeviceModifiedEvent   = 118950,
    RemovedDeviceEvent    = 118951,
  };

  enum {
    TYPE_NOT_DEFINED,
    TYPE_SERVER,
    TYPE_CLIENT,
    NUM_TYPE
  };

  static const char* ConnectorTypeStr[vtkIGTLIOConnector::NUM_TYPE];

  enum {
    STATE_OFF,
    STATE_WAIT_CONNECTION,
    STATE_CONNECTED,
    NUM_STATE
  };

  static const char* ConnectorStateStr[vtkIGTLIOConnector::NUM_STATE];
  
  enum {
    IO_UNSPECIFIED = 0x00,
    IO_INCOMING   = 0x01,
    IO_OUTGOING   = 0x02,
  };
  
  enum {
    PERSISTENT_OFF,
    PERSISTENT_ON,
  };

  typedef struct {
    std::string   name;
    std::string   type;
    int           io;
  } DeviceInfoType;

  typedef struct {
    int           lock;
    int           second;
    int           nanosecond;
  } NodeInfoType;

//  typedef std::map<int, DeviceInfoType>   DeviceInfoMapType;   // Device list:  index is referred as
//                                                               // a device id in the connector.
//  typedef std::set<int>                   DeviceIDSetType;
//  typedef std::list< vtkSmartPointer<vtkIGTLToMRMLBase> >   MessageConverterListType;
//  typedef std::vector< vtkSmartPointer<vtkMRMLNode> >       MRMLNodeListType;
//  typedef std::map<std::string, NodeInfoType>       NodeInfoMapType;
//  typedef std::map<std::string, vtkSmartPointer <vtkIGTLToMRMLBase> > MessageConverterMapType;

 public:

  //----------------------------------------------------------------
  // Standard methods for MRML nodes
  //----------------------------------------------------------------

  static vtkIGTLIOConnector *New();
  vtkTypeMacro(vtkIGTLIOConnector,vtkIGTLIOObject);

  void PrintSelf(ostream& os, vtkIndent indent);

//  virtual vtkMRMLNode* CreateNodeInstance();

//  // Description:
//  // Set node attributes
//  virtual void ReadXMLAttributes( const char** atts);

//  // Description:
//  // Write this node's information to a MRML file in XML format.
//  virtual void WriteXML(ostream& of, int indent);

//  // Description:
//  // Copy the node's attributes to this object
//  virtual void Copy(vtkMRMLNode *node);

//  // Description:
//  // Get node XML tag name (like Volume, Model)
//  virtual const char* GetNodeTagName()
//    {return "IGTLConnector";};

//  // method to propagate events generated in mrml
//  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );

//  //BTX
//  virtual void OnNodeReferenceAdded(vtkMRMLNodeReference *reference);

//  virtual void OnNodeReferenceRemoved(vtkMRMLNodeReference *reference);

//  virtual void OnNodeReferenceModified(vtkMRMLNodeReference *reference);
//  //ETX

 protected:
  //----------------------------------------------------------------
  // Constructor and destroctor
  //----------------------------------------------------------------

  vtkIGTLIOConnector();
  ~vtkIGTLIOConnector();
  vtkIGTLIOConnector(const vtkIGTLIOConnector&);
  void operator=(const vtkIGTLIOConnector&);


 public:
  //----------------------------------------------------------------
  // Connector configuration
  //----------------------------------------------------------------

  vtkGetMacro( Name, std::string );
  vtkSetMacro( Name, std::string );
  vtkGetMacro( UID, int );
  vtkSetMacro( UID, int );
  vtkGetMacro( ServerPort, int );
  vtkSetMacro( ServerPort, int );
  vtkGetMacro( Type, int );
  vtkSetMacro( Type, int );
  vtkGetMacro( State, int );
  vtkSetMacro( RestrictDeviceName, int );
  vtkGetMacro( RestrictDeviceName, int );

  // Controls if active connection will be resumed when
  // scene is loaded (cf: PERSISTENT_ON/_OFF)
  vtkSetMacro( Persistent, int );
  vtkGetMacro( Persistent, int );

  const char* GetServerHostname();
  void SetServerHostname(std::string str);

  int SetTypeServer(int port);
  int SetTypeClient(std::string hostname, int port);

  vtkGetMacro( CheckCRC, int );
  void SetCheckCRC(int c);

  //----------------------------------------------------------------
  // Thread Control
  //----------------------------------------------------------------

  int Start();
  int Stop();

private:

  static void* ThreadFunction(void* ptr);

  //----------------------------------------------------------------
  // OpenIGTLink Message handlers
  //----------------------------------------------------------------
  int WaitForConnection(); // called from Thread
  int ReceiveController(); // called from Thread
  int SendData(int size, unsigned char* data);
  int Skip(int length, int skipFully=1);

  //----------------------------------------------------------------
  // Circular Buffer
  //----------------------------------------------------------------

  typedef std::vector<std::string> NameListType;
  unsigned int GetUpdatedBuffersList(NameListType& nameList); // TODO: this will be moved to private
  vtkIGTLIOCircularBufferPointer GetCircularBuffer(std::string& key);     // TODO: Is it OK to use device name as a key?

  //----------------------------------------------------------------
  // Device Lists
  //----------------------------------------------------------------

  // Description:
  // Import received data from the circular buffer to the MRML scne.
  // This is currently called by vtkOpenIGTLinkIFLogic class.
  void ImportDataFromCircularBuffer();

  // Description:
  // Import events from the event buffer to the MRML scene.
  // This is currently called by vtkOpenIGTLinkIFLogic class.
  void ImportEventsFromEventBuffer();

  // Description:
  // Push all outgoing messages to the network stream, if permitted.
  // This function is used, when the connection is established. To permit the OpenIGTLink IF
  // to push individual "outgoing" MRML nodes, set "OpenIGTLinkIF.pushOnConnection" attribute to 1. 
  void PushOutgoingMessages();

//  // Description:
//  // A function to explicitly push node to OpenIGTLink. The function is called either by
//  // external nodes or MRML event hander in the connector node.
    int PushNode(vtkIGTLIODevicePointer node, int event=-1);

 protected:
  // Description:
  // Inserts the eventId to the EventQueue, and the event will be invoked from the main thread
  void RequestInvokeEvent(unsigned long eventId); // might be called from Thread

  // Description:
  // Reeust to push all outgoing messages to the network stream, if permitted.
  // This function is used, when the connection is established. To permit the OpenIGTLink IF
  // to push individual "outgoing" MRML nodes, set "OpenIGTLinkIF.pushOnConnection" attribute to 1. 
  // The request will be processed in PushOutgonigMessages().
  void RequestPushOutgoingMessages(); // called from Thread

 protected:

  //----------------------------------------------------------------
  // Devices
  //----------------------------------------------------------------

  std::vector<vtkIGTLIODevicePointer> Devices;

  //----------------------------------------------------------------
  // Connector configuration
  //----------------------------------------------------------------
  std::string Name;
  int UID; /// unique ID for this connector
  int Type;
  int State;
  int Persistent;

  //----------------------------------------------------------------
  // Thread and Socket
  //----------------------------------------------------------------

  vtkMultiThreaderPointer Thread;
  vtkMutexLockPointer     Mutex;
  igtl::ServerSocket::Pointer  ServerSocket;
  igtl::ClientSocket::Pointer  Socket;
  int               ThreadID;
  int               ServerPort;
  int               ServerStopFlag;

  std::string       ServerHostname;

  //----------------------------------------------------------------
  // Data
  //----------------------------------------------------------------


  typedef std::map<std::string, vtkIGTLIOCircularBufferPointer> CircularBufferMap;
  CircularBufferMap Buffer;

  vtkMutexLockPointer CircularBufferMutex;
  int           RestrictDeviceName;  // Flag to restrict incoming and outgoing data by device names

  // Event queueing mechanism is needed to send all event notifications from the main thread.
  // Events can be pushed to the end of the EventQueue by calling RequestInvoke from any thread,
  // and they will be Invoked in the main thread.
  std::list<unsigned long> EventQueue;
  vtkMutexLockPointer EventQueueMutex;

  // Flag for the push outoing message request
  // If the flag is ON, the external timer will update the outgoing nodes with 
  // "OpenIGTLinkIF.pushOnConnection" attribute to push the nodes to the network.
  int PushOutgoingMessageFlag;
  vtkMutexLockPointer PushOutgoingMessageMutex;

  vtkIGTLIODeviceFactoryPointer DeviceFactory;

  int CheckCRC;

};

#endif

