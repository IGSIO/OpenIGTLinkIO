#include <string>
#include "vtkIGTLIOLogic.h"
#include "vtkIGTLIOConnector.h"
#include "vtkTimerLog.h"
#include "vtkIGTLIOImageDevice.h"
#include "vtkImageData.h"
#include "igtlImageConverter.h"
#include "vtkMatrix4x4.h"
#include <vtksys/SystemTools.hxx>

struct LogicFixture
{
  LogicFixture()
  {
  }

  void startClient()
  {
    Logic = vtkIGTLIOLogicPointer::New();
    Connector = Logic->CreateConnector();
    Connector->SetTypeClient(Connector->GetServerHostname(), Connector->GetServerPort());
    std::cout << "Starting CLIENT connector " << Connector.GetPointer()  << std::endl;
    Connector->Start();
  }
  void startServer()
  {
    Logic = vtkIGTLIOLogicPointer::New();
    Connector = Logic->CreateConnector();
    Connector->SetTypeServer(Connector->GetServerPort());
    std::cout << "Starting SERVER connector " << Connector.GetPointer() << std::endl;
    Connector->Start();
  }

  vtkSmartPointer<vtkIGTLIOImageDevice> CreateDummyImageDevice()
  {
    vtkSmartPointer<vtkIGTLIOImageDevice> imageDevice;
    imageDevice = vtkIGTLIOImageDevice::SafeDownCast(vtkIGTLIOImageDeviceCreator::New()->Create("TestDevice_Image"));
    igtl::ImageConverter::ContentData imageContent;
    imageContent.image = vtkSmartPointer<vtkImageData>::New();
    imageContent.image->SetSpacing(1.5, 1.2, 1);
    imageContent.image->SetExtent(0, 19, 0, 49, 0, 1);
    imageContent.image->AllocateScalars(VTK_UNSIGNED_CHAR, 2);
    imageContent.transform = vtkSmartPointer<vtkMatrix4x4>::New();
    imageContent.transform->Identity();

  //	int scalarSize = dim[0]*dim[1]*dim[2]*components;
  //	TYPE* ptr = reinterpret_cast<TYPE*>(data->GetScalarPointer());
  //	std::fill(ptr, ptr+scalarSize, initValue);

    imageDevice->SetContent(imageContent);
    return imageDevice;
  }

  vtkIGTLIOLogicPointer Logic;
  vtkIGTLIOConnectorPointer Connector;
};

int main(int argc, char **argv)
{
  LogicFixture server;
  server.startServer();

  LogicFixture client;
  client.startClient();
  client.Connector->DebugOn();

  double timeout = 2;
  double starttime = vtkTimerLog::GetUniversalTime();

  // Client connects to server.
  while (vtkTimerLog::GetUniversalTime() - starttime < timeout)
  {
    server.Logic->PeriodicProcess();
    client.Logic->PeriodicProcess();
    vtksys::SystemTools::Delay(5);

    if (client.Connector->GetState() == vtkIGTLIOConnector::STATE_CONNECTED)
    {
      std::cout << "SUCCESS: connected to server" << std::endl;
      break;
    }
    if (client.Connector->GetState() == vtkIGTLIOConnector::STATE_OFF)
    {
      std::cout << "FAILURE to connect to server" << std::endl;
      return 1;
    }
  }

  if (client.Logic->GetNumberOfDevices() != 0)
  {
    std::cout << "ERROR: Client has devices before they have been added or fundamental error!" << std::endl;
    return 1;
  }

  std::cout << "********** CONNECTION DONE *********" << std::endl;

  vtkSmartPointer<vtkIGTLIOImageDevice> imageDevice = server.CreateDummyImageDevice();
  server.Connector->AddDevice(imageDevice);
  server.Connector->SendMessage(CreateDeviceKey(imageDevice));

  starttime = vtkTimerLog::GetUniversalTime();
  // Client waits for an image to be sent from the server.
  while (vtkTimerLog::GetUniversalTime() - starttime < timeout)
  {
    server.Logic->PeriodicProcess();
    client.Logic->PeriodicProcess();
    vtksys::SystemTools::Delay(5);

    if (client.Logic->GetNumberOfDevices() > 0)
    {
      std::cout << "SUCCESS: Found devices in the client." << std::endl;
      return 0;
    }
  }

  std::cout << "ERROR: TIMEOUT receiving image" << std::endl;
  return 1;
}
