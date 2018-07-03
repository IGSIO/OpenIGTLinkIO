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
#include <string>
#include <iostream>
#include <sstream>
#include <map>

// OpenIGTLink includes
#include <igtl_header.h>
#include <igtlServerSocket.h>
#include <igtlClientSocket.h>
#include <igtlOSUtil.h>
#include <igtlMessageBase.h>
#include <igtlMessageHeader.h>
#include <igtlCommandMessage.h>

// VTK includes
#include <vtkCommand.h>
#include <vtkCollection.h>
#include <vtkImageData.h>
#include <vtkIntArray.h>
#include <vtkMultiThreader.h>
#include <vtkMutexLock.h>
#include <vtkObjectFactory.h>
#include <vtkTimerLog.h>
#include <vtksys/SystemTools.hxx>

// OpenIGTLinkIO includes
#include "igtlioConnector.h"
#include "igtlioCircularBuffer.h"
#include "igtlioCircularSectionBuffer.h"
#include "igtlioCommandConverter.h"

//------------------------------------------------------------------------------
vtkStandardNewMacro(igtlioConnector);
//----------------------------------------------------------------------------

int SOCKET_TIMEOUT_MILLISECONDS = 1;

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
  , Mutex(vtkMutexLockPointer::New())
  , ThreadID(-1)
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

  // Check if thread is detached
  if (this->ThreadID >= 0)
    {
      //vtkErrorMacro("Thread exists.");
    return 0;
    }

  this->ServerStopFlag = false;
  this->ThreadID = this->Thread->SpawnThread((vtkThreadFunctionType) &igtlioConnector::ThreadFunction, this);

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
  if (this->ThreadID >= 0)
  {
    // NOTE: Thread should be killed by activating ServerStopFlag.
    this->ServerStopFlag = true;
    {
      igtlioLockGuard<vtkMutexLock> lock(this->Mutex);
      for (std::vector<Client>::iterator clientIt = this->Sockets.begin(); clientIt != this->Sockets.end(); ++clientIt)
        {
        if (clientIt->Socket.IsNotNull())
          {
          clientIt->Socket->CloseSocket();
          }
        }
      this->Sockets.clear();
    }
    this->Thread->TerminateThread(this->ThreadID);
    this->ThreadID = -1;
    return 1;
    }
  else
    {
    return 0;
    }
}

//---------------------------------------------------------------------------
void* igtlioConnector::ThreadFunction(void* ptr)
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

  // Communication -- common to both Server and Client
  while (!connector->ServerStopFlag)
    {

    {
      igtlioLockGuard<vtkMutexLock> lock(connector->Mutex);
      connector->WaitForConnection();
    }

    if (connector->IsConnected())
      {
      connector->State = STATE_CONNECTED;
      // need to Request the InvokeEvent, because we are not on the main thread now
      connector->RequestInvokeEvent(igtlioConnector::ConnectedEvent);
      connector->RequestPushOutgoingMessages();
      connector->ReceiveController();
      connector->State = STATE_WAIT_CONNECTION;
      connector->RequestInvokeEvent(igtlioConnector::DisconnectedEvent); // need to Request the InvokeEvent, because we are not on the main thread now
      }
    }

  igtlioLockGuard<vtkMutexLock> lock(connector->Mutex);
  for (std::vector<Client>::iterator clientIt = connector->Sockets.begin(); clientIt != connector->Sockets.end(); ++clientIt)
    {
    if (clientIt->Socket.IsNotNull())
      {
      clientIt->Socket->CloseSocket();
      }
    }
  connector->Sockets.clear();

  if (connector->Type == TYPE_SERVER && connector->ServerSocket.IsNotNull())
    {
    connector->ServerSocket->CloseSocket();
    }

  connector->ThreadID = -1;
  connector->State = STATE_OFF;
  connector->RequestInvokeEvent(igtlioConnector::DeactivatedEvent); // need to Request the InvokeEvent, because we are not on the main thread now

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
int igtlioConnector::WaitForConnection()
{
  if (this->Type == TYPE_CLIENT)
    {
    igtlioLockGuard<vtkMutexLock> lock(this->Mutex);
    this->Sockets.push_back(Client(0, igtl::ClientSocket::New()));
    }

  while (!this->ServerStopFlag)
    {
    if (this->Type == TYPE_SERVER)
      {
      igtl::ClientSocket::Pointer client = this->ServerSocket->WaitForConnection(1000);
      if (client.IsNotNull()) // if client connected
        {
        igtlioLockGuard<vtkMutexLock> lock(this->Mutex);
        client->SetTimeout(SOCKET_TIMEOUT_MILLISECONDS);
        this->Sockets.push_back(Client(this->NextClientID++, client));
        return 1;
        }
      }
    else if (this->Type == TYPE_CLIENT) // if this->Type == TYPE_CLIENT
      {
      igtlioLockGuard<vtkMutexLock> lock(this->Mutex);
      assert(this->Sockets.size() > 0);
      int r = this->Sockets[0].Socket->ConnectToServer(this->ServerHostname.c_str(), this->ServerPort);
      if (r == 0) // if connected to server
        {
        this->Sockets[0].Socket->SetTimeout(SOCKET_TIMEOUT_MILLISECONDS);
        return 1;
        }
      else
        {
        igtl::Sleep(100);
        break;
        }
      }
    else
      {
      this->ServerStopFlag = true;
      }
    }

  igtlioLockGuard<vtkMutexLock> lock(this->Mutex);
  for (std::vector<Client>::iterator clientIt = this->Sockets.begin(); clientIt != this->Sockets.end(); ++clientIt)
    {
    if (clientIt->Socket.IsNotNull())
      {
      clientIt->Socket->CloseSocket();
      }
    }
  this->Sockets.clear();

  return 0;
}

