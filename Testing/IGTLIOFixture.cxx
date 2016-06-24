#include "IGTLIOFixture.h"

#include "vtkImageData.h"
#include "igtlImageConverter.h"
#include "vtkMatrix4x4.h"
#include <vtksys/SystemTools.hxx>

//---------------------------------------------------------------------------
void onReceivedEventFunc(vtkObject* caller, unsigned long eid, void* clientdata, void *calldata)
{
  LogicFixture* self = reinterpret_cast<LogicFixture*>(clientdata);
  std::cout << "-------------------- LogicFixture=" << self << ", onReceivedEventFunc " << eid << std::endl;

  self->LastReceivedEvent = eid;
//  logic->InvokeEvent(vtkIGTLIOLogic::NewDeviceEvent, calldata);
}

LogicFixture::LogicFixture()
{
  LastReceivedEvent = -1;

  LogicEventCallback = vtkSmartPointer<vtkCallbackCommand>::New();
  LogicEventCallback->SetCallback(onReceivedEventFunc);
  LogicEventCallback->SetClientData(this);
}

void LogicFixture::startClient()
{
  Logic = vtkIGTLIOLogicPointer::New();
  Connector = Logic->CreateConnector();
  Connector->SetTypeClient(Connector->GetServerHostname(), Connector->GetServerPort());
  std::cout << "Starting CLIENT connector " << Connector.GetPointer()  << std::endl;
  Logic->AddObserver(vtkIGTLIOLogic::CommandQueryReceivedEvent, LogicEventCallback);
  Logic->AddObserver(vtkIGTLIOLogic::CommandResponseReceivedEvent, LogicEventCallback);
  Connector->Start();
}

void LogicFixture::startServer()
{
  Logic = vtkIGTLIOLogicPointer::New();
  Connector = Logic->CreateConnector();
  Connector->SetTypeServer(Connector->GetServerPort());
  std::cout << "Starting SERVER connector " << Connector.GetPointer() << std::endl;
  Logic->AddObserver(vtkIGTLIOLogic::CommandQueryReceivedEvent, LogicEventCallback);
  Logic->AddObserver(vtkIGTLIOLogic::CommandResponseReceivedEvent, LogicEventCallback);
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
      break;
    }
    if (Client.Connector->GetState() == vtkIGTLIOConnector::STATE_OFF)
    {
      std::cout << "FAILURE to connect to server" << std::endl;
      return false;
    }
  }

  return true;
}

bool ClientServerFixture::LoopUntilExpectedNumberOfDevicesReached(LogicFixture logic, int expectedNumberOfDevices)
{
  double timeout = 2;
  double starttime = vtkTimerLog::GetUniversalTime();
  // Client waits for an image to be sent from the server.
  while (vtkTimerLog::GetUniversalTime() - starttime < timeout)
  {
    Server.Logic->PeriodicProcess();
    Client.Logic->PeriodicProcess();
    vtksys::SystemTools::Delay(5);

    if (logic.Logic->GetNumberOfDevices() >= expectedNumberOfDevices)
    {
      return true;
    }
  }

  std::cout << "FAILURE: Expected " << expectedNumberOfDevices << " devices, "
            << "got " << logic.Logic->GetNumberOfDevices()
            << std::endl;

  return false;
}

bool ClientServerFixture::LoopUntilEventDetected(LogicFixture* logic, int eventId)
{
//  logic->LastReceivedEvent = -1;

  double timeout = 2;
  double starttime = vtkTimerLog::GetUniversalTime();
  // Client waits for an image to be sent from the server.
  while (vtkTimerLog::GetUniversalTime() - starttime < timeout)
  {
    Server.Logic->PeriodicProcess();
    Client.Logic->PeriodicProcess();
    vtksys::SystemTools::Delay(200);

    std::cout << "ClientServerFixture::LoopUntilEventDetected LogicFixture=" << logic  << ", eid=" << logic->LastReceivedEvent << std::endl;
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
