#include "igtlioSession.h"

#include <vtksys/SystemTools.hxx>
#include <vtkObjectFactory.h>
#include <vtkNew.h>
#include "vtkTimerLog.h"
#include "igtlioConnector.h"
#include "igtlioImageDevice.h"
#include "igtlioTransformDevice.h"
#include "igtlioStringDevice.h"
#include "igtlioStatusDevice.h"

#if defined(OpenIGTLink_ENABLE_VIDEOSTREAMING)
  #include "igtlioVideoDevice.h"
#endif

//---------------------------------------------------------------------------
vtkStandardNewMacro(igtlioSession);


//----------------------------------------------------------------------
void igtlioSession::PrintSelf(std::ostream &, vtkIndent)
{

}

igtlioSession::igtlioSession()
{
}

//----------------------------------------------------------------------
igtlioCommandPointer igtlioSession::SendCommand(std::string name, std::string content, IGTLIO_SYNCHRONIZATION_TYPE synchronized, double timeout_s/*=5.0*/, igtl::MessageBase::MetaDataMap* metaData/*=NULL*/)
{
  igtlioCommandPointer command = Connector->SendCommand(name, content, synchronized, timeout_s, metaData);
  return command;
}

//----------------------------------------------------------------------
int igtlioSession::SendCommandResponse(igtlioCommandPointer command)
{
  return Connector->SendCommandResponse(command);
}

//----------------------------------------------------------------------
igtlioImageDevicePointer igtlioSession::SendImage(std::string device_id, vtkSmartPointer<vtkImageData> image, vtkSmartPointer<vtkMatrix4x4> transform)
{
  igtlioImageDevicePointer device;
  igtlioDeviceKeyType key(igtlioImageConverter::GetIGTLTypeName(), device_id);
  device = igtlioImageDevice::SafeDownCast(Connector->AddDeviceIfNotPresent(key));

  igtlioImageConverter::ContentData contentdata = device->GetContent();
  contentdata.image = image;
  contentdata.transform = transform;
  device->SetContent(contentdata);

  Connector->SendMessage(igtlioDeviceKeyType::CreateDeviceKey(device));

  return device;
}

#if defined(OpenIGTLink_ENABLE_VIDEOSTREAMING)
#include "igtl_video.h"
igtlioVideoDevicePointer igtlioSession::SendFrame(std::string device_id, vtkSmartPointer<vtkImageData> image)
{
  igtlioVideoDevicePointer device;
  igtlioDeviceKeyType key(igtlioVideoConverter::GetIGTLTypeName(), device_id);
  device = igtlioVideoDevice::SafeDownCast(Connector->AddDeviceIfNotPresent(key));

  igtlioVideoConverter::ContentData contentdata = device->GetContent();
  contentdata.image = image;
  device->SetContent(contentdata);
  #if defined(OpenIGTLink_USE_VP9)
  device->SetCurrentCodecType(IGTL_VIDEO_CODEC_NAME_VP9);
  #elif defined(OpenIGTLink_USE_H264) 
  device->SetCurrentCodecType(IGTL_VIDEO_CODEC_NAME_H264);
  #elif defined(OpenIGTLink_USE_X265)
  device->SetCurrentCodecType(IGTL_VIDEO_CODEC_NAME_X265);
  #endif
  Connector->SendMessage(igtlioDeviceKeyType::CreateDeviceKey(device));

  return device;
}
#endif

//----------------------------------------------------------------------
igtlioConnectorPointer igtlioSession::GetConnector()
{
  return Connector;
}

//----------------------------------------------------------------------
void igtlioSession::SetConnector(igtlioConnectorPointer connector)
{
  Connector = connector;
}