//----------------------------------------------------------------------------
int igtlioConnector::ReceiveController()
{
  igtl::MessageHeader::Pointer headerMsg;
  headerMsg = igtl::MessageHeader::New();

  while (!this->ServerStopFlag)
  {
  igtlioLockGuard<vtkMutexLock> lock(this->Mutex);
  for (std::vector<Client>::iterator clientIt = this->Sockets.begin(); clientIt != this->Sockets.end(); ++clientIt)
    {
      // check if connection is alive
      if (!clientIt->Socket->GetConnected())
      {
        continue;
      }

      //----------------------------------------------------------------
      // Receive Header
      headerMsg->InitPack();

      vtkDebugMacro("Waiting for header of size: " << headerMsg->GetPackSize());

      // This may need to be parallelized so that other socket aren't waiting on timeouts
      int r = clientIt->Socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());

      vtkDebugMacro("Received header of size: " << headerMsg->GetPackSize());

      if (r != headerMsg->GetPackSize())
      {
        vtkDebugMacro("ignoring header, breaking. received=" << r);
        break;
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
          //TODO: Cannot call GetDevice in Thread!!!!
        igtlioDeviceKeyType key = igtlioDeviceKeyType::CreateDeviceKey(headerMsg);
        int registered = this->GetDevice(key).GetPointer() != NULL;
        if (registered == 0)
        {
          this->Skip(headerMsg->GetBodySizeToRead(), *clientIt);
          continue; //  while (!this->ServerStopFlag)
        }
      }

      vtkDebugMacro("completed read header : " << headerMsg->GetDeviceName() << " body size to read: " << headerMsg->GetBodySizeToRead());

      //----------------------------------------------------------------
      // Search Circular Buffer
      igtlioDeviceKeyType key = igtlioDeviceKeyType::CreateDeviceKey(headerMsg);

      // Intercept command devices before they are added to the circular buffer, and add them to the command queue
      if (std::strcmp(headerMsg->GetDeviceType(), "COMMAND") == 0 || std::strcmp(headerMsg->GetDeviceType(), "RTS_COMMAND") == 0)
      {
        if (this->ReceiveCommandMessage(headerMsg, *clientIt))
        {
          continue;
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
        int read = clientIt->Socket->Receive(buffer->GetPackBodyPointer(), buffer->GetPackBodySize());
        vtkDebugMacro("Received body: " << read);
        if (read != buffer->GetPackBodySize())
        {
          if (!this->ServerStopFlag)
          {
            vtkErrorMacro("Only read " << read << " but expected to read "
              << buffer->GetPackBodySize() << "\n");
          }
          continue;
        }

        circBuffer->EndPush();

      }
      else
      {
        break;
      }
    }
  } // while (!this->ServerStopFlag)

  igtlioLockGuard<vtkMutexLock> lock(this->Mutex);
  for (std::vector<Client>::iterator clientIt = this->Sockets.begin(); clientIt != this->Sockets.end(); ++clientIt)
  {
    if (clientIt->Socket.IsNotNull())
    {
      clientIt->Socket->CloseSocket();
    }
  }
  this->Sockets.clear();

  return 0;
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
igtlioConnector::Client igtlioConnector::GetClient(int clientId)
{
  igtlioLockGuard<vtkMutexLock> lock(this->Mutex);
  for (std::vector<Client>::iterator clientIt = this->Sockets.begin(); clientIt != this->Sockets.end(); ++clientIt)
  {
    if (clientIt->ID == clientId)
    {
      return *clientIt;
    }
  }

  return Client(-1, nullptr);
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

      device->ReceiveIGTLMessage(messageFromBuffer, this->CheckCRC);
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
      for (unsigned i=0; i<Devices.size(); ++i)
        {
          if (Devices[i]->MessageDirectionIsOut() && Devices[i]->GetPushOnConnect())
            this->PushNode(Devices[i]);
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
int igtlioConnector::SendCommand(igtlioCommandPointer command, int clientId)
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

  Client client = GetClient(clientId);
  if (client.ID == -1)
  {
    bool result = true;
    igtlioLockGuard<vtkMutexLock> lock(this->Mutex);
    for (std::vector<Client>::iterator clientIt = this->Sockets.begin(); clientIt != this->Sockets.end(); ++clientIt)
    {
      int success = this->SendData(commandMessage->GetBufferSize(), (unsigned char*)commandMessage->GetBufferPointer(), *clientIt);
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
  igtlioLockGuard<vtkMutexLock> lock(this->Mutex);
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
        vtkWarningMacro("Response matches no outgoing commands!");
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
  igtlioLockGuard<vtkMutexLock> lock(this->OutgoingCommandDequeMutex);

  igtlioCommandDequeType completedCommands = igtlioCommandDequeType();
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

  // Invoke igtlioCommand::CommandCompletedEvent on all pruned commands and remove them from the deque
  for (igtlioCommandDequeType::iterator completedCommandIt = completedCommands.begin();
       completedCommandIt != completedCommands.end(); ++completedCommandIt)
    {
    igtlioCommandPointer command = (*completedCommandIt);
    this->InvokeEvent(igtlioCommand::CommandCompletedEvent, command.GetPointer());
    command->InvokeEvent(igtlioCommand::CommandCompletedEvent);
    this->OutgoingCommandDeque.erase(std::remove(this->OutgoingCommandDeque.begin(), this->OutgoingCommandDeque.end(), command), this->OutgoingCommandDeque.end());
    }
}

//----------------------------------------------------------------------------
igtlioDevicePointer igtlioConnector::AddDeviceIfNotPresent(igtlioDeviceKeyType key)
{
  igtlioDevicePointer device = GetDevice(key);

  if (!device)
  {
    device = GetDeviceFactory()->create(key.type, key.name);
    AddDevice(device);
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
  Devices.push_back(device);
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
  igtlioDeviceKeyType key = igtlioDeviceKeyType::CreateDeviceKey(device);
  for (unsigned i=0; i<Devices.size(); ++i)
  {
    if (igtlioDeviceKeyType::CreateDeviceKey(Devices[i])==key)
    {
      Devices.erase(Devices.begin()+i);
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
  return Devices.size();
}

//---------------------------------------------------------------------------
void igtlioConnector::RemoveDevice(int index)
{
  //TODO: disconnect listen to device events?
  igtlioDevicePointer device = Devices[index]; // ensure object lives until event has completed
  Devices.erase(Devices.begin()+index);
  this->InvokeEvent(igtlioConnector::RemovedDeviceEvent, device.GetPointer());
}

//---------------------------------------------------------------------------
igtlioDevicePointer igtlioConnector::GetDevice(int index)
{
  return Devices[index];
}

//---------------------------------------------------------------------------
igtlioDevicePointer igtlioConnector::GetDevice(igtlioDeviceKeyType key)
{
  for (unsigned i=0; i<Devices.size(); ++i)
    if (igtlioDeviceKeyType::CreateDeviceKey(Devices[i])==key)
      return Devices[i];
  return igtlioDevicePointer();
}

//---------------------------------------------------------------------------
bool igtlioConnector::HasDevice(igtlioDevicePointer d )
{
    for(unsigned i=0; i<Devices.size(); ++i)
        if( Devices[i] == d )
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
    igtlioLockGuard<vtkMutexLock> lock(this->Mutex);
    for (std::vector<Client>::iterator clientIt = this->Sockets.begin(); clientIt != this->Sockets.end(); ++clientIt)
    {
      int r = this->SendData(msg->GetPackSize(), (unsigned char*)msg->GetPackPointer(), *clientIt);
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
  igtlioLockGuard<vtkMutexLock> lock(this->Mutex);
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
  igtlioLockGuard<vtkMutexLock> lock(this->Mutex);
  for (std::vector<Client>::iterator clientIt = this->Sockets.begin(); clientIt != this->Sockets.end(); ++clientIt)
  {
    result &= (this->SendMessage(igtlioDeviceKeyType::CreateDeviceKey(node), igtlioDevice::MESSAGE_PREFIX_NOT_DEFINED, clientId) == 1);
  }
  return result ? 1 : 0;
}
