#include <string>
#include "vtkIGTLIOLogic.h"
#include "vtkIGTLIOConnector.h"
#include "vtkTimerLog.h"
#include "vtkIGTLIOImageDevice.h"

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
    Connector->Start();
  }
  void startServer()
  {
    Logic = vtkIGTLIOLogicPointer::New();
    Connector = Logic->CreateConnector();
    Connector->SetTypeServer(Connector->GetServerPort());
    Connector->Start();
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

  double timeout = 2;
  double starttime = vtkTimerLog::GetUniversalTime();

  while (vtkTimerLog::GetUniversalTime() - starttime < timeout)
  {
    server.Logic->PeriodicProcess();
    client.Logic->PeriodicProcess();

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

  server.Connector->AddDevice(vtkIGTLIOImageDeviceCreator::New()->Create("TestDevice_Image"));

  starttime = vtkTimerLog::GetUniversalTime();
  while (vtkTimerLog::GetUniversalTime() - starttime < timeout)
  {
    server.Logic->PeriodicProcess();
    client.Logic->PeriodicProcess();

    if (client.Logic->GetNumberOfDevices() > 0)
    {
      std::cout << "SUCCESS: Found devices in the client." << std::endl;
      return 0;
    }

  }


  std::cout << "ERROR: TIMEOUT receiving image" << std::endl;
  return 1;


  return 0;
}
