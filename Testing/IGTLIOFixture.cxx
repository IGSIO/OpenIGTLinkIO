#include "IGTLIOFixture.h"

#include "vtkImageData.h"
#include "igtlImageConverter.h"
#include "vtkMatrix4x4.h"
#include <vtksys/SystemTools.hxx>

//---------------------------------------------------------------------------
void onReceivedEventFunc(vtkObject* caller, unsigned long eid, void* clientdata, void *calldata)
{
  LogicFixture* self = reinterpret_cast<LogicFixture*>(clientdata);
  self->LastReceivedEvent = eid;
}

LogicFixture::LogicFixture()
{
  LastReceivedEvent = -1;

  Logic = vtkIGTLIOLogicPointer::New();
  Connector = Logic->CreateConnector();

  LogicEventCallback = vtkSmartPointer<vtkCallbackCommand>::New();
  LogicEventCallback->SetCallback(onReceivedEventFunc);
  LogicEventCallback->SetClientData(this);

  Logic->AddObserver(vtkIGTLIOLogic::NewDeviceEvent, LogicEventCallback);
  Logic->AddObserver(vtkIGTLIOLogic::RemovedDeviceEvent, LogicEventCallback);
  Logic->AddObserver(vtkIGTLIOLogic::CommandQueryReceivedEvent, LogicEventCallback);
  Logic->AddObserver(vtkIGTLIOLogic::CommandResponseReceivedEvent, LogicEventCallback);
}

void LogicFixture::startClient()
{
  Connector->SetTypeClient(Connector->GetServerHostname(), Connector->GetServerPort());
  std::cout << "Starting CLIENT connector " << Connector.GetPointer()  << std::endl;
  Connector->Start();
}

void LogicFixture::startServer()
{
  Connector->SetTypeServer(Connector->GetServerPort());
  std::cout << "Starting SERVER connector " << Connector.GetPointer() << std::endl;
  Connector->Start();
}

vtkSmartPointer<vtkIGTLIOImageDevice> LogicFixture::CreateDummyImageDevice()
{
  vtkSmartPointer<vtkIGTLIOImageDevice> imageDevice;
  imageDevice = vtkIGTLIOImageDevice::SafeDownCast(vtkIGTLIOImageDeviceCreator::New()->Create("TestDevice_Image"));
  igtl::ImageConverter::ContentData imageContent;
  imageContent.image = vtkSmartPointer<vtkImageData>::New();
  imageContent.image->SetSpacing(1.5, 1.2, 1);
  imageContent.image->SetExtent(0, 19, 0, 49, 0, 1);
  imageContent.image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
  imageContent.transform = vtkSmartPointer<vtkMatrix4x4>::New();
  imageContent.transform->Identity();

  int scalarSize = imageContent.image->GetScalarSize();
  unsigned char* ptr = reinterpret_cast<unsigned char*>(imageContent.image->GetScalarPointer());
  unsigned char color = 0;
  std::fill(ptr, ptr+scalarSize, color++);

  imageDevice->SetContent(imageContent);
  return imageDevice;
}

vtkSmartPointer<vtkIGTLIOCommandDevice> LogicFixture::CreateDummyCommandDevice()
{
  vtkSmartPointer<vtkIGTLIOCommandDevice> device;
  device = vtkIGTLIOCommandDevice::SafeDownCast(vtkIGTLIOCommandDeviceCreator::New()->Create("TestDevice_Command"));
  igtl::CommandConverter::ContentData content;

  content.id = 0;
  content.name = "GetDeviceParameters";
  content.content = ""
      "<Command>\n"
      "  <Parameter Name=\"Depth\" />\n"
      "</Command>";

  device->SetContent(content);
  return device;

}

bool LogicFixture::ConvertCommandDeviceToResponse(vtkSmartPointer<vtkIGTLIOCommandDevice> device)
{
  if (!device)
  {
    std::cout << "FAILURE: Non-command device received." << std::endl;
    return false;
  }

  igtl::CommandConverter::ContentData content = device->GetContent();
  if (content.name != "GetDeviceParameters")
  {
    std::cout << "FAILURE: Wrong command received." << std::endl;
    return false;
  }

  content.content = ""
      "<Command>\n"
      "  <Result>GetDeviceParameters: success</Result>\n"
      "  <Parameter Name=\"Depth\" Value=\"45\" />\n"
      "</Command>";

  device->SetContent(content);
  return true;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


bool ClientServerFixture::ConnectClientToServer()
{
  Server.startServer();

  Client.startClient();

  double timeout = 2;
  double starttime = vtkTimerLog::GetUniversalTime();

  // Client connects to server.
  while (vtkTimerLog::GetUniversalTime() - starttime < timeout)
  {
    Server.Logic->PeriodicProcess();
    Client.Logic->PeriodicProcess();
    vtksys::SystemTools::Delay(5);

    if (Client.Connector->GetState() == vtkIGTLIOConnector::STATE_CONNECTED)
    {
      std::cout << "SUCCESS: connected to server" << std::endl;
      return true;
    }
    if (Client.Connector->GetState() == vtkIGTLIOConnector::STATE_OFF)
    {
      std::cout << "FAILURE to connect to server" << std::endl;
      return false;
    }
  }

  std::cout << "TIMEOUT connecting to server" << std::endl;
  return false;
}


bool ClientServerFixture::LoopUntilEventDetected(LogicFixture* logic, int eventId)
{
  logic->LastReceivedEvent = -1;

  double timeout = 2;
  double starttime = vtkTimerLog::GetUniversalTime();
  // Client waits for an image to be sent from the server.
  while (vtkTimerLog::GetUniversalTime() - starttime < timeout)
  {
    Server.Logic->PeriodicProcess();
    Client.Logic->PeriodicProcess();
    vtksys::SystemTools::Delay(5);

    if (logic->LastReceivedEvent == eventId)
    {
      return true;
    }
  }

  std::cout << "FAILURE: Expected event: " << eventId << ", "
            << "got nothing."
            << std::endl;

  return false;
}
