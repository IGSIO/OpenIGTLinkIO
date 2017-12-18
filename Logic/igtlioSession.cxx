#include "igtlioSession.h"

#include <vtksys/SystemTools.hxx>
#include <vtkObjectFactory.h>
#include <vtkNew.h>
#include "vtkTimerLog.h"
#include "igtlioConnector.h"
#include "igtlioCommandDevice.h"
#include "igtlioImageDevice.h"
#include "igtlioTransformDevice.h"
#include "igtlioStringDevice.h"
#include "igtlioStatusDevice.h"

#if OpenIGTLink_ENABLE_VIDEOSTREAMING == 1
  #include "igtlioVideoDevice.h"
#endif


namespace igtlio
{

//---------------------------------------------------------------------------
vtkStandardNewMacro(Session);


//----------------------------------------------------------------------

void Session::PrintSelf(std::ostream &, vtkIndent)
{

}

Session::Session()
{
}




CommandDevicePointer Session::SendCommand(std::string device_id, std::string command, std::string content, igtlio::SYNCHRONIZATION_TYPE synchronized, double timeout_s)
{
  CommandDevicePointer device = Connector->SendCommand( device_id, command, content );

  if (synchronized==igtlio::BLOCKING)
  {
    double starttime = vtkTimerLog::GetUniversalTime();
    while (vtkTimerLog::GetUniversalTime() - starttime < timeout_s)
    {
      Connector->PeriodicProcess();
      vtksys::SystemTools::Delay(5);

      CommandDevicePointer response = device->GetResponseFromCommandID(device->GetContent().id);

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

  return vtkSmartPointer<CommandDevice>();
}

CommandDevicePointer Session::SendCommandResponse(std::string device_id, std::string command, std::string content)
{
  DeviceKeyType key(igtlio::CommandConverter::GetIGTLTypeName(), device_id);
  CommandDevicePointer device = CommandDevice::SafeDownCast(Connector->GetDevice(key));

  igtlio::CommandConverter::ContentData contentdata = device->GetContent();

  if (command != contentdata.name)
  {
    vtkErrorMacro("Requested command response " << command << " does not match the existing query: " << contentdata.name);
    return CommandDevicePointer();
  }

  contentdata.name = command;
  contentdata.content = content;
  device->SetContent(contentdata);

  Connector->SendMessage(CreateDeviceKey(device), Device::MESSAGE_PREFIX_RTS);
  return device;
}

ImageDevicePointer Session::SendImage(std::string device_id, vtkSmartPointer<vtkImageData> image, vtkSmartPointer<vtkMatrix4x4> transform)
{
  ImageDevicePointer device;
  DeviceKeyType key(igtlio::ImageConverter::GetIGTLTypeName(), device_id);
  device = ImageDevice::SafeDownCast(Connector->AddDeviceIfNotPresent(key));

  igtlio::ImageConverter::ContentData contentdata = device->GetContent();
  contentdata.image = image;
  contentdata.transform = transform;
  device->SetContent(contentdata);

  Connector->SendMessage(CreateDeviceKey(device));

  return device;
}

#if OpenIGTLink_ENABLE_VIDEOSTREAMING == 1
VideoDevicePointer Session::SendFrame(std::string device_id, vtkSmartPointer<vtkImageData> image)
{
  VideoDevice* device;
  DeviceKeyType key(igtlio::VideoConverter::GetIGTLTypeName(), device_id);
  device = VideoDevice::SafeDownCast(Connector->AddDeviceIfNotPresent(key));
  
  igtlio::VideoConverter::ContentData contentdata = device->GetContent();
  contentdata.image = image;
  device->SetContent(contentdata);
  
  Connector->SendMessage(CreateDeviceKey(device));
  
  return device;
}
#endif
  
ConnectorPointer Session::GetConnector()
{
  return Connector;
}

void Session::SetConnector(ConnectorPointer connector)
{
  Connector = connector;
}

void Session::StartServer(int serverPort, igtlio::SYNCHRONIZATION_TYPE sync, double timeout_s)
{
  if (!Connector)
  {
    vtkWarningMacro("No connector, ignoring connect request");
    return;
  }
  if (Connector->GetState()!=Connector::STATE_OFF)
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

void Session::ConnectToServer(std::string serverHost, int serverPort, igtlio::SYNCHRONIZATION_TYPE sync, double timeout_s)
{
  if (!Connector)
  {
    vtkWarningMacro("No connector, ignoring connect request");
    return;
  }
  if (Connector->GetState()!=Connector::STATE_OFF)
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

bool Session::waitForConnection(double timeout_s)
{
  double starttime = vtkTimerLog::GetUniversalTime();

  while (vtkTimerLog::GetUniversalTime() - starttime < timeout_s)
  {
    Connector->PeriodicProcess();
    vtksys::SystemTools::Delay(5);

    if (Connector->GetState() != Connector::STATE_WAIT_CONNECTION)
    {
      break;
    }
  }

  return Connector->GetState() == Connector::STATE_CONNECTED;
}


TransformDevicePointer Session::SendTransform(std::string device_id, vtkSmartPointer<vtkMatrix4x4> transform)
{
  TransformDevicePointer device;
  DeviceKeyType key(TransformConverter::GetIGTLTypeName(), device_id);
  device = TransformDevice::SafeDownCast(Connector->AddDeviceIfNotPresent(key));

  TransformConverter::ContentData contentdata = device->GetContent();
  contentdata.deviceName = device_id;
  contentdata.transform = transform;
  device->SetContent(contentdata);

  Connector->SendMessage(CreateDeviceKey(device));

  return device;
}

StringDevicePointer Session::SendString(std::string device_id, std::string content)
{
  StringDevicePointer device;
  DeviceKeyType key(StringConverter::GetIGTLTypeName(), device_id);
  device = StringDevice::SafeDownCast((Connector->AddDeviceIfNotPresent(key)));

  StringConverter::ContentData contentdata = device->GetContent();
  contentdata.encoding = 3; //what should this be?
  contentdata.string_msg = content;

  Connector->SendMessage(CreateDeviceKey(device));

  return device;
}

StatusDevicePointer Session::SendStatus(std::string device_id, int code, int subcode, std::string statusstring, std::string errorname)
{
	StatusDevicePointer device;
	DeviceKeyType key(StatusConverter::GetIGTLTypeName(), device_id);
    device = StatusDevice::SafeDownCast((Connector->AddDeviceIfNotPresent(key)));

	StatusConverter::ContentData contentdata = device->GetContent();
	contentdata.code = code;
	contentdata.errorname = errorname;
	contentdata.statusstring = statusstring;
	contentdata.subcode = subcode;

	Connector->SendMessage(CreateDeviceKey(device));

	return device;
}

} //namespace igtlio
