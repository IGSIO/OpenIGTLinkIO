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
#include "vtkIGTLIOLogic.h"
#include "vtkIGTLIOConnector.h"


#include <vtkObjectFactory.h>

// OpenIGTLinkIF Logic includes
#include <vtkNew.h>
#include <vtkCallbackCommand.h>
#include <vtkImageData.h>
#include <vtkTransform.h>


//---------------------------------------------------------------------------
void onNewDeviceEventFunc(vtkObject* caller, unsigned long eid, void* clientdata, void *calldata)
{
  vtkIGTLIOLogic* logic = reinterpret_cast<vtkIGTLIOLogic*>(clientdata);
  logic->InvokeEvent(vtkIGTLIOLogic::NewDeviceEvent, calldata);
}

//---------------------------------------------------------------------------
void onRemovedDeviceEventFunc(vtkObject* caller, unsigned long eid, void* clientdata, void *calldata)
{
  vtkIGTLIOLogic* logic = reinterpret_cast<vtkIGTLIOLogic*>(clientdata);
  logic->InvokeEvent(vtkIGTLIOLogic::RemovedDeviceEvent, calldata);
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
  for (int i=0; i<Connectors.size(); ++i)
    {
      retval = std::max<int>(retval, Connectors[i]->GetUID()+1);
    }
  return retval;
}

//---------------------------------------------------------------------------
int vtkIGTLIOLogic::RemoveConnector(int index)
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
vtkIGTLIOConnectorPointer vtkIGTLIOLogic::GetConnector(int index)
{
  return Connectors[index];
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
int vtkIGTLIOLogic::GetNumberOfDevices() const
{
  std::vector<vtkIGTLIODevicePointer> all = this->CreateDeviceList();
  return all.size();
}

//---------------------------------------------------------------------------
void vtkIGTLIOLogic::RemoveDevice(int index)
{
  vtkIGTLIODevicePointer device = this->GetDevice(index);

  for (unsigned i=0; i<Connectors.size(); ++i)
    {
      for (unsigned j=0; j<Connectors[i]->GetNumberOfDevices(); ++j)
        {
          vtkIGTLIODevicePointer local = Connectors[i]->GetDevice(j);
          if (device==local)
            Connectors[i]->RemoveDevice(j);
        }
    }
}

//---------------------------------------------------------------------------
vtkIGTLIODevicePointer vtkIGTLIOLogic::GetDevice(int index)
{
  // TODO: optimize by caching the vector if necessary
  std::vector<vtkIGTLIODevicePointer> all = this->CreateDeviceList();

  if (index<0 || index>=all.size())
    {
      return vtkIGTLIODevicePointer();
    }

  return all[index];
}

//---------------------------------------------------------------------------
std::vector<vtkIGTLIODevicePointer> vtkIGTLIOLogic::CreateDeviceList() const
{
  std::set<vtkIGTLIODevicePointer> all;

  for (unsigned i=0; i<Connectors.size(); ++i)
    {
      for (unsigned j=0; j<Connectors[i]->GetNumberOfDevices(); ++j)
        {
          all.insert(Connectors[i]->GetDevice(j));
        }
    }

  return std::vector<vtkIGTLIODevicePointer>(all.begin(), all.end());
}


