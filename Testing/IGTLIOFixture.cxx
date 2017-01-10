#include "IGTLIOFixture.h"

#include "vtkImageData.h"
#include "igtlImageConverter.h"
#include "vtkMatrix4x4.h"
#include <vtksys/SystemTools.hxx>
#include "vtkIGTLIOSession.h"


bool contains(std::vector<int> input, int value)
{
  for(int i=0; i<input.size(); ++i)
    {
      if(input.at(i) == value)
        return true;
    }
  return false;
}

//---------------------------------------------------------------------------
void onReceivedEventFunc(vtkObject* caller, unsigned long eid, void* clientdata, void *calldata)
{
  LogicFixture* self = reinterpret_cast<LogicFixture*>(clientdata);
  self->ReceivedEvents.push_back(eid);
}

LogicFixture::LogicFixture()
{
  //ReceivedEvents = -1;
  Logic = vtkIGTLIOLogicPointer::New();

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
  Session = Logic->ConnectToServer("localhost", -1, igtlio::ASYNCHRONOUS);
  Connector = Session->GetConnector();
  std::cout << "Starting CLIENT connector " << Connector.GetPointer()  << std::endl;
}

void LogicFixture::startServer()
{
  Session = Logic->StartServer(-1, igtlio::ASYNCHRONOUS);
  Connector = Session->GetConnector();
  std::cout << "Starting SERVER connector " << Connector.GetPointer() << std::endl;
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
  logic->ReceivedEvents.clear();

  double timeout = 2;
  double starttime = vtkTimerLog::GetUniversalTime();
  // Client waits for an image to be sent from the server.
  while (vtkTimerLog::GetUniversalTime() - starttime < timeout)
  {
    Server.Logic->PeriodicProcess();
    Client.Logic->PeriodicProcess();
    vtksys::SystemTools::Delay(5);

    if (contains(logic->ReceivedEvents, eventId))
    {
      return true;
    }
  }

  std::cout << "FAILURE: Expected event: " << eventId << ", "
            << "got nothing."
            << std::endl;

  return false;
}

vtkSmartPointer<vtkImageData> ClientServerFixture::CreateTestImage()
{
  vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New();
  image->SetSpacing(1.5, 1.2, 1);
  image->SetExtent(0, 19, 0, 49, 0, 1);
  image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

  int scalarSize = image->GetScalarSize();
  unsigned char* ptr = reinterpret_cast<unsigned char*>(image->GetScalarPointer());
  unsigned char color = 0;
  std::fill(ptr, ptr+scalarSize, color++);

  return image;
}

vtkSmartPointer<vtkMatrix4x4> ClientServerFixture::CreateTestTransform()
{
  vtkSmartPointer<vtkMatrix4x4> transform = vtkSmartPointer<vtkMatrix4x4>::New();
  transform->Identity();
  return transform;
}
