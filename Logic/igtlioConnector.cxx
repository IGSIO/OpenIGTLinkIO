/*==========================================================================

Portions (c) Copyright 2018 IGSIO

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   igtlio
Module:    $RCSfile: igtlioConnector.cxx,v $
Date:      $Date: 2018/06/28 11:34:29 $
Version:   $Revision: 1.4 $

=========================================================================auto=*/

// std includes
#include <iostream>
#include <map>
#include <sstream>
#include <string>

// OpenIGTLink includes
#include <igtl_header.h>
#include <igtlClientSocket.h>
#include <igtlCommandMessage.h>
#include <igtlMessageBase.h>
#include <igtlMessageHeader.h>
#include <igtlOSUtil.h>
#include <igtlServerSocket.h>

// VTK includes
#include <vtkCommand.h>
#include <vtkCollection.h>
#include <vtkImageData.h>
#include <vtkIntArray.h>
#include <vtkMultiThreader.h>
#include <vtkMutexLock.h>
#include <vtkObjectFactory.h>
#include <vtkTimerLog.h>

// vtksys includes
#include <vtksys/SystemTools.hxx>

// OpenIGTLinkIO includes
#include "igtlioCircularBuffer.h"
#include "igtlioCircularSectionBuffer.h"
#include "igtlioCommandConverter.h"

#include "igtlioConnector.h"

//------------------------------------------------------------------------------
vtkStandardNewMacro(igtlioConnector);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
const char *igtlioConnector::ConnectorTypeStr[igtlioConnector::NUM_TYPE] =
{
  "?", // TYPE_NOT_DEFINED
  "S", // TYPE_SERVER
  "C", // TYPE_CLIENT
};

//----------------------------------------------------------------------------
const char *igtlioConnector::ConnectorStateStr[igtlioConnector::NUM_STATE] =
{
  "OFF",       // OFF
  "WAIT",      // WAIT_CONNECTION
  "ON",        // CONNECTED
};

//----------------------------------------------------------------------------
igtlioConnector::igtlioConnector()
  : Type(TYPE_CLIENT)
  , State(STATE_OFF)
  , Persistent(PERSISTENT_OFF)
  , Thread(vtkMultiThreaderPointer::New())
  , ClientMutex(vtkMutexLockPointer::New())
  , ConnectionThreadID(-1)
  , ServerHostname("localhost")
  , ServerPort(18944)
  , ServerStopFlag(false)
  , CircularBufferMutex(vtkMutexLockPointer::New())
  , RestrictDeviceName(0)
  , EventQueueMutex(vtkMutexLockPointer::New())
  , IncomingCommandQueueMutex(vtkMutexLockPointer::New())
  , OutgoingCommandDequeMutex(vtkMutexLockPointer::New())
  , PushOutgoingMessageFlag(0)
  , PushOutgoingMessageMutex(vtkMutexLockPointer::New())
  , DeviceMutex(vtkMutexLockPointer::New())
  , DeviceFactory(igtlioDeviceFactoryPointer::New())
  , CheckCRC(true)
  , NextCommandID(1)
  , NextClientID(1)
{

}

//----------------------------------------------------------------------------
igtlioConnector::~igtlioConnector()
{
  this->Stop();
}

//----------------------------------------------------------------------------
void igtlioConnector::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);

  if (this->Type == TYPE_SERVER)
    {
    os << indent << "Connector Type : SERVER\n";
    os << indent << "Listening Port #: " << this->ServerPort << "\n";
    }
  else if (this->Type == TYPE_CLIENT)
    {
    os << indent << "Connector Type: CLIENT\n";
    os << indent << "Server Hostname: " << this->ServerHostname << "\n";
    os << indent << "Server Port #: " << this->ServerPort << "\n";
    }

  switch (this->State)
    {
    case STATE_OFF:
      os << indent << "State: OFF\n";
      break;
    case STATE_WAIT_CONNECTION:
      os << indent << "State: WAIT FOR CONNECTION\n";
      break;
    case STATE_CONNECTED:
      os << indent << "State: CONNECTED\n";
      break;
    }
  os << indent << "Persistent: " << this->Persistent << "\n";
  os << indent << "Restrict Device Name: " << this->RestrictDeviceName << "\n";
  os << indent << "Push Outgoing Message Flag: " << this->PushOutgoingMessageFlag << "\n";
  os << indent << "Check CRC: " << (this->CheckCRC ? "true" : "false") << "\n";
  os << indent << "Number of devices: " << this->GetNumberOfDevices() << "\n";
  os << indent << "NextCommandID: " << this->NextCommandID << "\n";
}

//----------------------------------------------------------------------------
const char* igtlioConnector::GetServerHostname()
{
  return this->ServerHostname.c_str();
}

//----------------------------------------------------------------------------
void igtlioConnector::SetServerHostname(std::string str)
{
  if (this->ServerHostname.compare(str) == 0)
    {
    return;
    }
  this->ServerHostname = str;
  this->Modified();
}

//----------------------------------------------------------------------------
int igtlioConnector::SetTypeServer(int port)
{
  if (this->Type == TYPE_SERVER
      && this->ServerPort == port)
    {
    return 1;
    }
  this->Type = TYPE_SERVER;
  this->ServerPort = port;
  this->Modified();
  return 1;
}

//----------------------------------------------------------------------------
int igtlioConnector::SetTypeClient(std::string hostname, int port)
{
  if (this->Type == TYPE_CLIENT
      && this->ServerPort == port
      && this->ServerHostname.compare(hostname) == 0)
    {
    return 1;
    }
  this->Type = TYPE_CLIENT;
  this->ServerPort = port;
  this->ServerHostname = hostname;
  this->Modified();
  return 1;
}

//----------------------------------------------------------------------------
void igtlioConnector::SetCheckCRC(bool c)
{
  this->CheckCRC = c;
}

