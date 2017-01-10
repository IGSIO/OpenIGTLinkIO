#include "vtkIGTLIOSession.h"

#include <vtkObjectFactory.h>
#include <vtkNew.h>

#include "vtkIGTLIOConnector.h"
#include <vtksys/SystemTools.hxx>
#include "vtkTimerLog.h"
#include "vtkIGTLIOCommandDevice.h"
#include "vtkIGTLIOImageDevice.h"
#include "vtkIGTLIOTransformDevice.h"


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

  if (synchronized==igtlio::BLOCKING)
  {
    double starttime = vtkTimerLog::GetUniversalTime();
    while (vtkTimerLog::GetUniversalTime() - starttime < timeout_s)
    {
      Connector->PeriodicProcess();
      vtksys::SystemTools::Delay(5);

      vtkIGTLIOCommandDevicePointer response = device->GetResponseFromCommandID(contentdata.id);

      if (response)
      {
        return response;
      }
    }
  }
  else
  {
    return device;
  }

  return vtkSmartPointer<vtkIGTLIOCommandDevice>();
}

vtkIGTLIOCommandDevicePointer vtkIGTLIOSession::SendCommandResponse(std::string device_id, std::string command, std::string content)
{
  DeviceKeyType key(igtl::CommandConverter::GetIGTLTypeName(), device_id);
  vtkIGTLIOCommandDevicePointer device = vtkIGTLIOCommandDevice::SafeDownCast(Connector->GetDevice(key));

  igtl::CommandConverter::ContentData contentdata = device->GetContent();

  if (command != contentdata.name)
  {
    vtkErrorMacro("Requested command response " << command << " does not match the existing query: " << contentdata.name);
    return vtkIGTLIOCommandDevicePointer();
  }

  contentdata.name = command;
  contentdata.content = content;
  device->SetContent(contentdata);

  Connector->SendMessage(CreateDeviceKey(device), vtkIGTLIODevice::MESSAGE_PREFIX_REPLY);
  return device;
}

vtkIGTLIOImageDevicePointer vtkIGTLIOSession::SendImage(std::string device_id, vtkSmartPointer<vtkImageData> image, vtkSmartPointer<vtkMatrix4x4> transform)
{
  vtkSmartPointer<vtkIGTLIOImageDevice> device;
  DeviceKeyType key(igtl::ImageConverter::GetIGTLTypeName(), device_id);
  device = vtkIGTLIOImageDevice::SafeDownCast(this->AddDeviceIfNotPresent(key));

  igtl::ImageConverter::ContentData contentdata = device->GetContent();
  contentdata.image = image;
  contentdata.transform = transform;
  device->SetContent(contentdata);

  Connector->SendMessage(CreateDeviceKey(device));

  return device;
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


vtkIGTLIOTransformDevicePointer vtkIGTLIOSession::SendTransform(std::string device_id, vtkSmartPointer<vtkMatrix4x4> transform)
{
  vtkIGTLIOTransformDevicePointer device;
  DeviceKeyType key(igtl::TransformConverter::GetIGTLTypeName(), device_id);
  device = vtkIGTLIOTransformDevice::SafeDownCast(this->AddDeviceIfNotPresent(key));

  igtl::TransformConverter::ContentData contentdata = device->GetContent();
  contentdata.deviceName = device_id;
  contentdata.transform = transform;
  device->SetContent(contentdata);

  Connector->SendMessage(CreateDeviceKey(device));

  return device;
}