//----------------------------------------------------------------------
void igtlioSession::StartServer(int serverPort, IGTLIO_SYNCHRONIZATION_TYPE sync, double timeout_s)
{
  if (!Connector)
  {
    vtkWarningMacro("No connector, ignoring connect request");
    return;
  }
  if (Connector->GetState()!=igtlioConnector::STATE_OFF)
  {
    vtkWarningMacro("Session is already working, ignoring connect request");
    return;
  }

  if (serverPort<=0)
    serverPort = Connector->GetServerPort();

  Connector->SetTypeServer(serverPort);
  Connector->Start();

  if (sync==IGTLIO_BLOCKING)
  {
    this->waitForConnection(timeout_s);
  }
}

//----------------------------------------------------------------------
void igtlioSession::ConnectToServer(std::string serverHost, int serverPort, IGTLIO_SYNCHRONIZATION_TYPE sync, double timeout_s)
{
  if (!Connector)
  {
    vtkWarningMacro("No connector, ignoring connect request");
    return;
  }
  if (Connector->GetState()!=igtlioConnector::STATE_OFF)
  {
    vtkWarningMacro("Session is already working, ignoring connect request");
    return;
  }

  if (serverPort<=0)
    serverPort = Connector->GetServerPort();
  Connector->SetTypeClient(serverHost, serverPort);
  Connector->Start();

  if (sync==IGTLIO_BLOCKING)
  {
    this->waitForConnection(timeout_s);
  }
}

//----------------------------------------------------------------------
bool igtlioSession::waitForConnection(double timeout_s)
{
  double starttime = vtkTimerLog::GetUniversalTime();

  while (vtkTimerLog::GetUniversalTime() - starttime < timeout_s)
  {
    Connector->PeriodicProcess();
    vtksys::SystemTools::Delay(5);

    if (Connector->GetState() != igtlioConnector::STATE_WAIT_CONNECTION)
    {
      break;
    }
  }

  return Connector->GetState() == igtlioConnector::STATE_CONNECTED;
}

//----------------------------------------------------------------------
igtlioTransformDevicePointer igtlioSession::SendTransform(std::string device_id, vtkSmartPointer<vtkMatrix4x4> transform)
{
  igtlioTransformDevicePointer device;
  igtlioDeviceKeyType key(igtlioTransformConverter::GetIGTLTypeName(), device_id);
  device = igtlioTransformDevice::SafeDownCast(Connector->AddDeviceIfNotPresent(key));

  igtlioTransformConverter::ContentData contentdata = device->GetContent();
  contentdata.deviceName = device_id;
  contentdata.transform = transform;
  device->SetContent(contentdata);

  Connector->SendMessage(igtlioDeviceKeyType::CreateDeviceKey(device));

  return device;
}

//----------------------------------------------------------------------
igtlioStringDevicePointer igtlioSession::SendString(std::string device_id, std::string content)
{
  igtlioStringDevicePointer device;
  igtlioDeviceKeyType key(igtlioStringConverter::GetIGTLTypeName(), device_id);
  device = igtlioStringDevice::SafeDownCast((Connector->AddDeviceIfNotPresent(key)));

  igtlioStringConverter::ContentData contentdata = device->GetContent();
  contentdata.encoding = 3; //what should this be?
  contentdata.string_msg = content;

  Connector->SendMessage(igtlioDeviceKeyType::CreateDeviceKey(device));

  return device;
}

//----------------------------------------------------------------------
igtlioStatusDevicePointer igtlioSession::SendStatus(std::string device_id, int code, int subcode, std::string statusstring, std::string errorname)
{
  igtlioStatusDevicePointer device;
  igtlioDeviceKeyType key(igtlioStatusConverter::GetIGTLTypeName(), device_id);
  device = igtlioStatusDevice::SafeDownCast((Connector->AddDeviceIfNotPresent(key)));

  igtlioStatusConverter::ContentData contentdata = device->GetContent();
  contentdata.code = code;
  contentdata.errorname = errorname;
  contentdata.statusstring = statusstring;
  contentdata.subcode = subcode;

  Connector->SendMessage(igtlioDeviceKeyType::CreateDeviceKey(device));

  return device;
}