//---------------------------------------------------------------------------
int igtlioConnector::Start()
{
  // Check if type is defined.
  if (this->Type == igtlioConnector::TYPE_NOT_DEFINED)
    {
      //vtkErrorMacro("Connector type is not defined.");
    return 0;
    }

  if (this->ConnectionThreadID >= 0)
    {
    vtkErrorMacro("Connector is already running!");
    return 0;
    }

  this->ServerStopFlag = false;
  this->ConnectionThreadID = this->Thread->SpawnThread((vtkThreadFunctionType)&igtlioConnector::ConnectionAcceptThreadFunction, this);

  // Following line is necessary in some Linux environment,
  // since it takes for a while for the thread to update
  // this->State to non STATE_OFF value. This causes error
  // after calling vtkMRMLIGTLConnectorNode::Start() in ProcessGUIEvent()
  // in vtkOpenIGTLinkIFGUI class.
  this->State = STATE_WAIT_CONNECTION;
  this->InvokeEvent(igtlioConnector::ActivatedEvent);

  return 1;
}

//---------------------------------------------------------------------------
int igtlioConnector::Stop()
{
  // Check if thread exists
  if (this->ConnectionThreadID >= 0)
  {
    if (this->Type == TYPE_SERVER && this->ServerSocket.IsNotNull())
    {
      this->ServerSocket->CloseSocket();
    }

    // NOTE: Thread should be killed by activating ServerStopFlag.
    this->ServerStopFlag = true;
    while(this->ConnectionThreadID >= 0 || this->Sockets.size() > 0)
     {
      // Sleep required to keep loop from hanging in release mode
      igtl::Sleep(1);
     }

    return 1;
    }
  else
    {
    return 0;
    }
}

//---------------------------------------------------------------------------
void* igtlioConnector::ReceiverThreadFunction(void* ptr)
{
  vtkMultiThreader::ThreadInfo* vinfo = static_cast<vtkMultiThreader::ThreadInfo*>(ptr);
  igtlioConnector* connector = static_cast<igtlioConnector*>(vinfo->UserData);
  int currentThreadID = vinfo->ThreadID;
  int clientID = -1;
  bool connected = true;
  // Communication -- common to both Server and Client
  while (!connector->ServerStopFlag && connected)
    {
    if (clientID == -1)
      {
      std::vector<int> clientIds = connector->GetClientIds();
      for (std::vector<int>::iterator clientIdIt = clientIds.begin(); clientIdIt != clientIds.end(); ++clientIdIt)
        {
        Client client = connector->GetClient(*clientIdIt);
        if (client.ThreadID == currentThreadID)
          {
            clientID = *clientIdIt;
            break;
          }
        }
      }

    connected = connector->ReceiveController(clientID);
    }

  connector->RemoveClient(clientID);

  // Signal to the threader that this thread has become free
  vinfo->ActiveFlagLock->Lock();
  (*vinfo->ActiveFlag) = 0;
  vinfo->ActiveFlagLock->Unlock();
  return NULL; //why???
}

//----------------------------------------------------------------------------
igtlioCommandPointer igtlioConnector::GetOutgoingCommand(int commandId, int clientId)
{
  igtlioLockGuard<vtkMutexLock> lock(this->OutgoingCommandDequeMutex);
  igtlioCommandPointer command = NULL;
  if (!this->OutgoingCommandDeque.empty())
    {
    igtlioCommandDequeType::iterator outgoingCommandIt = (std::find_if(
      this->OutgoingCommandDeque.begin(),
      this->OutgoingCommandDeque.end(),
      [this, commandId, clientId](const igtlioCommandPointer& entry)
      { return entry->GetCommandId() == commandId && (entry->GetClientId() == clientId || entry->GetClientId() == -1); }));
    if (outgoingCommandIt != this->OutgoingCommandDeque.end())
      {
      command = *outgoingCommandIt;
      }
    }
  return command;
}

//----------------------------------------------------------------------------
void igtlioConnector::RequestInvokeEvent(unsigned long eventId)
{
  igtlioLockGuard<vtkMutexLock> lock(this->EventQueueMutex);
  this->EventQueue.push_back(eventId);
}

//----------------------------------------------------------------------------
void igtlioConnector::RequestPushOutgoingMessages()
{
  igtlioLockGuard<vtkMutexLock> lock(this->PushOutgoingMessageMutex);
  this->PushOutgoingMessageFlag = 1;
}

