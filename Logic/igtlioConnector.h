/*=auto=========================================================================

  Portions (c) Copyright 2018 IGSIO

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   igtlio
  Module:    $RCSfile: igtlioConnector.h,v $
  Date:      $Date: 2018/06/28 11:34:29 $
  Version:   $Revision: 1.4 $

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


// The macro SendMessage in winuser.h clashes with the method name in igtlioConnector.
// A simple workaround is to undefine this macro to avoid name conflict (https://stackoverflow.com/a/69041270)
#pragma push_macro("SendMessage")
#undef SendMessage

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
  /// Call periodically to perform processing in the main thread.
  /// Suggested timeout 5ms.
  void PeriodicProcess();

  // ClientId of -1 means to all connected client
  int SendCommand(igtlioCommandPointer command);
  igtlioCommandPointer SendCommand(std::string command, std::string content, IGTLIO_SYNCHRONIZATION_TYPE synchronized, double timeout_s = 5.0, igtl::MessageBase::MetaDataMap* metaData = NULL, int clientID = -1);

  int ConnectedClientsCount() const;

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
  /// ClientId of -1 means to all connected client
  int SendMessage(igtlioDeviceKeyType device_id, igtlioDevice::MESSAGE_PREFIX=igtlioDevice::MESSAGE_PREFIX_NOT_DEFINED, int clientId=-1);

  igtlioDeviceFactoryPointer GetDeviceFactory();
  void SetDeviceFactory(igtlioDeviceFactoryPointer val);

public:
  // Events
  enum {
    ConnectedEvent              = 118944,
    DisconnectedEvent           = 118945,
    ActivatedEvent              = 118946,
    DeactivatedEvent            = 118947,
    NewDeviceEvent              = 118949,
    DeviceContentModifiedEvent  = 118950, // invoked by the devices
    RemovedDeviceEvent          = 118951,
    ClientConnectedEvent        = 118952,
    ClientDisconnectedEvent     = 118953,
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

  struct Client
  {
    int                           ID;
    igtl::ClientSocket::Pointer   Socket;
    int ThreadID;
    Client(int id, igtl::ClientSocket::Pointer socket, int threadID)
      : ID(id)
      , Socket(socket)
      , ThreadID(threadID)
    {}
  };

public:
  struct SectionBufferKey
  {
    igtlioDeviceKeyType Key;
    int ClientID;
    SectionBufferKey(igtlioDeviceKeyType key, int clientID)
    {
      this->Key = key;
      this->ClientID = clientID;
    };
  };

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

  std::vector<int> GetClientIds(); // Thread safe

  //----------------------------------------------------------------
  // Thread Control
  //----------------------------------------------------------------

  int Start();
  int Stop();

private:
  static void* ConnectionAcceptThreadFunction(void* ptr);
  static void* ReceiverThreadFunction(void* ptr);

  //----------------------------------------------------------------
  // OpenIGTLink Message handlers
  //----------------------------------------------------------------
  bool ReceiveController(int clientID); // called from Thread
  int SendData(igtlUint64 size, unsigned char* data, Client& client);
  int Skip(igtlUint64 length, Client& client, int skipFully=1);

  //----------------------------------------------------------------
  // Clients
  //----------------------------------------------------------------
  Client GetClient(int clientId); // Thread safe
  bool RemoveClient(int clientId); // Thread safe

  //----------------------------------------------------------------
  // Circular Buffer
  //----------------------------------------------------------------
  typedef std::vector<SectionBufferKey> NameListType;
  unsigned int GetUpdatedSectionBuffersList(NameListType& nameList); // TODO: this will be moved to private
  igtlioCircularSectionBufferPointer GetCircularSectionBuffer(const igtlioDeviceKeyType& key, const int clientID);     // TODO: Is it OK to use device name as a key?

  //----------------------------------------------------------------
  // Device Lists
  //----------------------------------------------------------------

  // Description:
  // Decode command and response messages, creating an igtlioCommand if necessary
  // Invoke a corresponding event
  void ParseCommands();

  // Description:
  // Remove commands from the deque of outgoing commands if they are not currently pending
  void PruneCompletedCommands();

  // Description:
  // Import received data from the circular buffer to the MRML scene.
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
  int PushNode(igtlioDevicePointer node, int event=-1, int clientId=-1);

protected:
  // Description:
  // Inserts the eventId to the EventQueue, and the event will be invoked from the main thread
  void RequestInvokeEvent(unsigned long eventId); // might be called from Thread

  // Description:
  // Request to push all outgoing messages to the network stream, if permitted.
  // This function is used, when the connection is established. To permit the OpenIGTLink IF
  // to push individual "outgoing" MRML nodes, set "OpenIGTLinkIF.pushOnConnection" attribute to 1.
  // The request will be processed in PushOutgonigMessages().
  void RequestPushOutgoingMessages(); // called from Thread

  // Description:
  // Used when receiving command or command response messages.
  // Adds the commands to a queue that is parsed during periodic process
  bool ReceiveCommandMessage(igtl::MessageHeader::Pointer headerMsg, Client& client);

  // Description:
  // Get the command pointer for the command with the matching id
  // Returns NULL if there is no matching command
  igtlioCommandPointer GetOutgoingCommand(int commandID, int clientID);

 protected:
  //----------------------------------------------------------------
  // Devices
  //----------------------------------------------------------------
  std::vector<igtlioDevicePointer>          Devices;
  std::recursive_mutex                      DeviceMutex;

  //----------------------------------------------------------------
  // Connector configuration
  //----------------------------------------------------------------
  std::string                               Name;
  int                                       UID; /// unique ID for this connector
  int                                       Type;
  int                                       State;
  int                                       Persistent;

  //----------------------------------------------------------------
  // Thread and Socket
  //----------------------------------------------------------------
  vtkMultiThreaderPointer                   Thread;
  mutable std::recursive_mutex              ClientMutex;
  igtl::ServerSocket::Pointer               ServerSocket;
  std::vector<Client>                       Sockets; // Access is not thread safe, control usage with igtlioConnector::ClientMutex
  unsigned int                              NextClientID;
  int                                       ConnectionThreadID;
  std::string                               ServerHostname;
  int                                       ServerPort;
  int                                       ServerStopFlag;

  //----------------------------------------------------------------
  // Data
  //----------------------------------------------------------------
  typedef std::map<SectionBufferKey, igtlioCircularSectionBufferPointer> igtlioCircularSectionBufferMap;
  igtlioCircularSectionBufferMap            SectionBuffer;

  std::recursive_mutex                      CircularBufferMutex;
  int                                       RestrictDeviceName;  // Flag to restrict incoming and outgoing data by device names

  // Event queueing mechanism is needed to send all event notifications from the main thread.
  // Events can be pushed to the end of the EventQueue by calling RequestInvoke from any thread,
  // and they will be Invoked in the main thread.
  std::list<unsigned long>                  EventQueue;
  std::recursive_mutex                      EventQueueMutex;

  // Collect commands before they enter the circular buffer, in order to make sure that they are not overwritten
  //typedef int ClientIDType;
  struct IncomingCommandType
  {
    unsigned int                ClientID;
    igtl::MessageBase::Pointer  CommandMessage;
    IncomingCommandType(int clientID, igtl::MessageBase::Pointer commandMessage)
      : ClientID(clientID),
        CommandMessage(commandMessage){}
  };
  typedef std::queue<IncomingCommandType>   IncomingCommandQueueType;

  IncomingCommandQueueType                  IncomingCommandQueue;
  std::recursive_mutex                      IncomingCommandQueueMutex;

  // Flag for the push outgoing message request
  // If the flag is ON, the external timer will update the outgoing nodes with
  // "OpenIGTLinkIF.pushOnConnection" attribute to push the nodes to the network.
  int                                       PushOutgoingMessageFlag;
  std::recursive_mutex                      PushOutgoingMessageMutex;

  igtlioDeviceFactoryPointer                DeviceFactory;

  bool                                      CheckCRC;

  igtlioCommandDequeType                    OutgoingCommandDeque;
  std::recursive_mutex                      OutgoingCommandDequeMutex;

  int                                       NextCommandID;

};

bool operator<(const igtlioConnector::SectionBufferKey& lhs, const igtlioConnector::SectionBufferKey& rhs);

#pragma pop_macro("SendMessage")
#endif
