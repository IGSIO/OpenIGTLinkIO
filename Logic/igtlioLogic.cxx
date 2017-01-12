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


namespace igtlio
{

//---------------------------------------------------------------------------
void onNewDeviceEventFunc(vtkObject* caller, unsigned long eid, void* clientdata, void *calldata)
{
  vtkIGTLIOLogic* logic = reinterpret_cast<vtkIGTLIOLogic*>(clientdata);
  logic->InvokeEvent(vtkIGTLIOLogic::NewDeviceEvent, calldata);

  Device* device = reinterpret_cast<Device*>(calldata);
  device->AddObserver(Device::CommandQueryReceivedEvent, logic->DeviceEventCallback);
  device->AddObserver(Device::CommandResponseReceivedEvent, logic->DeviceEventCallback);
}

//---------------------------------------------------------------------------
void onRemovedDeviceEventFunc(vtkObject* caller, unsigned long eid, void* clientdata, void *calldata)
{
  vtkIGTLIOLogic* logic = reinterpret_cast<vtkIGTLIOLogic*>(clientdata);
  logic->InvokeEvent(vtkIGTLIOLogic::RemovedDeviceEvent, calldata);

  Device* device = reinterpret_cast<Device*>(calldata);
  device->RemoveObserver(logic->DeviceEventCallback);
}

//---------------------------------------------------------------------------
void onDeviceEventFunc(vtkObject* caller, unsigned long eid, void* clientdata, void *calldata)
{
  vtkIGTLIOLogic* logic = reinterpret_cast<vtkIGTLIOLogic*>(clientdata);

  if ((eid==Device::CommandQueryReceivedEvent) ||
      (eid==Device::CommandResponseReceivedEvent))
  {
    logic->InvokeEvent(eid, calldata);
  }
}


//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLIOLogic);


//---------------------------------------------------------------------------
vtkIGTLIOLogic::vtkIGTLIOLogic()
{
  NewDeviceCallback = vtkSmartPointer<vtkCallbackCommand>::New();
  NewDeviceCallback->SetCallback(onNewDeviceEventFunc);
  NewDeviceCallback->SetClientData(this);

  RemovedDeviceCallback = vtkSmartPointer<vtkCallbackCommand>::New();
  RemovedDeviceCallback->SetCallback(onRemovedDeviceEventFunc);
  RemovedDeviceCallback->SetClientData(this);

  DeviceEventCallback = vtkSmartPointer<vtkCallbackCommand>::New();
  DeviceEventCallback->SetCallback(onDeviceEventFunc);
  DeviceEventCallback->SetClientData(this);
}

//---------------------------------------------------------------------------
vtkIGTLIOLogic::~vtkIGTLIOLogic()
{
}

//---------------------------------------------------------------------------
void vtkIGTLIOLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << "vtkIGTLIOLogic:             " << this->GetClassName() << "\n";
}


//---------------------------------------------------------------------------
vtkIGTLIOConnectorPointer vtkIGTLIOLogic::CreateConnector()
{
  vtkIGTLIOConnectorPointer connector = vtkIGTLIOConnectorPointer::New();
  connector->SetUID(this->CreateUniqueConnectorID());
  std::stringstream ss;
  ss << "IGTLConnector_" << connector->GetUID();
  connector->SetName(ss.str());
  Connectors.push_back(connector);

  connector->AddObserver(vtkIGTLIOConnector::NewDeviceEvent, NewDeviceCallback);
  connector->AddObserver(vtkIGTLIOConnector::RemovedDeviceEvent, RemovedDeviceCallback);

  this->InvokeEvent(ConnectionAddedEvent, connector.GetPointer());
  return connector;
}

//---------------------------------------------------------------------------
int vtkIGTLIOLogic::CreateUniqueConnectorID() const
{
  int retval=0;
  for (unsigned int i=0; i<Connectors.size(); ++i)
    {
      retval = std::max<int>(retval, Connectors[i]->GetUID()+1);
    }
  return retval;
}

//---------------------------------------------------------------------------
int vtkIGTLIOLogic::RemoveConnector(unsigned int index)
{
  std::vector<vtkIGTLIOConnectorPointer>::iterator toRemove = Connectors.begin()+index;

  toRemove->GetPointer()->RemoveObserver(NewDeviceCallback);
  toRemove->GetPointer()->RemoveObserver(RemovedDeviceCallback);

  this->InvokeEvent(ConnectionAboutToBeRemovedEvent, toRemove->GetPointer());
  Connectors.erase(toRemove);
  return 0;
}

//---------------------------------------------------------------------------
int vtkIGTLIOLogic::GetNumberOfConnectors() const
{
  return Connectors.size();
}

//---------------------------------------------------------------------------
vtkIGTLIOConnectorPointer vtkIGTLIOLogic::GetConnector(unsigned int index)
{
  if (index>=0 && index<Connectors.size())
    return Connectors[index];

  vtkErrorMacro("index " << index << " out of bounds.");
  return NULL;
}

vtkIGTLIOSessionPointer vtkIGTLIOLogic::StartServer(int serverPort, igtlio::SYNCHRONIZATION_TYPE sync, double timeout_s)
{
  vtkIGTLIOSessionPointer session = vtkIGTLIOSessionPointer::New();
  session->SetConnector(this->CreateConnector());
  session->StartServer(serverPort, sync, timeout_s);
  return session;
}

vtkIGTLIOSessionPointer vtkIGTLIOLogic::ConnectToServer(std::string serverHost, int serverPort, igtlio::SYNCHRONIZATION_TYPE sync, double timeout_s)
{
  vtkIGTLIOSessionPointer session = vtkIGTLIOSessionPointer::New();
  session->SetConnector(this->CreateConnector());
  session->ConnectToServer(serverHost, serverPort, sync, timeout_s);
  return session;
}

//---------------------------------------------------------------------------
void vtkIGTLIOLogic::PeriodicProcess()
{
  for (unsigned i=0; i<Connectors.size(); ++i)
    {
      Connectors[i]->PeriodicProcess();
    }
}

//---------------------------------------------------------------------------
unsigned int vtkIGTLIOLogic::GetNumberOfDevices() const
{
  std::vector<DevicePointer> all = this->CreateDeviceList();
  return all.size();
}

//---------------------------------------------------------------------------
void vtkIGTLIOLogic::RemoveDevice(unsigned int index)
{
  DevicePointer device = this->GetDevice(index);

  for (unsigned i=0; i<Connectors.size(); ++i)
    {
      for (unsigned j=0; j<Connectors[i]->GetNumberOfDevices(); ++j)
        {
          DevicePointer local = Connectors[i]->GetDevice(j);
          if (device==local)
            Connectors[i]->RemoveDevice(j);
        }
    }
}

//---------------------------------------------------------------------------
DevicePointer vtkIGTLIOLogic::GetDevice(unsigned int index)
{
  // TODO: optimize by caching the vector if necessary
  std::vector<DevicePointer> all = this->CreateDeviceList();

  if (index>=0 && index<all.size())
    return all[index];

  vtkErrorMacro("index " << index << " out of bounds.");
  return NULL;
}

//---------------------------------------------------------------------------
std::vector<DevicePointer> vtkIGTLIOLogic::CreateDeviceList() const
{
  std::set<DevicePointer> all;

  for (unsigned i=0; i<Connectors.size(); ++i)
    {
      for (unsigned j=0; j<Connectors[i]->GetNumberOfDevices(); ++j)
        {
          all.insert(Connectors[i]->GetDevice(j));
        }
    }

  return std::vector<DevicePointer>(all.begin(), all.end());
}

} // namespace igtlio