//----------------------------------------------------------------------------
void* igtlioConnector::ConnectionAcceptThreadFunction(void* ptr)
{
  vtkMultiThreader::ThreadInfo* vinfo = static_cast<vtkMultiThreader::ThreadInfo*>(ptr);
  igtlioConnector* connector = static_cast<igtlioConnector*>(vinfo->UserData);

  connector->State = STATE_WAIT_CONNECTION;
  if (connector->Type == TYPE_SERVER)
    {
    connector->ServerSocket = igtl::ServerSocket::New();
    if (connector->ServerSocket->CreateServer(connector->ServerPort) == -1)
      {
      vtkErrorWithObjectMacro(connector, "Failed to create server socket !");
      connector->ServerStopFlag = true;
      }
    }

  while (!connector->ServerStopFlag)
    {
    if (connector->Type == TYPE_SERVER)
      {
      igtl::ClientSocket::Pointer client = connector->ServerSocket->WaitForConnection(1000);
      if (client.IsNotNull()) // if client connected
        {
        igtlioLockGuard<vtkMutexLock> lock(connector->ClientMutex);
        int clientThreadID = connector->Thread->SpawnThread((vtkThreadFunctionType)&igtlioConnector::ReceiverThreadFunction, connector);
        Client clientInfo = Client(connector->NextClientID++, client, clientThreadID);
        connector->Sockets.push_back(clientInfo);
        connector->RequestInvokeEvent(ClientConnectedEvent);
        }
      }
    else if (connector->Type == TYPE_CLIENT) // if this->Type == TYPE_CLIENT
      {
      if (connector->Sockets.empty())
        {
          igtl::ClientSocket::Pointer socket = igtl::ClientSocket::New();
          int r = socket->ConnectToServer(connector->ServerHostname.c_str(), connector->ServerPort, false);
          if (r == 0) // if connected to server
          {
            igtlioLockGuard<vtkMutexLock> lock(connector->ClientMutex);
            int clientThreadID = connector->Thread->SpawnThread((vtkThreadFunctionType)&igtlioConnector::ReceiverThreadFunction, connector);
            Client clientInfo = Client(0, socket, clientThreadID);
            connector->Sockets.push_back(clientInfo);
            connector->RequestInvokeEvent(ClientConnectedEvent);
          }
        }
      }
    else
      {
      connector->ServerStopFlag = true;
      }

    if (connector->IsConnected())
      {
      if (connector->State == STATE_WAIT_CONNECTION)
        {
        connector->State = STATE_CONNECTED;
        // need to Request the InvokeEvent, because we are not on the main thread now
        connector->RequestInvokeEvent(igtlioConnector::ConnectedEvent);
        }
      }
    else if (connector->State == STATE_CONNECTED)
      {
      connector->State = STATE_WAIT_CONNECTION;
      connector->RequestInvokeEvent(igtlioConnector::DisconnectedEvent); // need to Request the InvokeEvent, because we are not on the main thread now
      }

    // Delay is required for situations where the ConnectToServer function exits without delay
    // Without it, the loop will be processed extremely quickly and cause the program to hang
    igtl::Sleep(100);
    }

  if (connector->Type == TYPE_SERVER && connector->ServerSocket.IsNotNull())
    {
    connector->ServerSocket->CloseSocket();
    }

  igtlioLockGuard<vtkMutexLock> lock(connector->ClientMutex);
  for (std::vector<Client>::iterator clientIt = connector->Sockets.begin(); clientIt != connector->Sockets.end(); ++clientIt)
    {
    if (clientIt->Socket.IsNotNull())
      {
      clientIt->Socket->CloseSocket();
      }
    }

  connector->ConnectionThreadID = -1;
  connector->State = STATE_OFF;
  connector->RequestInvokeEvent(igtlioConnector::DeactivatedEvent); // need to Request the InvokeEvent, because we are not on the main thread now

  // Signal to the threader that this thread has become free
  vinfo->ActiveFlagLock->Lock();
  (*vinfo->ActiveFlag) = 0;
  vinfo->ActiveFlagLock->Unlock();
  return 0;
}

