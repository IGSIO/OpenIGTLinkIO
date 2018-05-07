/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer4/trunk/Modules/OpenIGTLinkIF/vtkSlicerOpenIGTLinkIFLogic.cxx $
  Date:      $Date: 2011-06-12 14:55:20 -0400 (Sun, 12 Jun 2011) $
  Version:   $Revision: 16985 $

==========================================================================*/

#include <algorithm>
// IGTLIO includes
#include "igtlioLogic.h"
#include "igtlioConnector.h"
#include "igtlioSession.h"

#include <vtkObjectFactory.h>

// OpenIGTLinkIF Logic includes
#include <vtkNew.h>
#include <vtkCallbackCommand.h>
#include <vtkImageData.h>
#include <vtkTransform.h>

//---------------------------------------------------------------------------
void onNewDeviceEventFunc(vtkObject* caller, unsigned long eid, void* clientdata, void *calldata)
{
  igtlioLogic* logic = reinterpret_cast<igtlioLogic*>(clientdata);
  logic->InvokeEvent(igtlioLogic::NewDeviceEvent, calldata);
}

//---------------------------------------------------------------------------
void onRemovedDeviceEventFunc(vtkObject* caller, unsigned long eid, void* clientdata, void *calldata)
{
  igtlioLogic* logic = reinterpret_cast<igtlioLogic*>(clientdata);
  logic->InvokeEvent(igtlioLogic::RemovedDeviceEvent, calldata);

  igtlioDevice* device = reinterpret_cast<igtlioDevice*>(calldata);
  device->RemoveObserver(logic->DeviceEventCallback);
}

//---------------------------------------------------------------------------
void onDeviceEventFunc(vtkObject* caller, unsigned long eid, void* clientdata, void *calldata)
{
  igtlioLogic* logic = reinterpret_cast<igtlioLogic*>(clientdata);

  if (eid==igtlioConnector::DeviceContentModifiedEvent)
  {
    logic->InvokeEvent(eid, calldata);
  }
}

//---------------------------------------------------------------------------
void onCommandEventFunc(vtkObject* caller, unsigned long eid, void* clientdata, void *calldata)
{
  igtlioLogic* logic = reinterpret_cast<igtlioLogic*>(clientdata);

  if ((eid == igtlioCommand::CommandResponseEvent)  ||
      (eid == igtlioCommand::CommandExpiredEvent)   ||
      (eid == igtlioCommand::CommandReceivedEvent)  ||
      (eid == igtlioCommand::CommandCancelledEvent) ||
      (eid == igtlioCommand::CommandCompletedEvent))
  {
    logic->InvokeEvent(eid, calldata);
  }
}

//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioLogic);


//---------------------------------------------------------------------------
igtlioLogic::igtlioLogic()
  : vtkObject()
  , NewDeviceCallback(vtkSmartPointer<vtkCallbackCommand>::New())
  , RemovedDeviceCallback(vtkSmartPointer<vtkCallbackCommand>::New())
  , DeviceEventCallback(vtkSmartPointer<vtkCallbackCommand>::New())
  , CommandEventCallback(vtkSmartPointer<vtkCallbackCommand>::New())
{
  this->NewDeviceCallback->SetCallback(onNewDeviceEventFunc);
  this->NewDeviceCallback->SetClientData(this);

  this->RemovedDeviceCallback->SetCallback(onRemovedDeviceEventFunc);
  this->RemovedDeviceCallback->SetClientData(this);

  this->DeviceEventCallback->SetCallback(onDeviceEventFunc);
  this->DeviceEventCallback->SetClientData(this);

  this->CommandEventCallback->SetCallback(onCommandEventFunc);
  this->CommandEventCallback->SetClientData(this);
}

//---------------------------------------------------------------------------
igtlioLogic::~igtlioLogic()
{
}

//---------------------------------------------------------------------------
void igtlioLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << "vtkIGTLIOLogic:             " << this->GetClassName() << "\n";
}


//---------------------------------------------------------------------------
igtlioConnectorPointer igtlioLogic::CreateConnector()
{
  igtlioConnectorPointer connector = igtlioConnectorPointer::New();
  connector->SetUID(this->CreateUniqueConnectorID());
  std::stringstream ss;
  ss << "IGTLConnector_" << connector->GetUID();
  connector->SetName(ss.str());
  Connectors.push_back(connector);

  connector->AddObserver(igtlioConnector::NewDeviceEvent, NewDeviceCallback);
  connector->AddObserver(igtlioConnector::DeviceContentModifiedEvent, DeviceEventCallback);
  connector->AddObserver(igtlioConnector::RemovedDeviceEvent, RemovedDeviceCallback);
  connector->AddObserver(igtlioCommand::CommandResponseEvent, CommandEventCallback);
  connector->AddObserver(igtlioCommand::CommandExpiredEvent, CommandEventCallback);
  connector->AddObserver(igtlioCommand::CommandReceivedEvent, CommandEventCallback);
  connector->AddObserver(igtlioCommand::CommandCancelledEvent, CommandEventCallback);
  connector->AddObserver(igtlioCommand::CommandCompletedEvent, CommandEventCallback);

  this->InvokeEvent(ConnectionAddedEvent, connector.GetPointer());
  return connector;
}

