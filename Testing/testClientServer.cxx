#include <string>
#include "vtkIGTLIOLogic.h"
#include "vtkIGTLIOConnector.h"
#include "vtkTimerLog.h"

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
      return 0;
    }
    if (client.Connector->GetState() == vtkIGTLIOConnector::STATE_OFF)
    {
      std::cout << "FAILURE to connect to server" << std::endl;
      return 1;
    }
  }

  std::cout << "TIMEOUT connecting to server" << std::endl;
  return 1;


  return 0;
}
