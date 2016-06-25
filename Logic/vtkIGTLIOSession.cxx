#include "vtkIGTLIOSession.h"

#include <vtkObjectFactory.h>
#include <vtkNew.h>

#include "vtkIGTLIOConnector.h"
#include <vtksys/SystemTools.hxx>
#include "vtkTimerLog.h"
#include "vtkIGTLIOCommandDevice.h"


//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLIOSession);


//----------------------------------------------------------------------

void vtkIGTLIOSession::PrintSelf(std::ostream &, vtkIndent)
{

}

vtkIGTLIOSession::vtkIGTLIOSession()
{
}

vtkIGTLIODevicePointer vtkIGTLIOSession::AddDeviceIfNotPresent(DeviceKeyType key)
{
  vtkSmartPointer<vtkIGTLIODevice> device = Connector->GetDevice(key);

  if (!device)
  {
    device = Connector->GetDeviceFactory()->create(key.type, key.name);
    Connector->AddDevice(device);
  }

  return device;
}


vtkIGTLIOCommandDevicePointer vtkIGTLIOSession::SendCommandQuery(std::string device_id,
                                                                 std::string command,
                                                                 std::string content,
                                                                 igtlio::SYNCHRONIZATION_TYPE synchronized,
                                                                 double timeout_s)
{
  vtkSmartPointer<vtkIGTLIOCommandDevice> device;
  DeviceKeyType key(igtl::CommandConverter::GetIGTLTypeName(), device_id);
  device = vtkIGTLIOCommandDevice::SafeDownCast(this->AddDeviceIfNotPresent(key));

  igtl::CommandConverter::ContentData contentdata = device->GetContent();
  contentdata.id +=1;
  contentdata.name = command;
  contentdata.content = content;
  device->SetContent(contentdata);

  device->PruneCompletedQueries();

  Connector->SendMessage(CreateDeviceKey(device));

  vtkSmartPointer<vtkIGTLIOCommandDevice> response;

  if (synchronized==igtlio::BLOCKING)
  {
    double starttime = vtkTimerLog::GetUniversalTime();
    while (vtkTimerLog::GetUniversalTime() - starttime < timeout_s)
    {
      Connector->PeriodicProcess();
      vtksys::SystemTools::Delay(5);

      vtkSmartPointer<vtkIGTLIOCommandDevice> response;
      response = device->GetResponseFromCommandID(contentdata.id);

      if (response)
      {
        break;
      }
    }
  }

  return response;
}

vtkIGTLIOConnectorPointer vtkIGTLIOSession::GetConnector()
{
  return Connector;
}

void vtkIGTLIOSession::SetConnector(vtkIGTLIOConnectorPointer connector)
{
  Connector = connector;
}

void vtkIGTLIOSession::StartServer(int serverPort, igtlio::SYNCHRONIZATION_TYPE sync, double timeout_s)
{
  if (!Connector)
  {
    vtkWarningMacro("No connector, ignoring connect request");
    return;
  }
  if (Connector->GetState()!=vtkIGTLIOConnector::STATE_OFF)
  {
    vtkWarningMacro("Session is already working, ignoring connect request");
    return;
  }

  if (serverPort<=0)
    serverPort = Connector->GetServerPort();

  Connector->SetTypeServer(serverPort);
  Connector->Start();

  if (sync==igtlio::BLOCKING)
  {
    this->waitForConnection(timeout_s);
  }
}

void vtkIGTLIOSession::ConnectToServer(std::string serverHost, int serverPort, igtlio::SYNCHRONIZATION_TYPE sync, double timeout_s)
{
  if (!Connector)
  {
    vtkWarningMacro("No connector, ignoring connect request");
    return;
  }
  if (Connector->GetState()!=vtkIGTLIOConnector::STATE_OFF)
  {
    vtkWarningMacro("Session is already working, ignoring connect request");
    return;
  }

  if (serverPort<=0)
    serverPort = Connector->GetServerPort();
  Connector->SetTypeClient(serverHost, serverPort);
  Connector->Start();

  if (sync==igtlio::BLOCKING)
  {
    this->waitForConnection(timeout_s);
  }
}

bool vtkIGTLIOSession::waitForConnection(double timeout_s)
{
  double starttime = vtkTimerLog::GetUniversalTime();

  while (vtkTimerLog::GetUniversalTime() - starttime < timeout_s)
  {
    Connector->PeriodicProcess();
    vtksys::SystemTools::Delay(5);

    if (Connector->GetState() != vtkIGTLIOConnector::STATE_WAIT_CONNECTION)
    {
      break;
    }
  }

  return Connector->GetState() == vtkIGTLIOConnector::STATE_CONNECTED;
}

