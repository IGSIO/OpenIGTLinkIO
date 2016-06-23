#include <string>
#include "vtkIGTLIOLogic.h"
#include "vtkIGTLIOConnector.h"
#include "vtkTimerLog.h"
#include <vtksys/SystemTools.hxx>
#include "IGTLIOFixture.h"

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
    vtksys::SystemTools::Delay(5);

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
