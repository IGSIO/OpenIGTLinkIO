/*=auto=========================================================================

  Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLCurveAnalysisNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
#ifndef IGTLIOCONNECTOR_H
#define IGTLIOCONNECTOR_H

// OpenIGTLink includes
#include <igtlServerSocket.h>
#include <igtlClientSocket.h>

// IGTLIO includes
#include "igtlioLogicExport.h"
#include "igtlioDevice.h"
#include "igtlioDeviceFactory.h"
#include "igtlioObject.h"
#include "igtlioUtilities.h"
#include "igtlioCommand.h"

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>
#include <vtkWeakPointer.h>

// STD includes
#include <string>
#include <map>
#include <vector>
#include <set>
#include <queue>

typedef vtkSmartPointer<class vtkMutexLock> vtkMutexLockPointer;
typedef vtkSmartPointer<class vtkMultiThreader> vtkMultiThreaderPointer;
typedef std::vector< vtkSmartPointer<igtlioDevice> >   igtlioMessageDeviceListType;
typedef std::deque<igtlioCommandPointer> igtlioCommandDequeType;

typedef vtkSmartPointer<class igtlioConnector> igtlioConnectorPointer;
typedef vtkSmartPointer<class igtlioCircularBuffer> igtlioCircularBufferPointer;
typedef vtkSmartPointer<class igtlioCircularSectionBuffer> igtlioCircularSectionBufferPointer;


enum CONNECTION_ROLE
{
  CONNECTION_ROLE_NOT_DEFINED,
  CONNECTION_ROLE_SERVER,
  CONNECTION_ROLE_CLIENT,
  CONNECTION_ROLE_TYPE
};


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
class OPENIGTLINKIO_LOGIC_EXPORT igtlioConnector : public vtkIGTLIOObject
{
public:
  //added methods:
  //TODO: Notifications: New/Remove/Modify Device, message received?

  /// Call periodically to perform processing in the main thread.
  /// Suggested timeout 5ms.
  void PeriodicProcess();

  int SendCommand(igtlioCommandPointer command);
  igtlioCommandPointer SendCommand(std::string command, std::string content, IGTLIO_SYNCHRONIZATION_TYPE synchronized, double timeout_s = 5.0, igtl::MessageBase::MetaDataMap* metaData = NULL, int clientID = -1);

  // Currently each connector can only connect to one client
  //igtlioCommandDequeType SendCommandToAllClients(std::string name, std::string content, double timeout_s=5.0, igtl::MessageBase::MetaDataMap* metaData=NULL);

  int SendCommandResponse(int commandId, int clientId=-1);
  int SendCommandResponse(igtlioCommandPointer command);

  /// Cancel the command
  /// Note: This does not send anything to the original recipient of the command.
  /// It simply removes the command from the list of outgoing commands so that a response event will not be invoked
  void CancelCommand(int commandId, int clientId);
  void CancelCommand(igtlioCommandPointer);

  igtlioDevicePointer AddDeviceIfNotPresent(igtlioDeviceKeyType key);

  /// Add a new Device.
  /// If a Device with an identical device_id already exist, the method will fail.
  int AddDevice(igtlioDevicePointer device); // TODO look at OnNodeReferenceAdded
  unsigned int GetNumberOfDevices() const;
  void RemoveDevice(int index); //TODO: look at OnNodeReferenceRemoved
  int RemoveDevice(igtlioDevicePointer device);
  /// Get the given Device. This can be used to modify the Device contents.

  /// invoke event if device content modified
  void DeviceContentModified(vtkObject *caller, unsigned long event, void *callData );
  igtlioDevicePointer GetDevice(int index);
  igtlioDevicePointer GetDevice(igtlioDeviceKeyType key);
  bool HasDevice( igtlioDevicePointer d );

  /// Request the given Device to send a message with the given prefix.
  /// An undefined prefix means sending the normal message.
  int SendMessage(igtlioDeviceKeyType device_id, igtlioDevice::MESSAGE_PREFIX=igtlioDevice::MESSAGE_PREFIX_NOT_DEFINED);

  igtlioDeviceFactoryPointer GetDeviceFactory();
  void SetDeviceFactory(igtlioDeviceFactoryPointer val);

public:

  // Events
  enum {
    ConnectedEvent        = 118944,
    DisconnectedEvent     = 118945,
    ActivatedEvent        = 118946,
    DeactivatedEvent      = 118947,
//    ReceiveEvent          = 118948,
    NewDeviceEvent        = 118949,
    DeviceContentModifiedEvent   = 118950, // invoked by the devices
    RemovedDeviceEvent    = 118951,
  };

  enum {
    TYPE_NOT_DEFINED,
    TYPE_SERVER,
    TYPE_CLIENT,
    NUM_TYPE
  };

  static const char* ConnectorTypeStr[igtlioConnector::NUM_TYPE];

  enum {
    STATE_OFF,
    STATE_WAIT_CONNECTION,
    STATE_CONNECTED,
    NUM_STATE
  };

  static const char* ConnectorStateStr[igtlioConnector::NUM_STATE];

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

public:

  static igtlioConnector *New();
  vtkTypeMacro(igtlioConnector, vtkIGTLIOObject);

  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

protected:
  igtlioConnector();
  ~igtlioConnector();
  igtlioConnector(const igtlioConnector&);
  void operator=(const igtlioConnector&);

public:
  vtkGetMacro( Name, std::string );
  vtkSetMacro( Name, std::string );
  vtkGetMacro( UID, int );
  vtkSetMacro( UID, int );
  vtkGetMacro( ServerPort, int );
  vtkSetMacro( ServerPort, int );
  vtkGetMacro( Type, int );
  vtkSetMacro( Type, int );
  vtkSetMacro( State, int);
  vtkGetMacro( State, int );
  vtkSetMacro( RestrictDeviceName, int );
  vtkGetMacro( RestrictDeviceName, int );
  vtkSetMacro( PushOutgoingMessageFlag, int );
  vtkGetMacro( PushOutgoingMessageFlag, int );

  // Controls if active connection will be resumed when
  // scene is loaded (cf: PERSISTENT_ON/_OFF)
  vtkSetMacro( Persistent, int );
  vtkGetMacro( Persistent, int );

  const char* GetServerHostname();
  void SetServerHostname(std::string str);

  int SetTypeServer(int port);
  int SetTypeClient(std::string hostname, int port);

  vtkGetMacro( CheckCRC, bool);
  void SetCheckCRC(bool c);

  bool IsConnected();

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

  typedef std::vector<igtlioDeviceKeyType> NameListType;
  unsigned int GetUpdatedSectionBuffersList(NameListType& nameList); // TODO: this will be moved to private
  igtlioCircularSectionBufferPointer GetCircularSectionBuffer(const igtlioDeviceKeyType& key);     // TODO: Is it OK to use device name as a key?

  //----------------------------------------------------------------
  // Device Lists
  //----------------------------------------------------------------

  // Description:
  // Decode command and response messages, creating an igtlioCommand if neccessary
  // Invoke a corresponding event
  void ParseCommands();

  // Description:
  // Remove commands from the deque of outgoing commands if they are not currently pending
  void PruneCompletedCommands();

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

  // Description:
  // A function to explicitly push node to OpenIGTLink. The function is called either by
  // external nodes or MRML event hander in the connector node.
  int PushNode(igtlioDevicePointer node, int event=-1);

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

  // Description:
  // Used when receiving command or command response messages.
  // Adds the commands to a queue that is parsed during periodic process
  bool ReceiveCommandMessage(igtl::MessageHeader::Pointer headerMsg);

  // Description:
  // Get the command pointer for the command with the matching id
  // Returns NULL if there is no matching command
  igtlioCommandPointer GetOutgoingCommand(int commandID);

 protected:
  //----------------------------------------------------------------
  // Devices
  //----------------------------------------------------------------
  std::vector<igtlioDevicePointer>              Devices;

  //----------------------------------------------------------------
  // Connector configuration
  //----------------------------------------------------------------
  std::string                             Name;
  int                                     UID; /// unique ID for this connector
  int                                     Type;
  int                                     State;
  int                                     Persistent;

  //----------------------------------------------------------------
  // Thread and Socket
  //----------------------------------------------------------------
  vtkMultiThreaderPointer                 Thread;
  vtkMutexLockPointer                     Mutex;
  igtl::ServerSocket::Pointer             ServerSocket;
  igtl::ClientSocket::Pointer             Socket;
  int                                     ThreadID;
  std::string                             ServerHostname;
  int                                     ServerPort;
  int                                     ServerStopFlag;

  //----------------------------------------------------------------
  // Data
  //----------------------------------------------------------------
  typedef std::map<igtlioDeviceKeyType, igtlioCircularSectionBufferPointer> igtlioCircularSectionBufferMap;
  igtlioCircularSectionBufferMap                SectionBuffer;

  vtkMutexLockPointer                     CircularBufferMutex;
  int                                     RestrictDeviceName;  // Flag to restrict incoming and outgoing data by device names

  // Event queueing mechanism is needed to send all event notifications from the main thread.
  // Events can be pushed to the end of the EventQueue by calling RequestInvoke from any thread,
  // and they will be Invoked in the main thread.
  std::list<unsigned long>                EventQueue;
  vtkMutexLockPointer                     EventQueueMutex;

  // Collect commands before they enter the circular buffer, in order to make sure that they are not overwritten
  //typedef int ClientIDType;
  struct IncomingCommandType
  {
    int ClientID;
    igtl::MessageBase::Pointer CommandMessage;
    IncomingCommandType(int clientID, igtl::MessageBase::Pointer commandMessage)
      : ClientID(clientID),
        CommandMessage(commandMessage){}
  };
  typedef std::queue<IncomingCommandType> IncomingCommandQueueType;

  IncomingCommandQueueType                IncomingCommandQueue;
  vtkMutexLockPointer                     IncomingCommandQueueMutex;

  // Flag for the push outoing message request
  // If the flag is ON, the external timer will update the outgoing nodes with
  // "OpenIGTLinkIF.pushOnConnection" attribute to push the nodes to the network.
  int                                     PushOutgoingMessageFlag;
  vtkMutexLockPointer                     PushOutgoingMessageMutex;

  igtlioDeviceFactoryPointer              DeviceFactory;

  bool                                    CheckCRC;

  igtlioCommandDequeType                  OutgoingCommandDeque;
  vtkMutexLockPointer                     OutgoingCommandDequeMutex;

  int                                     NextCommandID;

};

#endif