//----------------------------------------------------------------------------
bool igtlioConnector::ReceiveController(int clientID)
{
  igtl::MessageHeader::Pointer headerMsg;
  headerMsg = igtl::MessageHeader::New();

  Client client = this->GetClient(clientID);
  if (client.ID == -1)
  {
    return false;
  }

  // check if connection is alive
  if (!client.Socket->GetConnected())
  {
    return false;
  }

  //----------------------------------------------------------------
  // Receive Header
  headerMsg->InitPack();

  vtkDebugMacro("Waiting for header of size: " << headerMsg->GetPackSize());

  // This may need to be parallelized so that other socket aren't waiting on timeouts
  int r = client.Socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());

  vtkDebugMacro("Received header of size: " << headerMsg->GetPackSize());

  if (r != headerMsg->GetPackSize())
  {
    vtkDebugMacro("ignoring header, breaking. received=" << r);
    return false;
  }

  // Deserialize the header
  headerMsg->Unpack();

  //----------------------------------------------------------------
  // Check Device Name
  // Nov 16, 2010: Currently the following code only checks
  // if the device name is defined in the message.
  const char* devName = headerMsg->GetDeviceName();
  if (devName[0] == '\0')
  {
    /// Dec 7, 2010: Removing the following code, since message without
    /// device name should be handled in the MRML scene as well.
    //// If no device name is defined, skip processing the message.
    //this->Skip(headerMsg->GetBodySizeToRead());
    //continue; //  while (!this->ServerStopFlag)
  }
  //----------------------------------------------------------------
  // If device name is restricted
  else if (this->RestrictDeviceName)
  {
    // Check if the node has already been registered.
    igtlioDeviceKeyType key = igtlioDeviceKeyType::CreateDeviceKey(headerMsg);
    int registered = this->GetDevice(key).GetPointer() != NULL;
    if (registered == 0)
    {
      this->Skip(headerMsg->GetBodySizeToRead(), client);
      return true;

    }
  }

  vtkDebugMacro("completed read header : " << headerMsg->GetDeviceName() << " body size to read: " << headerMsg->GetBodySizeToRead());

  //----------------------------------------------------------------
  // Search Circular Buffer
  igtlioDeviceKeyType key = igtlioDeviceKeyType::CreateDeviceKey(headerMsg);

  // Intercept command devices before they are added to the circular buffer, and add them to the command queue
  if (std::strcmp(headerMsg->GetDeviceType(), "COMMAND") == 0 || std::strcmp(headerMsg->GetDeviceType(), "RTS_COMMAND") == 0)
  {
    if (this->ReceiveCommandMessage(headerMsg, client))
    {
      return true;
    }
  }

  igtlioCircularSectionBufferMap::iterator iter = this->SectionBuffer.find(key);
  if (iter == this->SectionBuffer.end()) // First time to refer the device name
  {
    igtlioLockGuard<vtkMutexLock> lock(this->CircularBufferMutex);
    this->SectionBuffer[key] = igtlioCircularSectionBufferPointer::New();
    this->SectionBuffer[key]->SetPacketMode(igtlioCircularSectionBuffer::SinglePacketMode);
    if (strcmp(headerMsg->GetDeviceType(), "VIDEO") == 0)
    {
      this->SectionBuffer[key]->SetPacketMode(igtlioCircularSectionBuffer::MultiplePacketsMode);
    }
  }

  //----------------------------------------------------------------
  // Load to the circular buffer

  igtlioCircularSectionBufferPointer circBuffer = this->SectionBuffer[key];

  if (circBuffer && circBuffer->StartPush() != -1)
  {
    //std::cerr << "Pushing into the circular buffer." << std::endl;
    circBuffer->StartPush();
    igtl::MessageBase::Pointer buffer = circBuffer->GetPushBuffer();
    buffer->SetMessageHeader(headerMsg);
    buffer->AllocatePack();

    vtkDebugMacro("Waiting to receive body:  size=" << buffer->GetPackBodySize()
      << ", GetBodySizeToRead=" << buffer->GetBodySizeToRead()
      << ", GetPackSize=" << buffer->GetPackSize());
    int read = client.Socket->Receive(buffer->GetPackBodyPointer(), buffer->GetPackBodySize());
    vtkDebugMacro("Received body: " << read);
    if (read != buffer->GetPackBodySize())
    {
      if (!this->ServerStopFlag)
      {
        vtkErrorMacro("Only read " << read << " but expected to read "
          << buffer->GetPackBodySize() << "\n");
      }
      return true;
    }

    circBuffer->EndPush();
  }
  else
  {
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------
bool igtlioConnector::ReceiveCommandMessage(igtl::MessageHeader::Pointer headerMsg, Client& client)
{
  igtlioDeviceKeyType key = igtlioDeviceKeyType::CreateDeviceKey(headerMsg);

  igtl::MessageBase::Pointer buffer = igtl::MessageBase::New();
  buffer->InitBuffer();
  buffer->SetMessageHeader(headerMsg);
  buffer->AllocateBuffer();

  vtkDebugMacro("Waiting to receive body:  size=" << buffer->GetBufferBodySize()
    << ", GetBodySizeToRead=" << buffer->GetBodySizeToRead()
    << ", GetPackSize=" << buffer->GetPackSize());
  int read = client.Socket->Receive(buffer->GetBufferBodyPointer(), buffer->GetBufferBodySize());
  vtkDebugMacro("Received body: " << read);
  if (read != buffer->GetBufferBodySize())
  {
    if (!this->ServerStopFlag)
    {
      vtkErrorMacro("Only read " << read << " but expected to read " << buffer->GetBufferBodySize() << "\n");
    }
    return false;
  }

  igtlioLockGuard<vtkMutexLock> lock(this->IncomingCommandQueueMutex);
  this->IncomingCommandQueue.push(IncomingCommandType(client.ID, buffer));

  return true;
}

//----------------------------------------------------------------------------
int igtlioConnector::SendData(int size, unsigned char* data, Client& client)
{
  if (client.Socket.IsNull())
    {
    return 0;
    }

  // check if connection is alive
  if (!client.Socket->GetConnected())
    {
    return 0;
    }

  return client.Socket->Send(data, size);  // return 1 on success, otherwise 0.
}

//----------------------------------------------------------------------------
int igtlioConnector::Skip(int length, Client& client, int skipFully /* = 1 */)
{
  unsigned char dummy[256];
  int block  = 256;
  int n      = 0;
  int remain = length;

  do
    {
    if (remain < block)
      {
      block = remain;
      }

    n = client.Socket->Receive(dummy, block, skipFully);
    remain -= n;
    }
  while (remain > 0 || (skipFully && n < block));

  return (length - remain);
}

//----------------------------------------------------------------------------
std::vector<int> igtlioConnector::GetClientIds()
{
  std::vector<int> clientIds;
  igtlioLockGuard<vtkMutexLock> lock(this->ClientMutex);
  for (std::vector<Client>::iterator clientIt = this->Sockets.begin(); clientIt != this->Sockets.end(); ++clientIt)
  {
    clientIds.push_back(clientIt->ID);
  }
  return clientIds;
}

//----------------------------------------------------------------------------
igtlioConnector::Client igtlioConnector::GetClient(int clientId)
{
  igtlioLockGuard<vtkMutexLock> lock(this->ClientMutex);
  for (std::vector<Client>::iterator clientIt = this->Sockets.begin(); clientIt != this->Sockets.end(); ++clientIt)
  {
    if (clientIt->ID == clientId)
    {
      return *clientIt;
    }
  }

  return Client(-1, nullptr, -1);
}

//----------------------------------------------------------------------------
bool igtlioConnector::RemoveClient(int clientId)
{
  igtlioLockGuard<vtkMutexLock> lock(this->ClientMutex);
  std::vector<Client>::iterator clientIt = (std::find_if(
    this->Sockets.begin(),
    this->Sockets.end(),
    [this, clientId](const Client& entry)
  { return entry.ID == clientId; }));

  if (clientIt != this->Sockets.end())
  {
    clientIt->Socket->CloseSocket();
    this->Sockets.erase(clientIt);
    this->RequestInvokeEvent(ClientDisconnectedEvent);
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
unsigned int igtlioConnector::GetUpdatedSectionBuffersList(NameListType& nameList)
{
  nameList.clear();

  igtlioCircularSectionBufferMap::iterator iter;
  for (iter = this->SectionBuffer.begin(); iter != this->SectionBuffer.end(); iter ++)
    {
    if (iter->second != NULL && iter->second->IsUpdated())
      {
      nameList.push_back(iter->first);
      }
    }
  return nameList.size();
}

//----------------------------------------------------------------------------
igtlioCircularSectionBufferPointer igtlioConnector::GetCircularSectionBuffer(const igtlioDeviceKeyType &key)
{
  igtlioCircularSectionBufferMap::iterator iter = this->SectionBuffer.find(key);
  if (iter != this->SectionBuffer.end())
    {
    return this->SectionBuffer[key]; // the key has been found in the list
    }
  else
    {
    return NULL;  // nothing found
    }
}

//---------------------------------------------------------------------------
void igtlioConnector::ImportDataFromCircularBuffer()
{
  igtlioConnector::NameListType nameList;
  this->GetUpdatedSectionBuffersList(nameList);

  igtlioConnector::NameListType::iterator nameIter;
  for (nameIter = nameList.begin(); nameIter != nameList.end(); nameIter ++)
    {
    igtlioDeviceKeyType key = *nameIter;
    igtlioCircularSectionBuffer* circBuffer = this->GetCircularSectionBuffer(key);
    circBuffer->StartPull();
    igtlioDevicePointer device = NULL;
    while(circBuffer->IsSectionBufferInProcess())
    {
    igtl::MessageBase::Pointer messageFromBuffer = circBuffer->GetPullBuffer();

    igtlioDeviceCreatorPointer deviceCreator = DeviceFactory->GetCreator(key.GetBaseTypeName());

    if (!deviceCreator)
      {
      vtkErrorMacro(<< "Received unknown device type " << messageFromBuffer->GetDeviceType() << ", device=" << messageFromBuffer->GetDeviceName());
      continue;
      }

    device = this->GetDevice(key);

    if ((device.GetPointer()!=NULL) && !(igtlioDeviceKeyType::CreateDeviceKey(device)==igtlioDeviceKeyType::CreateDeviceKey(messageFromBuffer)))
        {
          vtkErrorMacro(
              << "Received an IGTL message of the wrong type, device=" << key.name
              << " has type " << device->GetDeviceType()
        << " got type " << messageFromBuffer->GetDeviceType()
                );
          continue;
        }

    if (!device && !this->RestrictDeviceName)
      {
        device = deviceCreator->Create(key.name);
        device->SetMessageDirection(igtlioDevice::MESSAGE_DIRECTION_IN);
        this->AddDevice(device);
      }

      device->InvokeEvent(igtlioDevice::AboutToReceiveEvent);
      device->ReceiveIGTLMessage(messageFromBuffer, this->CheckCRC);
      device->InvokeEvent(igtlioDevice::ReceiveEvent);
    }
    if (device)
      {
      device->Modified();
      }
    circBuffer->EndPull();
    }
}

//---------------------------------------------------------------------------
void igtlioConnector::ImportEventsFromEventBuffer()
{
  // Invoke all events in the EventQueue

  bool emptyQueue=true;
  unsigned long eventId=0;
  do
  {
    emptyQueue=true;
    igtlioLockGuard<vtkMutexLock> lock(this->EventQueueMutex);
    if (this->EventQueue.size()>0)
    {
      eventId=this->EventQueue.front();
      this->EventQueue.pop_front();
      emptyQueue=false;

      // Invoke the event
      this->InvokeEvent(eventId);
    }

  } while (!emptyQueue);
}

//---------------------------------------------------------------------------
void igtlioConnector::PushOutgoingMessages()
{
  int push = 0;

  // Read PushOutgoingMessageFlag and reset it.
  {
    igtlioLockGuard<vtkMutexLock> lock(this->PushOutgoingMessageMutex);
    push = this->PushOutgoingMessageFlag;
    this->PushOutgoingMessageFlag = 0;
  }

  if (push)
    {
      igtlioLockGuard<vtkMutexLock> lock(this->DeviceMutex);
      for (unsigned i=0; i<this->Devices.size(); ++i)
        {
          if (this->Devices[i]->MessageDirectionIsOut() && this->Devices[i]->GetPushOnConnect())
            this->PushNode(this->Devices[i]);
        }
    }
}

//----------------------------------------------------------------------------
void igtlioConnector::PeriodicProcess()
{
  this->ParseCommands();
  this->PruneCompletedCommands();

  this->ImportDataFromCircularBuffer();
  this->ImportEventsFromEventBuffer();
  this->PushOutgoingMessages();
}

//----------------------------------------------------------------------------
igtlioCommandPointer igtlioConnector::SendCommand(std::string name, std::string content, IGTLIO_SYNCHRONIZATION_TYPE synchronized, double timeout_s/*=5*/, igtl::MessageBase::MetaDataMap* metaData/*=NULL*/, int clientID/*=-1*/)
{
  igtlioCommandPointer command = igtlioCommandPointer::New();
  command->SetClientId(clientID);
  command->SetName(name);
  command->SetCommandContent(content);
  command->SetTimeoutSec(timeout_s);
  command->SetBlocking(synchronized == IGTLIO_BLOCKING);
  if (metaData)
    {
    command->SetCommandMetaData(*metaData);
    }

  int success = this->SendCommand(command);
  if (!success)
    {
    return NULL;
    }
  return command;
}

//----------------------------------------------------------------------------
int igtlioConnector::SendCommand(igtlioCommandPointer command)
{
  if (command->IsInProgress())
    {
    vtkWarningMacro("SendCommand: Command " << command->GetCommandId() << "-" << command->GetName() << " is already in progress! Attempting to cancel and resend.")
    this->CancelCommand(command);
    }

  command->SetCommandId(this->NextCommandID++);
  command->SetDirectionOut();

  igtlioBaseConverter::HeaderData headerData = igtlioBaseConverter::HeaderData();
  headerData.deviceName = "command";
  headerData.timestamp = vtkTimerLog::GetUniversalTime();

  igtlioCommandConverter::ContentData content = igtlioCommandConverter::ContentData();
  content.id = command->GetCommandId();
  content.name = command->GetName();
  content.content = command->GetCommandContent();

  igtl::CommandMessage::Pointer commandMessage = igtl::CommandMessage::New();
  igtl::MessageBase::MetaDataMap metaData = command->GetCommandMetaData();
  igtlioCommandConverter::toIGTL(headerData, content, &commandMessage, metaData);

  Client client = this->GetClient(command->GetClientId());
  if (client.ID == -1)
  {
    bool result = true;
    std::vector<int> clientIds = this->GetClientIds();
    for (std::vector<int>::iterator clientIdIt = clientIds.begin(); clientIdIt != clientIds.end(); ++clientIdIt)
    {
      Client currentClient = this->GetClient(*clientIdIt);
      if (currentClient.ID == -1)
      {
        continue;
      }

      int success = this->SendData(commandMessage->GetBufferSize(), (unsigned char*)commandMessage->GetBufferPointer(), currentClient);
      if (success)
      {
        {
          igtlioLockGuard<vtkMutexLock> lock(this->OutgoingCommandDequeMutex);
          this->OutgoingCommandDeque.push_back(command);
        }
        command->SetStatus(igtlioCommandStatus::CommandWaiting);
        command->SetSentTimestamp(vtkTimerLog::GetUniversalTime());

        while (command->GetBlocking() && !command->IsCompleted())
        {
          this->PeriodicProcess();
          vtksys::SystemTools::Delay(5);
        }
      }
      else
      {
        vtkErrorMacro("Could not send command " << command->GetCommandId() << ": \"" << command->GetName() << "\"to client");
        command->SetStatus(igtlioCommandStatus::CommandFailed);
      }
      result &= success == 1;
    }
    return result ? 1 : 0;
  }
  else
  {
    int success = this->SendData(commandMessage->GetBufferSize(), (unsigned char*)commandMessage->GetBufferPointer(), client);
    if (success)
    {
      {
        igtlioLockGuard<vtkMutexLock> lock(this->OutgoingCommandDequeMutex);
        this->OutgoingCommandDeque.push_back(command);
      }
      command->SetStatus(igtlioCommandStatus::CommandWaiting);
      command->SetSentTimestamp(vtkTimerLog::GetUniversalTime());

      while (command->GetBlocking() && !command->IsCompleted())
      {
        this->PeriodicProcess();
        vtksys::SystemTools::Delay(5);
      }
    }
    else
    {
      vtkErrorMacro("Could not send command " << command->GetCommandId() << ": \"" << command->GetName() << "\"to client");
      command->SetStatus(igtlioCommandStatus::CommandFailed);
    }
    return success;
  }
}

//----------------------------------------------------------------------------
int igtlioConnector::ConnectedClientsCount() const
{
  int total(0);
  igtlioLockGuard<vtkMutexLock> lock(this->ClientMutex);
  for (std::vector<Client>::const_iterator clientIt = this->Sockets.begin(); clientIt != this->Sockets.end(); ++clientIt)
  {
    if (clientIt->Socket->GetConnected())
    {
      total++;
    }
  }

  return total;
}

//----------------------------------------------------------------------------
int igtlioConnector::SendCommandResponse(int commandId, int clientId/*=-1*/)
{
  igtlioCommandPointer response = igtlioCommandPointer::New();
  response->SetCommandId(commandId);

  return this->SendCommandResponse(response);
}

//----------------------------------------------------------------------------
int igtlioConnector::SendCommandResponse(igtlioCommandPointer command)
{

  if (command->GetStatus() != igtlioCommandStatus::CommandWaiting)
    {
    return 0;
    }

  igtl::RTSCommandMessage::Pointer responseMessage = igtl::RTSCommandMessage::New();
  igtlioBaseConverter::HeaderData headerData = igtlioBaseConverter::HeaderData();
  igtlioCommandConverter::ContentData content = igtlioCommandConverter::ContentData();
  content.id = command->GetCommandId();
  content.name = command->GetName();
  content.content = command->GetResponseContent();

  igtl::MessageBase::MetaDataMap metaData = command->GetResponseMetaData();
  igtlioCommandConverter::toIGTLResponse(headerData, content, &responseMessage, metaData);

  Client client = GetClient(command->GetClientId());
  if (client.ID == -1)
  {
    return 0;
  }

  int success = this->SendData(responseMessage->GetBufferSize(), (unsigned char*)responseMessage->GetBufferPointer(), client);
  command->SetStatus(igtlioCommandStatus::CommandResponseSent);
  return success;
}

//----------------------------------------------------------------------------
void igtlioConnector::CancelCommand(int commandId, int clientId/*=-1*/)
{
  igtlioCommandPointer command = this->GetOutgoingCommand(commandId, clientId);
  this->CancelCommand(command);
}

//----------------------------------------------------------------------------
void igtlioConnector::CancelCommand(igtlioCommandPointer command)
{
  if (!command)
    {
    return;
    }

  if (command->IsInProgress())
    {
    command->SetStatus(igtlioCommandStatus::CommandCancelled);
    this->PruneCompletedCommands();
    this->InvokeEvent(igtlioCommand::CommandCancelledEvent, command.GetPointer());
    command->InvokeEvent(igtlioCommand::CommandCancelledEvent);
    }
}

//----------------------------------------------------------------------------
void igtlioConnector::ParseCommands()
{
  igtlioLockGuard<vtkMutexLock> lock(this->IncomingCommandQueueMutex);

  while (!this->IncomingCommandQueue.empty())
    {
    int clientID = this->IncomingCommandQueue.front().ClientID;
    igtl::MessageBase::Pointer message = this->IncomingCommandQueue.front().CommandMessage;
    this->IncomingCommandQueue.pop();

    if (message->GetMessageType() == "COMMAND")
      {
      igtlioCommandConverter::ContentData content = igtlioCommandConverter::ContentData();
      igtlioBaseConverter::HeaderData headerData = igtlioBaseConverter::HeaderData();
      igtl::MessageBase::MetaDataMap metaData = igtl::MessageBase::MetaDataMap();
      igtlioCommandConverter::fromIGTL(message, &headerData, &content, false, metaData);

      igtlioCommandPointer command = igtlioCommandPointer::New();
      command->SetClientId(clientID);
      command->SetCommandId(content.id);
      command->SetName(content.name);
      command->SetCommandContent(content.content);
      command->SetCommandMetaData(metaData);
      command->SetStatus(igtlioCommandStatus::CommandWaiting);
      command->SetDirectionIn();

      this->InvokeEvent(igtlioCommand::CommandReceivedEvent, command.GetPointer());
      }
    else if (message->GetMessageType() == "RTS_COMMAND")
      {
      igtl::RTSCommandMessage::Pointer responseMessage = static_cast<igtl::RTSCommandMessage*>(message.GetPointer());
      igtlioCommandConverter::ContentData content = igtlioCommandConverter::ContentData();
      igtlioBaseConverter::HeaderData headerData = igtlioBaseConverter::HeaderData();
      igtl::MessageBase::MetaDataMap metaData = igtl::MessageBase::MetaDataMap();
      igtlioCommandConverter::fromIGTLResponse(message, &headerData, &content, false, metaData);

      igtlioCommandPointer command = this->GetOutgoingCommand(content.id, clientID);
      if (!command)
        {
        vtkWarningMacro("Response for command ID: " << content.id << " from client " << clientID << "  matches no outgoing commands!");
        continue;
        }
      command->SetResponseContent(content.content);
      command->SetResponseMetaData(metaData);
      command->SetStatus(igtlioCommandStatus::CommandResponseReceived);

      this->InvokeEvent(igtlioCommand::CommandResponseEvent, command.GetPointer());
      command->InvokeEvent(igtlioCommand::CommandResponseEvent);
      }
    }
}

//----------------------------------------------------------------------------
void igtlioConnector::PruneCompletedCommands()
{
  igtlioCommandDequeType completedCommands = igtlioCommandDequeType();
    {
    igtlioLockGuard<vtkMutexLock> lock(this->OutgoingCommandDequeMutex);

    for (igtlioCommandDequeType::iterator outgoingCommandIt = this->OutgoingCommandDeque.begin();
         outgoingCommandIt != this->OutgoingCommandDeque.end(); ++outgoingCommandIt)
      {
      igtlioCommandPointer command = (*outgoingCommandIt);
      double currentTimestamp = vtkTimerLog::GetUniversalTime();
      double elapsedTimeSec = currentTimestamp - command->GetSentTimestamp();
      if (command->GetStatus() == CommandWaiting && elapsedTimeSec > command->GetTimeoutSec())
        {
        completedCommands.push_back(command);
        command->SetStatus(CommandExpired);
        this->InvokeEvent(igtlioCommand::CommandExpiredEvent, command.GetPointer());
        command->InvokeEvent(igtlioCommand::CommandExpiredEvent);
        }
      else if (command->GetStatus() == CommandResponseReceived || command->GetStatus() == CommandCancelled)
        {
        completedCommands.push_back(command);
        }
      }

    // Remove completed commands from the deque
    for (igtlioCommandDequeType::iterator completedCommandIt = completedCommands.begin();
       completedCommandIt != completedCommands.end(); ++completedCommandIt)
      {
      igtlioCommandPointer command = (*completedCommandIt);
      this->OutgoingCommandDeque.erase(
        std::remove(this->OutgoingCommandDeque.begin(),
                    this->OutgoingCommandDeque.end(),
                    command),
        this->OutgoingCommandDeque.end());
      }
    }

  // Invoke igtlioCommand::CommandCompletedEvent on all pruned commands
  for (igtlioCommandDequeType::iterator completedCommandIt = completedCommands.begin();
       completedCommandIt != completedCommands.end(); ++completedCommandIt)
    {
    igtlioCommandPointer command = (*completedCommandIt);
    this->InvokeEvent(igtlioCommand::CommandCompletedEvent, command.GetPointer());
    command->InvokeEvent(igtlioCommand::CommandCompletedEvent);
    }
}

//----------------------------------------------------------------------------
igtlioDevicePointer igtlioConnector::AddDeviceIfNotPresent(igtlioDeviceKeyType key)
{
  igtlioDevicePointer device = GetDevice(key);

  if (!device)
  {
    device = GetDeviceFactory()->create(key.type, key.name);
    this->AddDevice(device);
  }

  return device;
}

//----------------------------------------------------------------------------
int igtlioConnector::AddDevice(igtlioDevicePointer device)
{
  if (this->GetDevice(igtlioDeviceKeyType::CreateDeviceKey(device))!=NULL)
    {
    vtkErrorMacro("Failed to add igtl device: " << device->GetDeviceName() << " already present");
    return 0;
    }

  device->SetTimestamp(vtkTimerLog::GetUniversalTime());
  {
  igtlioLockGuard<vtkMutexLock> lock(this->DeviceMutex);
  this->Devices.push_back(device);
  }
  
  //TODO: listen to device events?
  unsigned int deviceEvent = device->GetDeviceContentModifiedEvent();
  device->AddObserver((unsigned long)deviceEvent, this, &igtlioConnector::DeviceContentModified);
  this->InvokeEvent(igtlioConnector::NewDeviceEvent, device.GetPointer());
  return 1;
}

//----------------------------------------------------------------------------
void igtlioConnector::DeviceContentModified(vtkObject *caller, unsigned long event, void *callData )
{
  igtlioDevice* modifiedDevice = reinterpret_cast<igtlioDevice*>(callData);
  if (modifiedDevice)
    {
      this->InvokeEvent(igtlioConnector::DeviceContentModifiedEvent, modifiedDevice);
    }
}

//----------------------------------------------------------------------------
int igtlioConnector::RemoveDevice(igtlioDevicePointer device)
{
  igtlioLockGuard<vtkMutexLock> lock(this->DeviceMutex);
  igtlioDeviceKeyType key = igtlioDeviceKeyType::CreateDeviceKey(device);
  for (unsigned i=0; i< this->Devices.size(); ++i)
  {
    if (igtlioDeviceKeyType::CreateDeviceKey(this->Devices[i])==key)
    {
      this->Devices.erase(this->Devices.begin()+i);
      this->InvokeEvent(igtlioConnector::RemovedDeviceEvent, device.GetPointer());
      return 1;
    }
  }
  vtkErrorMacro("Failed to remove igtl device: " << device->GetDeviceName());
  return 0;
}

//---------------------------------------------------------------------------
unsigned int igtlioConnector::GetNumberOfDevices() const
{
  return this->Devices.size();
}

//---------------------------------------------------------------------------
void igtlioConnector::RemoveDevice(int index)
{
  igtlioLockGuard<vtkMutexLock> lock(this->DeviceMutex);
  //TODO: disconnect listen to device events?
  igtlioDevicePointer device = this->Devices[index]; // ensure object lives until event has completed
  this->Devices.erase(this->Devices.begin()+index);
  this->InvokeEvent(igtlioConnector::RemovedDeviceEvent, device.GetPointer());
}

//---------------------------------------------------------------------------
igtlioDevicePointer igtlioConnector::GetDevice(int index)
{
  igtlioLockGuard<vtkMutexLock> lock(this->DeviceMutex);
  return this->Devices[index];
}

//---------------------------------------------------------------------------
igtlioDevicePointer igtlioConnector::GetDevice(igtlioDeviceKeyType key)
{
  igtlioLockGuard<vtkMutexLock> lock(this->DeviceMutex);
  for (unsigned i=0; i< this->Devices.size(); ++i)
    if (igtlioDeviceKeyType::CreateDeviceKey(this->Devices[i])==key)
      return this->Devices[i];
  return igtlioDevicePointer();
}

//---------------------------------------------------------------------------
bool igtlioConnector::HasDevice(igtlioDevicePointer d )
{
  igtlioLockGuard<vtkMutexLock> lock(this->DeviceMutex);
    for(unsigned i=0; i< this->Devices.size(); ++i)
        if( this->Devices[i] == d )
            return true;
    return false;
}

//---------------------------------------------------------------------------
int igtlioConnector::SendMessage(igtlioDeviceKeyType device_id, igtlioDevice::MESSAGE_PREFIX prefix/*=Device::MESSAGE_PREFIX_NOT_DEFINED*/, int clientId)
{
  igtlioDevicePointer device = this->GetDevice(device_id);
  if (!device)
    {
      vtkErrorMacro("Sending OpenIGTLinkMessage: " << device_id.type << "/" << device_id.name<< ", device not found");
      return 0;
    }

  //TODO replace prefix with message-type or similar - giving the basic message same status as the queries
  igtl::MessageBase::Pointer msg = device->GetIGTLMessage(prefix);

  if (!msg)
    {
      vtkErrorMacro("Sending OpenIGTLinkMessage: " << device_id.type << "/" << device_id.name << ", message not available from device");
      return 0;
    }

  Client client = this->GetClient(clientId);
  if (client.ID == -1)
  {
    bool result = true;
    std::vector<int> clientIds = this->GetClientIds();
    for (std::vector<int>::iterator clientIdIt = clientIds.begin(); clientIdIt != clientIds.end(); ++clientIdIt)
    {
      Client currentClient = this->GetClient(*clientIdIt);
      if (currentClient.ID == -1)
      {
        continue;
      }

      int r = this->SendData(msg->GetPackSize(), (unsigned char*)msg->GetPackPointer(), currentClient);
      if (r == 0)
      {
        vtkDebugMacro("Sending OpenIGTLinkMessage: " << device_id.type << "/" << device_id.name << " failed.");
        result &= false;
      }
      else
      {
        result &= true;
      }
    }
    return result ? 1 : 0;
  }
  else
  {
    int r = this->SendData(msg->GetPackSize(), (unsigned char*)msg->GetPackPointer(), client);
    if (r == 0)
    {
      vtkDebugMacro("Sending OpenIGTLinkMessage: " << device_id.type << "/" << device_id.name << " failed.");
      return 0;
    }
    return r;
  }
}

//----------------------------------------------------------------------------
igtlioDeviceFactoryPointer igtlioConnector::GetDeviceFactory()
{
  return DeviceFactory;
}

//----------------------------------------------------------------------------
void igtlioConnector::SetDeviceFactory(igtlioDeviceFactoryPointer val)
{
  if (val==DeviceFactory)
    return;
  DeviceFactory = val;
  this->Modified();
}

//----------------------------------------------------------------------------
bool igtlioConnector::IsConnected()
{
  igtlioLockGuard<vtkMutexLock> lock(this->ClientMutex);
  for (std::vector<Client>::iterator clientIt = this->Sockets.begin(); clientIt != this->Sockets.end(); ++clientIt)
    {
    if (clientIt->Socket.IsNotNull())
      {
      if (clientIt->Socket->GetConnected())
        {
        return true;
        }
      }
    }

  return false;
}

//---------------------------------------------------------------------------
int igtlioConnector::PushNode(igtlioDevicePointer node, int event, int clientId /*=-1*/)
{
  // TODO: verify that removed event argument is OK
  // TODO: clientId doesn't make sense (whole function doesn't make sense), remove as parameter?
  bool result = true;
  std::vector<int> clientIds = this->GetClientIds();
  for (std::vector<int>::iterator clientIdIt = clientIds.begin(); clientIdIt != clientIds.end(); ++clientIdIt)
  {
    result &= (this->SendMessage(igtlioDeviceKeyType::CreateDeviceKey(node), igtlioDevice::MESSAGE_PREFIX_NOT_DEFINED, *clientIdIt) == 1);
  }
  return result ? 1 : 0;
}
