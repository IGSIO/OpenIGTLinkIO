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
  igtlioLogicFixture* self = reinterpret_cast<igtlioLogicFixture*>(clientdata);
  self->Session->PrintSelf(std::cout, vtkIndent(1));
  self->ReceivedEvents.push_back(eid);
}

igtlioLogicFixture::igtlioLogicFixture()
{
  Logic = igtlioLogicPointer::New();
  ReceivedEvents = std::vector<int>(1000);
  ReceivedEvents.clear();
  LogicEventCallback = vtkSmartPointer<vtkCallbackCommand>::New();
  LogicEventCallback->SetCallback(onReceivedEventFunc);
  LogicEventCallback->SetClientData(this);

  Logic->AddObserver(igtlioLogic::NewDeviceEvent, LogicEventCallback);
  Logic->AddObserver(igtlioLogic::RemovedDeviceEvent, LogicEventCallback);
  Logic->AddObserver(igtlioLogic::DeviceModifiedEvent, LogicEventCallback);
  Logic->AddObserver(igtlioCommand::CommandReceivedEvent, LogicEventCallback);
  Logic->AddObserver(igtlioCommand::CommandResponseEvent, LogicEventCallback);

}

void igtlioLogicFixture::startClient()
{
  Session = Logic->ConnectToServer("localhost", -1, IGTLIO_ASYNCHRONOUS);
  Connector = Session->GetConnector();
}

void igtlioLogicFixture::startServer()
{
  Session = Logic->StartServer(-1, IGTLIO_ASYNCHRONOUS);
  Connector = Session->GetConnector();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


bool igtlioClientServerFixture::ConnectClientToServer()
{
  Server.startServer();
  Client.startClient();

  double timeoutSec = 2.0;
  double starttime = vtkTimerLog::GetUniversalTime();

  // Client connects to server.
  while (vtkTimerLog::GetUniversalTime() - starttime < timeoutSec)
  {
    Server.Logic->PeriodicProcess();
    Client.Logic->PeriodicProcess();
    vtksys::SystemTools::Delay(5);

    if (Client.Connector->GetState() == igtlioConnector::STATE_CONNECTED)
    {
      std::cout << "SUCCESS: connected to server" << std::endl;
      return true;
    }
    if (Client.Connector->GetState() == igtlioConnector::STATE_OFF)
    {
      std::cout << "FAILURE to connect to server" << std::endl;
      return false;
    }
  }

  std::cout << "TIMEOUT connecting to server" << std::endl;
  return false;
}

bool igtlioClientServerFixture::LoopUntilEventDetected(igtlioLogicFixture* logic, int eventId, int count)
{

  double timeoutSec = 2.0;
  double starttime = vtkTimerLog::GetUniversalTime();

  // Client waits for an image to be sent from the server.
  while (vtkTimerLog::GetUniversalTime() - starttime < timeoutSec)
  {
    Server.Logic->PeriodicProcess();
    Client.Logic->PeriodicProcess();
    vtksys::SystemTools::Delay(5);

    if (igtlioContains(logic->ReceivedEvents, eventId, count))
    {
      return true;
    }

  }
  std::cout << "FAILURE: Expected event: " << eventId << " " << count << "times." << std::endl;

  return false;
}

vtkSmartPointer<vtkImageData> igtlioClientServerFixture::CreateTestImage()
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



int igtlioClientServerFixture::CreateTestFrame(vtkImageData* image )
{
  int width = 512, height = 512;
  image->Delete();
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

vtkSmartPointer<vtkMatrix4x4> igtlioClientServerFixture::CreateTestTransform()
{
  vtkSmartPointer<vtkMatrix4x4> transform = vtkSmartPointer<vtkMatrix4x4>::New();
  transform->Identity();
  return transform;
}