//---------------------------------------------------------------------------
int igtlioLogic::CreateUniqueConnectorID() const
{
  int retval=0;
  for (unsigned int i=0; i<Connectors.size(); ++i)
    {
      retval = std::max<int>(retval, Connectors[i]->GetUID()+1);
    }
  return retval;
}

//---------------------------------------------------------------------------
int igtlioLogic::RemoveConnector(unsigned int index)
{
  std::vector<igtlioConnectorPointer>::iterator toRemove = Connectors.begin()+index;

  return this->RemoveConnector(toRemove);
}

//---------------------------------------------------------------------------
int igtlioLogic::RemoveConnector(igtlioConnectorPointer connector)
{

  std::vector<igtlioConnectorPointer>::iterator toRemove = Connectors.begin();
  for(; toRemove != Connectors.end(); ++toRemove)
  {
    if(connector == (*toRemove))
      break;
  }
  return this->RemoveConnector(toRemove);
}

//---------------------------------------------------------------------------
int igtlioLogic::RemoveConnector(std::vector<igtlioConnectorPointer>::iterator toRemove)
{
  if(toRemove == Connectors.end())
    return 0;

  toRemove->GetPointer()->RemoveObserver(NewDeviceCallback);
  toRemove->GetPointer()->RemoveObserver(RemovedDeviceCallback);
  toRemove->GetPointer()->RemoveObserver(DeviceEventCallback);
  toRemove->GetPointer()->RemoveObserver(CommandEventCallback);

  this->InvokeEvent(ConnectionAboutToBeRemovedEvent, toRemove->GetPointer());
  Connectors.erase(toRemove);
  return 1;
}


//---------------------------------------------------------------------------
int igtlioLogic::GetNumberOfConnectors() const
{
  return Connectors.size();
}

//---------------------------------------------------------------------------
igtlioConnectorPointer igtlioLogic::GetConnector(unsigned int index)
{
  if (index<Connectors.size())
    return Connectors[index];

  vtkErrorMacro("index " << index << " out of bounds.");
  return NULL;
}

igtlioSessionPointer igtlioLogic::StartServer(int serverPort, IGTLIO_SYNCHRONIZATION_TYPE sync, double timeout_s)
{
  igtlioSessionPointer session = igtlioSessionPointer::New();
  session->SetConnector(this->CreateConnector());
  session->StartServer(serverPort, sync, timeout_s);
  return session;
}

igtlioSessionPointer igtlioLogic::ConnectToServer(std::string serverHost, int serverPort, IGTLIO_SYNCHRONIZATION_TYPE sync, double timeout_s)
{
  igtlioSessionPointer session = igtlioSessionPointer::New();
  session->SetConnector(this->CreateConnector());
  session->ConnectToServer(serverHost, serverPort, sync, timeout_s);
  return session;
}

//---------------------------------------------------------------------------
void igtlioLogic::PeriodicProcess()
{
  for (unsigned i=0; i<Connectors.size(); ++i)
    {
      Connectors[i]->PeriodicProcess();
    }
}

//---------------------------------------------------------------------------
unsigned int igtlioLogic::GetNumberOfDevices() const
{
  std::vector<igtlioDevicePointer> all = this->CreateDeviceList();
  return all.size();
}

//---------------------------------------------------------------------------
void igtlioLogic::RemoveDevice(unsigned int index)
{
  igtlioDevicePointer device = this->GetDevice(index);

  for (unsigned i=0; i<Connectors.size(); ++i)
    {
      for (unsigned j=0; j<Connectors[i]->GetNumberOfDevices(); ++j)
        {
          igtlioDevicePointer local = Connectors[i]->GetDevice(j);
          if (device==local)
            Connectors[i]->RemoveDevice(j);
        }
    }
}

//---------------------------------------------------------------------------
igtlioDevicePointer igtlioLogic::GetDevice(unsigned int index)
{
  // TODO: optimize by caching the vector if necessary
  std::vector<igtlioDevicePointer> all = this->CreateDeviceList();

  if (index<all.size())
    return all[index];

  vtkErrorMacro("index " << index << " out of bounds.");
  return NULL;
}

//---------------------------------------------------------------------------
int igtlioLogic::ConnectorIndexFromDevice( igtlioDevicePointer d )
{
    for( std::vector<igtlioConnectorPointer>::size_type i = 0; i < Connectors.size(); ++i )
        if( Connectors[i]->HasDevice(d) )
            return i;
    return -1;
}

//---------------------------------------------------------------------------
std::vector<igtlioDevicePointer> igtlioLogic::CreateDeviceList() const
{
  std::set<igtlioDevicePointer> all;

  for (unsigned i=0; i<Connectors.size(); ++i)
    {
      for (unsigned j=0; j<Connectors[i]->GetNumberOfDevices(); ++j)
        {
          all.insert(Connectors[i]->GetDevice(j));
        }
    }

  return std::vector<igtlioDevicePointer>(all.begin(), all.end());
}
