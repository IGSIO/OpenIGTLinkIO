#include "igtlioFixture.h"

#include "vtkImageData.h"
#include "igtlioImageConverter.h"
#include "vtkMatrix4x4.h"
#include <vtksys/SystemTools.hxx>
#include "igtlioSession.h"
#include "igtlioTestUtilities.h"

//---------------------------------------------------------------------------
void onReceivedEventFunc(vtkObject* caller, unsigned long eid, void* clientdata, void *calldata)
{
  LogicFixture* self = reinterpret_cast<LogicFixture*>(clientdata);
  self->Session->PrintSelf(std::cout, vtkIndent(1));
  self->ReceivedEvents.push_back(eid);
}

LogicFixture::LogicFixture()
{
  Logic = igtlio::LogicPointer::New();
  ReceivedEvents = std::vector<int>(1000);
  ReceivedEvents.clear();
  LogicEventCallback = vtkSmartPointer<vtkCallbackCommand>::New();
  LogicEventCallback->SetCallback(onReceivedEventFunc);
  LogicEventCallback->SetClientData(this);

  Logic->AddObserver(igtlio::Logic::NewDeviceEvent, LogicEventCallback);
  Logic->AddObserver(igtlio::Logic::RemovedDeviceEvent, LogicEventCallback);
  Logic->AddObserver(igtlio::Logic::CommandReceivedEvent, LogicEventCallback);
  Logic->AddObserver(igtlio::Logic::CommandResponseReceivedEvent, LogicEventCallback);
  Logic->AddObserver(igtlio::Logic::DeviceModifiedEvent, LogicEventCallback);
}

void LogicFixture::startClient()
{
  Session = Logic->ConnectToServer("localhost", -1, igtlio::ASYNCHRONOUS);
  Connector = Session->GetConnector();
}

void LogicFixture::startServer()
{
  Session = Logic->StartServer(-1, igtlio::ASYNCHRONOUS);
  Connector = Session->GetConnector();
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

    if (Client.Connector->GetState() == igtlio::Connector::STATE_CONNECTED)
    {
      std::cout << "SUCCESS: connected to server" << std::endl;
      return true;
    }
    if (Client.Connector->GetState() == igtlio::Connector::STATE_OFF)
    {
      std::cout << "FAILURE to connect to server" << std::endl;
      return false;
    }
  }

  std::cout << "TIMEOUT connecting to server" << std::endl;
  return false;
}

bool ClientServerFixture::LoopUntilEventDetected(LogicFixture* logic, int eventId, int count)
{

  double timeout = 2000;
  double starttime = vtkTimerLog::GetUniversalTime();

  // Client waits for an image to be sent from the server.
  while (vtkTimerLog::GetUniversalTime() - starttime < timeout)
  {
    Server.Logic->PeriodicProcess();
    Client.Logic->PeriodicProcess();
    vtksys::SystemTools::Delay(5);

    if (igtlio::contains(logic->ReceivedEvents, eventId, count))
    {
      return true;
    }

  }
  std::cout << "FAILURE: Expected event: " << eventId << " " << count << "times." << std::endl;

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



int ClientServerFixture::CreateTestFrame(vtkImageData* image )
{
  int width = 512, height = 512;
  image->SetSpacing(1, 1, 1);
  image->SetExtent(0, width-1, 0, height-1, 0, 0);
  image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
  
  unsigned char* ptr = reinterpret_cast<unsigned char*>(image->GetScalarPointer());
  unsigned char color = 108;
  
  for(int i = 0 ; i< width*height*3; i++)
  {
    *ptr = color%256;
    color++;
    ptr++;
  }
  return 1;
}

vtkSmartPointer<vtkMatrix4x4> ClientServerFixture::CreateTestTransform()
{
  vtkSmartPointer<vtkMatrix4x4> transform = vtkSmartPointer<vtkMatrix4x4>::New();
  transform->Identity();
  return transform;
}
